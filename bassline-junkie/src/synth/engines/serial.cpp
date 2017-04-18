/*
 * sharedfutures.cpp
 *
 *  Created on: Apr 12, 2017
 *      Author: alax
 */

#include "serial.h"

serial::serial() {
	stick_this_thread_to_core(councyrent_cores[1]);
}
serial::~serial() {

}


void serial::process(std::array<synth, voices_count> &voices) {

	voices[0].process();
	voices[1].process();

}

