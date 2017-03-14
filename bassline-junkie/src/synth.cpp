/*
 * synth.cpp
 *
 *  Created on: Mar 14, 2017
 *      Author: alax
 */

#include "synth.h"

synth::synth()
{
	// TODO Auto-generated constructor stub

}

synth::~synth()
{
	// TODO Auto-generated destructor stub
}

void synth::process(std::array<int, 512> &input)
{
	for (unsigned int i = 0; i < input.size(); i++)
		input[i] = sine.tick() * maxval;
}

void synth::init()
{
	stk::Stk::setSampleRate(44100);
	sine.setFrequency(440);

}
