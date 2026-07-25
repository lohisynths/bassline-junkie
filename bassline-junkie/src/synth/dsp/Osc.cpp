/*
 * Osc.cpp
 *
 *  Created on: Oct 3, 2016
 *      Author: alax
 */

#include "Osc.h"
#include "../config.h"

namespace {
double clamp01(double value)
{
	if (value < 0.0) return 0.0;
	if (value > 1.0) return 1.0;
	return value;
}
}

Osc::Osc()
	: m_osc_ctrl(waves_level{0,1,0,0}),
	  m_frequency(220.0),
	  m_pulseWidth(0.5),
	  m_sawWindowWidth(0.0),
	  m_sineFold(0.0)
{
	m_sine.setSampleRate(sample_rate);
	m_sineThird.setSampleRate(sample_rate);
	m_saw.setSampleRate(sample_rate);
	m_sawWindow.setSampleRate(sample_rate);
	m_square.setSampleRate(sample_rate);
	m_pulseEdge.setSampleRate(sample_rate);
	m_sine.setWaveform(bassline::dsp::WavetableOscillator::SINE);
	m_sineThird.setWaveform(bassline::dsp::WavetableOscillator::SINE);
	m_saw.setWaveform(bassline::dsp::WavetableOscillator::SAW);
	m_sawWindow.setWaveform(bassline::dsp::WavetableOscillator::SAW);
	m_square.setWaveform(bassline::dsp::WavetableOscillator::SAW);
	m_pulseEdge.setWaveform(bassline::dsp::WavetableOscillator::SAW);
	setFrequency(m_frequency);
	set_pulse_width(m_pulseWidth);
}

Osc::~Osc(){}

double Osc::tick()
{
    double output;

	const double sine = m_sine.tick();
	const double sineThird = m_sineThird.tick();
	double thirdWeight = m_sineFold;
	const double thirdRatio = (m_frequency * 3.0) / static_cast<double>(sample_rate);
	if (thirdRatio >= 0.5) {
		thirdWeight = 0.0;
	} else if (thirdRatio > 0.4) {
		double fade = (0.5 - thirdRatio) / 0.1;
		fade = fade * fade * (3.0 - 2.0 * fade);
		thirdWeight *= fade;
	}
	const double sineFold = (sine + thirdWeight * sineThird) / (1.0 + thirdWeight);
	output = sineFold * m_osc_ctrl.sin_level;

	const double saw = m_saw.tick();
	const double windowSaw = m_sawWindow.tick();
	const double sawShape =
		saw * (1.0 - m_sawWindowWidth) + windowSaw * m_sawWindowWidth;
	output += sawShape * m_osc_ctrl.saw_level;

	// A pulse is exactly the difference of two equal band-limited saws.
	// Keeping both reads in the same mip level makes a moving duty cycle
	// retain the wavetable oscillator's Nyquist harmonic limit.
	// The raw saw difference has levels -2*width and 2*(1-width).
	// Normalize its larger plateau to unity.  Keeping it DC-free avoids a
	// large offset and also bounds sparse high-frequency mip levels.
	const double pulsePeak =
		2.0 * (m_pulseWidth > 0.5 ? m_pulseWidth : 1.0 - m_pulseWidth);
	const double pulse = (m_square.tick() - m_pulseEdge.tick()) / pulsePeak;
	output += pulse * m_osc_ctrl.sqr_level;
	output += m_noise.tick() * m_osc_ctrl.rnd_level * 0.5;

	double div =  m_osc_ctrl.sin_level+m_osc_ctrl.saw_level+m_osc_ctrl.sqr_level+m_osc_ctrl.rnd_level;
	if(div<1) div=1;
	output /= div;

	return output ;
}

void Osc::setFrequency(double freq)
{
	m_frequency = freq;
	m_saw.setFrequency(freq);
	m_sawWindow.setFrequency(freq);
	m_square.setFrequency(freq);
	m_pulseEdge.setFrequency(freq);
	m_sine.setFrequency(freq);
	m_sineThird.setFrequency(freq * 3.0);
	set_pulse_width(m_pulseWidth);
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

void Osc::set_pulse_width(double dutyCycle)
{
	// Extremely narrow pulses have no useful in-band plateau once their two
	// edges are less than roughly one sample apart.
	double minimum = m_frequency / static_cast<double>(sample_rate);
	if (minimum < 0.01) minimum = 0.01;
	if (minimum > 0.49) minimum = 0.49;
	if (dutyCycle < minimum) dutyCycle = minimum;
	if (dutyCycle > 1.0 - minimum) dutyCycle = 1.0 - minimum;
	m_pulseWidth = dutyCycle;
	m_pulseEdge.setPhaseOffset(m_pulseWidth);
}

void Osc::set_saw_window(double width)
{
	m_sawWindowWidth = clamp01(width);
	// The second discontinuity moves from half a cycle to one eighth of a
	// cycle away.  Crossfading the two band-limited saws gives a useful
	// variable-window saw without warping a wavetable read.
	const double phaseOffset = 0.5 - 0.375 * m_sawWindowWidth;
	m_sawWindow.setPhaseOffset(phaseOffset);
}

void Osc::set_sine_fold(double amount)
{
	m_sineFold = clamp01(amount);
}

void Osc::reset()
{
	m_saw.reset();
	m_sawWindow.reset();
	m_square.reset();
	m_pulseEdge.reset();
	m_sine.reset();
	m_sineThird.reset();

}
