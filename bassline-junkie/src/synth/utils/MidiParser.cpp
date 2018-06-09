/*
 * MidiParser.cpp
 *
 *  Created on: Sep 19, 2017
 *      Author: alax
 */

#include "MidiParser.h"

MidiParser::MidiParser()
{
	// TODO Auto-generated constructor stub

}

MidiParser::~MidiParser()
{
	// TODO Auto-generated destructor stub
}



void MidiParser::midiHandler(std::deque<uint8_t> &bytes)
{

	size_t size = bytes.size();
	if(size > 2)
	{
		//std::cout << "midiHandler \n";

		// Parse the MIDI bytes ... only keep MIDI channel messages.
		uint8_t byte = bytes.front();
		bytes.pop_front();
		//std::cout << "midiHandler " << std::hex << +byte <<  " "<<  std::dec  << +byte << " popped " << +bytes.size() <<  "\n";

		if (byte > 239)
		{
			std::cout << "midiHandler wrong packet byte > 239\n";
			return;

		}


		uint8_t type = byte & 0xF0;
		uint8_t channel = byte & 0x0F;

		byte = bytes.front();
		bytes.pop_front();
		//std::cout << "midiHandler " << std::hex  << +byte << " "<<  std::dec  << +byte  << " popped " << +bytes.size() <<  "\n";

		uint8_t intValue0 = byte;

		if ((type != 0xC0) && (type != 0xD0))
		{
			if (size < 3)
			{
				std::cout << "midiHandler size < 3\n";
				return;
			}
			byte = bytes.front();
			bytes.pop_front();
			//std::cout << "midiHandler " << std::hex  << +byte << " "<<  std::dec  << +byte << " popped " << +bytes.size() <<  "\n";

			uint8_t intValue1 = byte;

			if (type == 0xb0)
			{
				tmp.m_type = MidiMessage::CC;
				tmp.m_val_1 = intValue0;
				tmp.m_val_2 = intValue1;
				tmp.channel = channel;
//				std::cout << "midi CC message on channel " << +channel << " "
//						<< +intValue0 << " " << +intValue1 << " received\n";
				msg.push_back(tmp);
			}
			else if (type == 0x90)
			{
				if(intValue1!=0)
				{
					tmp.m_type = MidiMessage::NOTE_ON;
				}
				else
				{
					tmp.m_type = MidiMessage::NOTE_OFF;
				}

				tmp.m_val_2 = intValue1;
				tmp.m_val_1 = intValue0;
				tmp.channel = channel;

//				std::cout << "midi NOTE ON message on channel " << +channel << " "
//						<< +intValue0 << " " << +intValue1 << " received\n";

				msg.push_back(tmp);
			}
			else if (type == 0x80)
			{
				tmp.m_type = MidiMessage::NOTE_OFF;

				tmp.m_val_2 = intValue1;
				tmp.m_val_1 = intValue0;
				tmp.channel = channel;

//				std::cout << "midi NOTE ON message on channel " << +channel << " "
//						<< +intValue0 << " " << +intValue1 << " received\n";

				msg.push_back(tmp);
			}

			else {
				std::cout << "midi reset " << std::endl;
				tmp.reset();
			}
		}
	}
}


MidiMessage* MidiParser::getMessage()
{
	MidiMessage* output=nullptr;

	if(msg.size() > 0)
	{
		tmp = msg.front();
		msg.pop_front();

		output = &tmp;
	}

	return output;

}
