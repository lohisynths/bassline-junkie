/*
 * Lfo.cpp
 *
 *  Created on: Oct 5, 2016
 *      Author: alax
 */

#include "Lfo.h"
#include <iostream>

Lfo::Lfo()
{
	osc.setSampleRate(48000);
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
        osc.setMode(Oscillator::OSCILLATOR_MODE_TRIANGLE);
    else if(shape == 3)
            osc.setMode(Oscillator::OSCILLATOR_MODE_SQUARE);
	else
		std::cout << "Lfo::setShape - wrong shape selected. input = " << shape << std::endl;
}


void Lfo::setFrequency(double freq)
{
	osc.setFrequency(static_cast<float>(freq));
}

void Lfo::noteOn() {
    if(sync) {
        osc.reset();
    }
}

double Lfo::tick()
{
	auto output = osc.nextSample();
	return output;
}

double Lfo::get_value()
{
	return osc.getSample();
}



void Lfo::reset()
{
	osc.reset();
}
