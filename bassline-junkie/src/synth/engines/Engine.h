/*
 * Engine.h
 *
 *  Created on: Apr 12, 2017
 *      Author: alax
 */

#ifndef SRC_SYNTH_ENGINE_H_
#define SRC_SYNTH_ENGINE_H_

#include "../voice.h"
#include "../config.h"

static void DoNotOptimize(int const& value)
{
	asm volatile("" : : "g"(value) : "memory");
}

#include "../utils/SerialReceiver.h"
#include "../utils/MidiReceiver.h"
#include <vector>
#include <algorithm>
#include "../config.h"

class Engine
{
public:
	virtual std::array<uint32_t, 512> &process(
			std::array<Voice, voices_count> &voices) = 0;

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
	SerialReceiver messager;

//	MidiReceiver messager;
	std::vector<std::pair<MidiMessage, int>> notes;
	std::vector<int> free_cores;

};

#endif /* SRC_SYNTH_ENGINE_H_ */
