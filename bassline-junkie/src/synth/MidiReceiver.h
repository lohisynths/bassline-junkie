/*
 * MidiReceiver.h
 *
 *  Created on: May 6, 2017
 *      Author: alax
 */

#ifndef MIDIRECEIVER_H_
#define MIDIRECEIVER_H_

#include <alsa/asoundlib.h>     /* Interface to the ALSA system */
#include <iostream>



class MidiMessage
{
public:
	enum Type{
		NOTE_ON,
		NOTE_OFF,
		CC,
		NO_MESSAGE
	};

	Type m_type=NO_MESSAGE;
	char m_val_1=0;
	char m_val_2=0;

	uint8_t count=0;
	uint8_t time=0;

	void reset()
	{
		m_type=NO_MESSAGE;
		m_val_1=0;
		m_val_2=0;
		count=0;
	}

	void print()
	{
		if (m_type != MidiMessage::NO_MESSAGE)
		{
			std::cout << "Message ";

			switch (m_type)
			{
			case MidiMessage::Type::CC:
				std::cout << "MidiMessage::Type::CC"; // prints "1"
				break;       // and exits the switchNOTE_ON
			case MidiMessage::Type::NOTE_ON:
				if (m_val_2 == 0)
					std::cout << "MidiMessage::Type::NOTE_OFF"; // prints "1"
				else
					std::cout << "MidiMessage::Type::NOTE_ON"; // prints "1"
				break;
			case MidiMessage::Type::NOTE_OFF:
				std::cout << "MidiMessage::Type::NOTE_OFF"; // prints "1"
				break;
			case MidiMessage::Type::NO_MESSAGE:
				std::cout << "MidiMessage::Type::NO_MESSAGE"; // prints "1"
				break;       // and exits the switchNOTE_ON
			}

			std::cout << " param " << +m_val_1 << " val "
					<< +m_val_2 << std::endl;
		}
	}

};



class MidiReceiver
{
public:
	MidiReceiver();
	virtual ~MidiReceiver();

	MidiMessage* getMessage();
	void printMsg(MidiMessage *message);


private:
	MidiMessage msg, msg_out;

	snd_rawmidi_t* midiin = NULL;
	int parse(char input);



};

#endif /* MIDIRECEIVER_H_ */