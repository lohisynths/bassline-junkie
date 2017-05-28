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
#include <FileLoop.h>

#include "ADSR.h"

#include <cmath>
#include "dsp/MoogFilter.h"
#include "utils/MidiReceiver.h"



class Voice
{
public:
	Voice();

	virtual ~Voice();

	std::array<double, 512>& get_array()
	{
		return array;
	}

	void process();

	void init(double freq);


	void message(MidiMessage *msg);

	void noteOn(double freq, double vel);
	void noteOff();
	void controlCange(uint8_t param, uint8_t val);




private:
	//stk::FileLoop *waves_;

	stk::BlitSaw osc;
	MoogFilter filter;
	stk::ADSR filter_adsr;

	double filter_adsr_range;

	stk::ADSR amp_adsr;

	double velocity;

	double osc_freq;
	double flt_freq;

	double flt_res;
	std::array<double	, 512> array;
};

#endif /* SYNTH_H_ */
