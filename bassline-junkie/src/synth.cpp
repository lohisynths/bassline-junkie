/*
 * synth.cpp
 *
 *  Created on: Mar 14, 2017
 *      Author: alax
 */

#include "synth.h"

synth::synth()
{
	osc_freq = 100;
	flt_freq = 500;
	flt_res = 0.75;

	filter.setCutoff(flt_freq);
	filter.setRes(flt_res);
}

synth::~synth()
{

}

void synth::init(double freq)
{
	osc_freq = freq;
	osc.setFrequency(osc_freq);
}
