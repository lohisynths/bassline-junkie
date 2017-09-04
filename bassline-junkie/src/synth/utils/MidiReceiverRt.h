/*
 * MidiReceiverRt.h
 *
 *  Created on: Jul 9, 2017
 *      Author: alax
 */

#ifndef SRC_SYNTH_UTILS_MIDIRECEIVERRT_H_
#define SRC_SYNTH_UTILS_MIDIRECEIVERRT_H_
#include "RtMidi.h"
#include "MidiMessage.h"

class MidiReceiverRt {
public:
	MidiReceiverRt();
	virtual ~MidiReceiverRt();
	MidiMessage* getMessage();


	int parse(uint8_t input);
	void probe();

private:
	  RtMidiIn midiin;
	  std::vector<unsigned char> message;
	  MidiMessage msg, msg_out;
};

#endif /* SRC_SYNTH_UTILS_MIDIRECEIVERRT_H_ */
