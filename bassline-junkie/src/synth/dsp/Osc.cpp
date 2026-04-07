/*
 * Osc.cpp
 *
 *  Created on: Oct 3, 2016
 *      Author: alax
 */

#include "Osc.h"
#include "../config.h"

Osc::Osc() : m_osc_ctrl( waves_level{0,1,0,0} )
{
	m_sine.setSampleRate(sample_rate);
	m_saw.setSampleRate(sample_rate);
	m_square.setSampleRate(sample_rate);
}

Osc::~Osc(){}

double Osc::tick()
{
    double output;

	output = m_sine.tick() * m_osc_ctrl.sin_level;
	output += m_saw.tick() * m_osc_ctrl.saw_level;
	output += m_square.tick() * m_osc_ctrl.sqr_level;
	output += m_noise.tick() * m_osc_ctrl.rnd_level * 0.5;

	double div =  m_osc_ctrl.sin_level+m_osc_ctrl.saw_level+m_osc_ctrl.sqr_level+m_osc_ctrl.rnd_level;
	if(div<1) div=1;
	output /= div;

	return output ;
}

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
