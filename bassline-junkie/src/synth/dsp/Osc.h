	/*
 * Osc.h
 *
 *  Created on: Oct 3, 2016
 *      Author: alax
 */

#ifndef OSC_H_
#define OSC_H_

#include "Noise.h"
#include "WavetableOscillator.h"

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
	bassline::dsp::WavetableOscillator m_sine;
	bassline::dsp::WavetableOscillator m_saw;
	bassline::dsp::WavetableOscillator m_square;
	bassline::dsp::Noise m_noise;
};

#endif /* OSC_H_ */
