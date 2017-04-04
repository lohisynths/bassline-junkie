/*
 * Engine.h
 *
 *  Created on: Apr 12, 2017
 *      Author: alax
 */

#ifndef SRC_SYNTH_ENGINE_H_
#define SRC_SYNTH_ENGINE_H_

#include "../synth.h"

const size_t voices_count = 4;

class Engine {
public:
	Engine();
	virtual ~Engine() = 0;

	virtual void process(std::array<synth, voices_count> &voices) = 0 ;
};


#endif /* SRC_SYNTH_ENGINE_H_ */
