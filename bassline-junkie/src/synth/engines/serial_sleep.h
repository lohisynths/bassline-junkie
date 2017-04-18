/*
 * sharedfutures.h
 *
 *  Created on: Apr 12, 2017
 *      Author: alax
 */

#ifndef SRC_SYNTH_ENGINES_SERIAL_SLEEP_H_
#define SRC_SYNTH_ENGINES_SERIAL_SLEEP_H_

#include "Engine.h"


#include "../concurency_helpers.h"

#include <chrono>
#include <array>
#include <thread> //sleep

class serial_sleep : public Engine {
public:
	serial_sleep();
	virtual ~serial_sleep();


	void process(std::array<synth, voices_count> &voices);


};

#endif /* SRC_SYNTH_ENGINES_SHAREDFUTURES_H_ */
