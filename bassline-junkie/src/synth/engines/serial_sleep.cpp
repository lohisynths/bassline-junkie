/*
 * sharedfutures.cpp
 *
 *  Created on: Apr 12, 2017
 *      Author: alax
 */

#include "serial_sleep.h"

serial_sleep::serial_sleep() {

}
serial_sleep::~serial_sleep() {

}


void serial_sleep::process(std::array<synth, voices_count> &voices) {

	using namespace std::chrono_literals;
	std::this_thread::sleep_for(8ms);

}

