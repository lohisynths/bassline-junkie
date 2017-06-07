/*
 * Lfo.h
 *
 *  Created on: Oct 5, 2016
 *      Author: alax
 */

#ifndef SRC_LFO_H_
#define SRC_LFO_H_

#include <memory>

#include "Stk.h"
#include "PolyBLEPOscillator/PolyBLEPOscillator.h"

class Lfo {
public:
	Lfo();
	virtual ~Lfo();

	void setFrequency(stk::StkFloat freq);
	void setShape(int shape);

	stk::StkFloat tick();

	void reset();

private:
	std::unique_ptr<PolyBLEPOscillator>osc = std::make_unique<PolyBLEPOscillator>();

};

#endif /* SRC_LFO_H_ */
