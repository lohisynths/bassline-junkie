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
	void set_pulse_width(double dutyCycle);
	void set_saw_window(double width);
	void set_sine_fold(double amount);
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
	bassline::dsp::WavetableOscillator m_sineThird;
	bassline::dsp::WavetableOscillator m_saw;
	bassline::dsp::WavetableOscillator m_sawWindow;
	bassline::dsp::WavetableOscillator m_square;
	bassline::dsp::WavetableOscillator m_pulseEdge;
	bassline::dsp::Noise m_noise;
	double m_frequency;
	double m_pulseWidth;
	double m_sawWindowWidth;
	double m_sineFold;
};

#endif /* OSC_H_ */
