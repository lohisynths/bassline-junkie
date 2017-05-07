/*
 * MidiReceiver.cpp
 *
 *  Created on: May 6, 2017
 *      Author: alax
 */

#include "MidiReceiver.h"

MidiReceiver::MidiReceiver()
{
	int status;
	int mode = SND_RAWMIDI_NONBLOCK;
	const char* portname = "hw:1,0,0";  // see alsarawportlist.c example program
	if ((status = snd_rawmidi_open(&midiin, NULL, portname, mode)) < 0)
	{
		std::cout << "Problem opening MIDI input: " << snd_strerror(status);
		exit(1); //TODO:: !
	}
}

MidiReceiver::~MidiReceiver()
{
	snd_rawmidi_close(midiin);
	midiin = NULL;    // snd_rawmidi_close() does not clear invalid pointer,
}

int MidiReceiver::parse(char input)
{
	if (input == 0xb0 || input == 0xb9)
	{
		msg.m_type = MidiMessage::Type::CC;
		msg.count++;
	}
	else if (input == 0x90 || input == 0x99)
	{
		msg.m_type = MidiMessage::Type::NOTE_ON;
		msg.count++;
	}
	else if (input == 0x80)
	{
		msg.m_type = MidiMessage::Type::NOTE_OFF;
		msg.count++;
	}
	else if (msg.m_type != MidiMessage::Type::NO_MESSAGE)
	{
		if (msg.count == 1)
		{
			msg.m_val_1 = input;
			msg.count++;
		}
		else if (msg.count == 2)
		{
			msg.m_val_2 = input;
			return 1;
		}
		else
		{
			msg.reset();
			std::cout << "wrong message" << std::endl;
		}
	}
	else
	{
		msg.reset();
		std::cout << "unknown message" << std::endl;
	}
	return 0;
}

MidiMessage* MidiReceiver::getMessage()
{
	MidiMessage* output= nullptr;

	char buffer[1] = { };
	int status = 0;

	status = snd_rawmidi_read(midiin, buffer, 1);
	if ((status < 0) && (status != -EBUSY) && (status != -EAGAIN))
	{
		std::cout << "Problem reading MIDI input: " << snd_strerror(status);
	}
	else if (buffer[0] >= 0 && (status != -EAGAIN))
	{
		//std::cout << "0x" << std::hex << +buffer[0] << " " << std::dec;
		if (parse(buffer[0]))
		{
			//std::cout << std::endl;
			msg_out = msg;
			output = &msg_out;
			msg.reset();
			//std::cout << "    get ";
			//output->print();

		}
	}

	return output;
}




void MidiReceiver::printMsg(MidiMessage *message)
{

}

