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
#include "MidiMessage.h"

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
	int parse(uint8_t input);



};

#endif /* MIDIRECEIVER_H_ */
