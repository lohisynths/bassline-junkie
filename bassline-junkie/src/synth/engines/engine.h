#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <vector>
#include <array>

#include "../utils/SerialReceiver.h"
#include "../utils/MidiReceiver.h"
#include "../utils/MidiReceiverRt.h"
#include "../config.h"
#include "../voice.h"
#include "thread.h"


template<size_t voices_count,size_t buffer_size>
class Engine {

private:
	static constexpr int preset_sync_retry_interval_ms = 500;
	static constexpr uint8_t preset_dump_request_channel = 15;
	static constexpr uint8_t preset_dump_request_control = 127;
	static constexpr uint8_t preset_dump_request_value = 127;
	static constexpr size_t expected_preset_sync_cc_count =
			(OSC_NUMBER * OSC_PARAMS) +
			(ADSR_NUMBER * ADSR_PARAMS) +
			(FLT_NUMBER * FLT_PARAMS) +
			(LFO_NUMBER * LFO_PARAMS) +
			(MATRIX_MOD_MATRIX_ITEMS * MATIRX_MOD_PARAMS_COUNT) +
			1U; // VOL adds one extra master-volume CC to the preset dump.

	std::array<thread<buffer_size>, max_cores> cores;
	std::array<stk::StkFloat, buffer_size> output_float;
	std::array<Voice<buffer_size>, voices_count> m_voices;

	SerialReceiver messager;
	//MidiReceiverRt messager;
	std::vector<std::pair<MidiMessage, int>> notes;
	std::vector<int> free_voices;
	bool preset_sync_complete_ = false;
	size_t preset_sync_cc_count_ = 0;
	size_t preset_sync_request_count_ = 0;
	std::chrono::steady_clock::time_point last_preset_sync_request_at_ = std::chrono::steady_clock::time_point::min();

public:
	Engine() {
		for(int i = 0; i < max_cores; i++) {
	        cores[i].set_cpu_affinity(i+first_cpu);
		}

		for(int voice_nr = 0; voice_nr < voices_count; voice_nr++) {
		  int core_nr = (max_cores - (voice_nr % max_cores));
          std::cout << "new thread created on core " << core_nr << std::endl;
          m_voices[voice_nr].set_voice_index(voice_nr);
          cores[core_nr - first_cpu].add_voice(&m_voices[voice_nr]);
          free_voices.push_back(voice_nr);
          std::cout << "core " << core_nr << " voice " << voice_nr << " pushed" << std::endl;
		}

		for(auto &core: cores) {
			std::cout << "core started" << std::endl;
			core.start();
		}
	}

	std::array<stk::StkFloat, buffer_size> &process()
	{
		if (!g_play)
		{
			std::fill(std::begin(output_float), std::end(output_float), 0);
			return output_float;
		}

		maybe_request_preset_dump_();

		for(int i=0;i<100;i++)
		{
			updateMessages();
		}

		for(auto &core : cores)
			core.request();
		for(auto &core : cores)
			core.wait();

		if (!g_play)
		{
			std::fill(std::begin(output_float), std::end(output_float), 0);
			return output_float;
		}

		// reset buffer
		std::fill(std::begin(output_float), std::end(output_float), 0);
		// fill output buffer with data
		for (auto &voice : m_voices)
		{
			auto &voice_data = voice.get_array();
			auto ciabej = [](stk::StkFloat &output, stk::StkFloat &input){ output += input; return output;};
			std::transform(output_float.begin(), output_float.end(), voice_data.begin(),output_float.begin(),ciabej);
		}
		return output_float;
	}

	void noteOff(MidiMessage* msg)
	{
		for (size_t i = 0; i < notes.size(); i++)
		{
			auto &note = notes[i].first;
			auto core = notes[i].second;

			if (note.m_val_1 == msg->m_val_1)
			{
				m_voices[core].noteOff(msg->m_val_1,msg->m_val_2);
				notes.erase(notes.begin() + i);
				free_voices.push_back(core);
			}
		}
	}

	void noteOn(MidiMessage* msg)
	{

		if (msg->m_val_2 != 0)
		{
			if (notes.size() < voices_count)
			{
				auto core = free_voices.back();
				free_voices.pop_back();

				notes.push_back(std::make_pair(*msg, core));

				auto note = &notes.back().first;
				auto core_nr = notes.back().second;

				m_voices[core_nr].noteOn(note->m_val_1,note->m_val_2);
			}
		}
		else
		{
			noteOff(msg);
		}
	}

	void updateMessages()
	{
		auto msg = messager.getMessage();
		if (msg)
		{
			//msg->print();

			if (msg->m_type != MidiMessage::NO_MESSAGE)
			{
				switch (msg->m_type)
				{

				case MidiMessage::Type::NOTE_ON:
				{
					if(msg->m_val_2!=0)
					{
						noteOn(msg);
						break;
					}
					else
						msg->m_type=MidiMessage::Type::NOTE_OFF;
				} // ignore warning NoteOn with vel 0 = NoteOff

				case MidiMessage::Type::NOTE_OFF:
				{
					noteOff(msg);

					break;       // and exits the switchNOTE_ON
				}
				case MidiMessage::Type::CC:
				{
					if (!preset_sync_complete_) {
						++preset_sync_cc_count_;
						if (preset_sync_cc_count_ >= expected_preset_sync_cc_count) {
							preset_sync_complete_ = true;
							std::cout << "preset sync complete after "
							          << preset_sync_request_count_
							          << " request(s)" << std::endl;
						}
					}

					for(auto &voice : m_voices) {
						if(msg->channel == 2) {
							voice.cc(msg->m_val_1+96,msg->m_val_2);
						} else {
							voice.cc(msg->m_val_1,msg->m_val_2);
						}
					}

					break;       // and exits the switchNOTE_ON
				}
				case MidiMessage::Type::NO_MESSAGE:
					break;       // and exits the switchNOTE_ON
				};
			}
		}
	}

	void maybe_request_preset_dump_()
	{
		if (preset_sync_complete_) {
			return;
		}

		const auto now = std::chrono::steady_clock::now();
		if ((preset_sync_request_count_ != 0U) &&
		    ((now - last_preset_sync_request_at_) < std::chrono::milliseconds(preset_sync_retry_interval_ms))) {
			return;
		}

		const uint8_t request_msg[3] = {
			static_cast<uint8_t>(0xB0U | preset_dump_request_channel),
			preset_dump_request_control,
			preset_dump_request_value,
		};
		messager.writeBytes(request_msg, sizeof(request_msg));
		last_preset_sync_request_at_ = now;
		++preset_sync_request_count_;
		preset_sync_cc_count_ = 0U;
		std::cout << "requesting preset dump via MIDI CC (" << preset_sync_request_count_
		          << ")" << std::endl;
	}
};
