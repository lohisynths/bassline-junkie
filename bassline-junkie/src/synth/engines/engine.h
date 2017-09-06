#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include "../utils/SerialReceiver.h"
#include "../utils/MidiReceiver.h"
#include "../utils/MidiReceiverRt.h"
#include <vector>
#include <algorithm>
#include "../config.h"
#include "../voice.h"
#include "thread.h"

const unsigned int format_bits = 32; //snd_pcm_format_width(*m_format);
const unsigned int maxval = (1U << (format_bits - 1U)) - 1U;

class Engine {

private:
	std::vector<thread*> cores;
	std::array<Voice, voices_count> &m_voices;
	std::array<stk::StkFloat, 512> output_float;



	MidiReceiverRt messager;
	std::vector<std::pair<MidiMessage, int>> notes;
	std::vector<int> free_cores;


public:
	Engine(std::array<Voice, voices_count> &voices) : m_voices(voices)
	{
		// ConditionalVar(vector of voices, first cpu, number of first voice, number of voices to process


		thread_info info={1,0,2};


		cores.push_back( new thread(m_voices, info) );
		info={2,2,3};
		cores.push_back( new thread(m_voices, info) );
		info={3,5,3};
		cores.push_back( new thread(m_voices, info) );

		free_cores.push_back(0);
		free_cores.push_back(1);
		free_cores.push_back(2);
		free_cores.push_back(3);
		free_cores.push_back(4);
		free_cores.push_back(5);
		free_cores.push_back(6);
		free_cores.push_back(7);
	}

	std::array<stk::StkFloat, 512> &process()
	{
		for(auto core : cores)
			core->request();
		for(auto core : cores)
			core->wait();


		for(int i = 0; i < 100; i++)
			updateMessages(m_voices);

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


	void noteOff(MidiMessage* msg, std::array<Voice, voices_count> &voices)
	{
		for (size_t i = 0; i < notes.size(); i++)
		{
			auto &note = notes[i].first;
			auto core = notes[i].second;

			if (note.m_val_1 == msg->m_val_1)
			{
				voices[core].message(msg);
				notes.erase(notes.begin() + i);
				free_cores.push_back(core);
			}
		}
	}

	void noteOn(MidiMessage* msg, std::array<Voice, voices_count> &voices)
	{

		if (msg->m_val_2 != 0)
		{
			if (notes.size() < voices_count)
			{
				//noteOn(msg, voices);

				auto core = free_cores.back();
				free_cores.pop_back();

				notes.push_back(std::make_pair(*msg, core));

				auto &note = notes.back().first;
				auto core_nr = notes.back().second;

				voices[core_nr].message(&note);

			}
		}
		else
		{
			noteOff(msg, voices);
		}


	}

	void updateMessages(std::array<Voice, voices_count> &voices)
	{
		auto msg = messager.getMessage();
		if (msg)
		{

			if (msg->m_type != MidiMessage::NO_MESSAGE)
			{
				switch (msg->m_type)
				{

				case MidiMessage::Type::NOTE_ON:
				{
					if(msg->m_val_2!=0)
					{
						noteOn(msg, voices);
						break;
					}
					else
						msg->m_type=MidiMessage::Type::NOTE_OFF;
				} // ignore warning NoteOn with vel 0 = NoteOff

				case MidiMessage::Type::NOTE_OFF:
				{
					noteOff(msg, voices);

					break;       // and exits the switchNOTE_ON
				}
				case MidiMessage::Type::CC:
				{
					for(auto &voice : voices)
						voice.message(msg);

					break;       // and exits the switchNOTE_ON
				}
				case MidiMessage::Type::NO_MESSAGE:
					break;       // and exits the switchNOTE_ON


				};
			}
		}

	}



};
