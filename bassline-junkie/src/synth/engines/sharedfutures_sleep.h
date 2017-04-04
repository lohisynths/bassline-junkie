/*
 * sharedfutures.h
 *
 *  Created on: Apr 12, 2017
 *      Author: alax
 */

#ifndef SRC_SYNTH_ENGINES_SHAREDFUTURES_SLEEP_H_
#define SRC_SYNTH_ENGINES_SHAREDFUTURES_SLEEP_H_

#include "Engine.h"
#include "../concurency_helpers.h"


#include <future>

class shared_futures_sleep {
public:
	shared_futures_sleep();
	virtual ~shared_futures_sleep();


	void process(std::array<synth, voices_count> &voices);


};

#endif /* SRC_SYNTH_ENGINES_SHAREDFUTURES_H_ */
