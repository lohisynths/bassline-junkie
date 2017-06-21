/*
 * SerialReceiver.h
 *
 *  Created on: Jun 20, 2017
 *      Author: alax
 */

#ifndef SRC_SYNTH_UTILS_SERIALRECEIVER_H_
#define SRC_SYNTH_UTILS_SERIALRECEIVER_H_

#include <atomic>
#include <thread>

#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>


#include <termios.h>

#include <iostream>
#include <unistd.h>

#include "MidiReceiver.h"

class SerialReceiver {
public:
	SerialReceiver();
	virtual ~SerialReceiver();

	int set_interface_attribs(int fd, int speed, int parity);
	void set_blocking(int fd, int should_block);

	void writeBytes(const char *data, size_t size);

	void readBytes();

	MidiMessage* getMessage();


private:

	char buf[100]={};

	std::atomic<size_t> n;
	std::thread *t;

	MidiMessage msg, msg_out;



	int parse(char input);


	int fd;
};

#endif /* SRC_SYNTH_UTILS_SERIALRECEIVER_H_ */
