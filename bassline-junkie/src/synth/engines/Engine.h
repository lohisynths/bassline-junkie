/*
 * Engine.h
 *
 *  Created on: Apr 12, 2017
 *      Author: alax
 */

#ifndef SRC_SYNTH_ENGINE_H_
#define SRC_SYNTH_ENGINE_H_

#include "../synth.h"

const size_t voices_count = 4;

static void DoNotOptimize(int const& value) {
	asm volatile("" : : "g"(value) : "memory");
}

#include "../MidiReceiver.h"
#include <vector>
#include <algorithm>

const size_t max_voices = 4;

class Engine
{
public:
	virtual void process(std::array<synth, voices_count> &voices) = 0;

	void remove_messages(std::array<synth, voices_count> &voices)
	{
		for (size_t i=0;i < notes.size();i++)
		{
			auto &note = notes[i].first;
			auto core = notes[i].second;

			if (note.m_type == MidiMessage::NO_MESSAGE)
			{
				note.time++;
				if(note.time > 10)
				{
					voices[core].noteOff();
					notes.erase(notes.begin() + i);
				//	std::cout << "core " << core << " freed of total " << notes.size() << " voices\n";

					//std::cout << +note.m_val_1 << " " << +note.m_type << " " << core << std::endl;
					break;
				}
			}

		}

	}

	void updateMessages(std::array<synth, voices_count> &voices)
	{
		auto msg = messager.getMessage();
		if(msg)
		{
			//std::cout << " main loop ";
			msg->print();
			message(voices, msg);
		}

		remove_messages(voices);
	}

	void message(std::array<synth, voices_count> &voices, MidiMessage *msg)
	{
		if (msg->m_type == MidiMessage::NOTE_ON)
		{
			if (notes.size() < max_voices)
			{
				notes.push_back(std::make_pair(*msg, notes.size()));

				auto &note = notes.back().first;
				auto core_nr = notes.back().second;

				voices[core_nr].noteOn(note.m_val_1,note.m_val_2);

			//	std::cout << "core " << core_nr << " bussy of total " << notes.size() << " voices\n";

			}
		}
		else if (msg->m_type == MidiMessage::NOTE_OFF)
		{
			for (size_t i=0;i < notes.size();i++)
			{
				auto &note = notes[i].first;
				auto core_nr = notes[i].second;

				if(note.m_val_1 == msg->m_val_1)
				{
			//		std::cout << "core " << core_nr << " free of total " << notes.size() << " voices\n";

					note.m_type = MidiMessage::NO_MESSAGE;
				}
			}



//			notes.erase(std::remove_if(notes.begin(), notes.end(), [](auto &msg)
//			{	return msg.first.m_type == MidiMessage::NO_MESSAGE;}), notes.end());

			//std::cout << "notes size " << notes.size() << "\n";

		}

	}
	MidiReceiver messager;

	std::vector<std::pair<MidiMessage, int>> notes;

};

#endif /* SRC_SYNTH_ENGINE_H_ */
