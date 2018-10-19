/*
 * Lfo.h
 *
 *  Created on: Oct 5, 2016
 *      Author: alax
 */

#ifndef SRC_LFO_H_
#define SRC_LFO_H_

#include "Stk.h"
#include "PolyBLEPOscillator/PolyBLEPOscillator.h"

class Lfo {
public:
	Lfo();
	virtual ~Lfo();

	void setFrequency(stk::StkFloat freq);
	void setShape(int shape);

	stk::StkFloat tick();
	stk::StkFloat get_value();

	void reset();

private:
	PolyBLEPOscillator osc;

};

#endif /* SRC_LFO_H_ */
