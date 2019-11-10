/*
 * Lfo.h
 *
 *  Created on: Oct 5, 2016
 *      Author: alax
 */

#ifndef SRC_LFO_H_
#define SRC_LFO_H_

#include "PolyBLEPOscillator/PolyBLEPOscillator.h"

class Lfo {
public:
	Lfo();
	virtual ~Lfo();

	void setFrequency(double freq);
	void setShape(int shape);

	double tick();
	double get_value();
	void noteOn();
	void reset();
    void set_sync(bool input) {
        sync = input;
    }

private:
	PolyBLEPOscillator osc;
	bool sync = false;

};

#endif /* SRC_LFO_H_ */
