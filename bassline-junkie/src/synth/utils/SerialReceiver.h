/*
 * SerialReceiver.h
 *
 *  Created on: Jun 20, 2017
 *      Author: alax
 */

#ifndef SRC_SYNTH_UTILS_SERIALRECEIVER_H_
#define SRC_SYNTH_UTILS_SERIALRECEIVER_H_

#include <thread>
#include <mutex>
#include <deque>

#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <termios.h>
#include "spdlog/spdlog.h"


#include <iostream>
#include <unistd.h>

#include "../config.h"

#include "concurency_helpers.h"

#include "MidiMessage.h"
#include "MidiParser.h"

//TODO!!: IMPORTANT no memory allocations after lauynch!!

class SerialReceiver {
public:
	SerialReceiver();
	virtual ~SerialReceiver();

	int set_interface_attribs(int fd, int speed, int parity);
	void set_blocking(int fd, int should_block);

	void writeBytes(const uint8_t *data, size_t size);

	void readBytes();

	MidiMessage* getMessage();

	MidiMessage* midiHandler(std::deque<uint8_t> &bytes);

	MidiParser midi_parser;

private:
	void worker_thread();

	void start();
	void stop();
	std::thread *t;
	std::mutex m;

	std::deque<uint8_t> vector_char;
	std::deque<uint8_t> &getBuffer();

	bool running;

	int fd;
	//pointer because of eclipse bug, not indexing shr_ptr
	spdlog::logger *logger=nullptr;

};

#endif /* SRC_SYNTH_UTILS_SERIALRECEIVER_H_ */
