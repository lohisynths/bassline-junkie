	/*
 * Osc.h
 *
 *  Created on: Oct 3, 2016
 *      Author: alax
 */

#ifndef OSC_H_
#define OSC_H_

#include "Stk.h"
#include "BlitSaw.h"
#include "BlitSquare.h"
#include "Noise.h"
#include "SineWave.h"

using namespace stk;

class Osc
{
public:
	Osc();
	virtual ~Osc();
	double tick();

	void setFrequency(double freq);
	void set_sin_level(double level);
	void set_saw_level(double level);
	void set_sqr_level(double level);
	void set_noise_level(double level);
	void reset();


private:
	struct waves_level{
		double sin_level;
		double saw_level;
		double sqr_level;
		double rnd_level;
	};
	waves_level m_osc_ctrl;
	SineWave m_sine;
	BlitSaw m_saw;
	BlitSquare m_square;
	Noise m_noise;
};

#endif /* OSC_H_ */
