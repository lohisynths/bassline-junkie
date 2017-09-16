/*
 * Lfo.cpp
 *
 *  Created on: Oct 5, 2016
 *      Author: alax
 */

#include "Lfo.h"

Lfo::Lfo()
{
	osc.setSampleRate(stk::Stk::sampleRate());
	osc.setMode(Oscillator::OSCILLATOR_MODE_SINE);
	osc.setFrequency(1.);
}

Lfo::~Lfo(){}

void Lfo::setShape(int shape)
{
	if(shape == 0)
		osc.setMode(Oscillator::OSCILLATOR_MODE_SINE);
	else if(shape == 1)
		osc.setMode(Oscillator::OSCILLATOR_MODE_SAW);
	else if(shape == 2)
		osc.setMode(Oscillator::OSCILLATOR_MODE_SQUARE);
	else
		std::cout << "Lfo::setShape - wrong shape selected. input = " << shape << std::endl;
}


void Lfo::setFrequency(stk::StkFloat freq)
{
	osc.setFrequency(static_cast<float>(freq));
}


stk::StkFloat Lfo::tick()
{
	auto output = osc.nextSample();
	return static_cast<stk::StkFloat>(output);
}



void Lfo::reset()
{
	osc.reset();
}
