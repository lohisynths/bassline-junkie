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


static void DoNotOptimize(int const& value) {
	asm volatile("" : : "g"(value) : "memory");
}

#include "../utils/MidiReceiver.h"
#include <vector>
#include <algorithm>
#include "../config.h"

class Engine
{
public:
	virtual std::array<uint32_t, 512> &process(std::array<Voice, voices_count> &voices) = 0;

	void updateMessages(std::array<Voice, voices_count> &voices)
	{
		auto msg = messager.getMessage();
		if(msg)
		{
			if (msg->m_type == MidiMessage::NOTE_ON)
			{
				bool noteOff=false;

				for(auto &note : notes )
				{
					if(note.first.m_val_1 == msg->m_val_1)
					{
						// midi: note off == note on , velocity =0
						if(note.second == 0)
						{
							note.first.m_type = MidiMessage::NOTE_OFF;
							noteOff=true;
						}
						else
						{
							// if note is already playing, do nothing
							noteOff=true;
						}
					}
				}

				if(!noteOff)
				{
					if (notes.size() < voices_count)
					{
						auto core = free_cores.back();
						free_cores.pop_back();

						notes.push_back(std::make_pair(*msg, core));

						auto &note = notes.back().first;
						auto core_nr = notes.back().second;

						voices[core_nr].message(&note);
					}
				}
			}


			if (msg->m_type == MidiMessage::NOTE_OFF)
			{
				for (size_t i=0;i < notes.size();i++)
				{
					auto &note = notes[i].first;
					if(note.m_val_1 == msg->m_val_1)
					{
						note.m_type = MidiMessage::NO_MESSAGE;
					}
				}
			}

			if(msg->m_type == MidiMessage::NOTE_ON && msg->m_type == MidiMessage::NOTE_OFF )
			{
				msg->print();

				for(auto &voice : voices)
				{
					voice.message(msg);
				}
			}
		}

		// clean messages
		// TODO: implement without dynamic allocation + smarter release
		for (size_t i=0;i < notes.size();i++)
		{
			auto &note = notes[i].first;
			auto core = notes[i].second;

			if (note.m_type == MidiMessage::NO_MESSAGE)
			{
				voices[core].noteOff();
				notes.erase(notes.begin() + i);
				free_cores.push_back(core);
			}
		}
	}

	MidiReceiver messager;
	std::vector<std::pair<MidiMessage, int>> notes;
	std::vector<int> free_cores;

};

#endif /* SRC_SYNTH_ENGINE_H_ */
