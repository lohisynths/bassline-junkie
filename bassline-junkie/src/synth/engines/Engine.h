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
	Engine()
	{
		free_cores.push_back(0);
		free_cores.push_back(1);
		free_cores.push_back(2);
		free_cores.push_back(3);
	}

	virtual void process(std::array<synth, voices_count> &voices) = 0;


	void updateMessages(std::array<synth, voices_count> &voices)
	{
		auto msg = messager.getMessage();
		if(msg)
		{
			//msg->print();

			if (msg->m_type == MidiMessage::NOTE_ON)
			{
				if (notes.size() < max_voices)
				{
					auto core = free_cores.back();
					free_cores.pop_back();

					notes.push_back(std::make_pair(*msg, core));

					auto &note = notes.back().first;
					auto core_nr = notes.back().second;

					voices[core_nr].message(&note);

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
			else
			{
				msg->print();

				for(auto &voice : voices)
				{
					voice.message(msg);
				}
			}
		}

		for (size_t i=0;i < notes.size();i++)
		{
			auto &note = notes[i].first;
			auto core = notes[i].second;

			if (note.m_type == MidiMessage::NO_MESSAGE)
			{
				note.time++;
				if(note.time > 0)
				{
					voices[core].noteOff();
					notes.erase(notes.begin() + i);
					free_cores.push_back(core);
				//	std::cout << "core " << core << " freed of total " << notes.size() << " voices\n";

					//std::cout << +note.m_val_1 << " " << +note.m_type << " " << core << std::endl;
					break;
				}
			}
		}
	}



	MidiReceiver messager;

	std::vector<std::pair<MidiMessage, int>> notes;
	std::vector<int> free_cores;

};

#endif /* SRC_SYNTH_ENGINE_H_ */
