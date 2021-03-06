#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
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
	std::array<thread<buffer_size>, max_cores> cores;
	std::array<stk::StkFloat, buffer_size> output_float;
	std::array<Voice<buffer_size>, voices_count> m_voices;

	SerialReceiver messager;
	//MidiReceiverRt messager;
	std::vector<std::pair<MidiMessage, int>> notes;
	std::vector<int> free_voices;

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
		for(auto &core : cores)
			core.request();
		for(auto &core : cores)
			core.wait();

		for(int i=0;i<100;i++)
		{
			updateMessages();
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
};
