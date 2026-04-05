/*
 * MidiParser.h
 *
 *  Created on: Sep 19, 2017
 *      Author: alax
 */

#ifndef SRC_SYNTH_UTILS_MIDIPARSER_H_
#define SRC_SYNTH_UTILS_MIDIPARSER_H_
#include <deque>
#include <stdint.h>
#include <stddef.h>

#include "MidiMessage.h"

class MidiParser
{
public:
	MidiParser();
	virtual ~MidiParser();

	void midiHandler(std::deque<uint8_t> &bytes);

	MidiMessage* getMessage();

	std::deque<MidiMessage> msg;

	MidiMessage tmp;

private:
	void parsePendingBytes();
	size_t getChannelMessageSize(uint8_t status) const;

	std::deque<uint8_t> pending_bytes;
	uint8_t running_status = 0;

};

#endif /* SRC_SYNTH_UTILS_MIDIPARSER_H_ */
