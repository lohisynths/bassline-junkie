/*
 * synth.h
 *
 *  Created on: Mar 14, 2017
 *      Author: alax
 */

#ifndef SYNTH_H_
#define SYNTH_H_

#include <array>
#include <BlitSaw.h>

#include "ADSR.h"

#include <cmath>
#include "MoogFilter.h"
#include "MidiReceiver.h"

const unsigned int format_bits = 32; //snd_pcm_format_width(*m_format);
const unsigned int maxval = (1U << (format_bits - 1U)) - 1U;

class synth
{
public:
	synth();

	virtual ~synth();

	std::array<int, 512>& get_array()
	{
		return array;
	}

	void process();

	void init(double freq);


	void message(MidiMessage *msg);

	void noteOn(double freq, double vel);
	void noteOff();




private:
	stk::BlitSaw osc;
	MoogFilter filter;
	stk::ADSR adsr;

	double velocity;

	double osc_freq;
	double flt_freq;
	double flt_res;
	std::array<int, 512> array;
};

#endif /* SYNTH_H_ */
