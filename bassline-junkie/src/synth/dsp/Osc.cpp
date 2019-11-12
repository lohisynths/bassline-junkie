/*
 * Osc.cpp
 *
 *  Created on: Oct 3, 2016
 *      Author: alax
 */

#include "Osc.h"

Osc::Osc() : m_osc_ctrl()
{
  m_sine.setMode(PolyBLEPOscillator::OSCILLATOR_MODE_SINE);
  m_saw.setMode(PolyBLEPOscillator::OSCILLATOR_MODE_SAW);
  m_square.setMode(PolyBLEPOscillator::OSCILLATOR_MODE_SQUARE);

  m_sine.setSampleRate(44800);
  m_saw.setSampleRate(44800);
  m_square.setSampleRate(44800);
}

Osc::~Osc(){}

double Osc::tick()
{
	double output;
	output = m_sine.nextSample() * m_osc_ctrl.sin_level;
	output += m_saw.nextSample() * m_osc_ctrl.saw_level;
	output += m_square.nextSample() * m_osc_ctrl.sqr_level;
	output += m_noise.nextSample() * m_osc_ctrl.rnd_level * 0.5;

	return output;
}

const double env_range_in_notes = 12 * 4;
const double lfo_range_in_notes = 12 * 2;

void Osc::setFrequency(double freq)
{
	m_saw.setFrequency(freq);
	m_square.setFrequency(freq);
	m_sine.setFrequency(freq);
}

void Osc::set_sin_level(double level)
{
	m_osc_ctrl.sin_level=level;
}

void Osc::set_saw_level(double level)
{
	m_osc_ctrl.saw_level=level;
}

void Osc::set_sqr_level(double level)
{
	m_osc_ctrl.sqr_level=level;
}

void Osc::set_noise_level(double level)
{
	m_osc_ctrl.rnd_level=level;
}

void Osc::reset()
{
	m_saw.reset();
	m_square.reset();
	m_sine.reset();

}
