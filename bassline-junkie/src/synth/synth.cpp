/*
 * synth.cpp
 *
 *  Created on: Mar 14, 2017
 *      Author: alax
 */

#include "synth.h"

//! A static table of equal-tempered MIDI to frequency (Hz) values.
static const double Midi2Pitch[129] = {
  8.176, 8.662, 9.177, 9.723, 10.301, 10.913, 11.562, 12.25,
  12.978, 13.75, 14.568, 15.434, 16.352, 17.324, 18.354, 19.445,
  20.602, 21.827, 23.125, 24.50, 25.957, 27.50, 29.135, 30.868,
  32.703, 34.648, 36.708, 38.891, 41.203, 43.654, 46.249, 49.0,
  51.913, 55.0, 58.271, 61.735, 65.406, 69.296, 73.416, 77.782,
  82.407, 87.307, 92.499, 97.999, 103.826, 110.0, 116.541, 123.471,
  130.813, 138.591, 146.832, 155.563, 164.814, 174.614, 184.997, 195.998,
  207.652, 220.0, 233.082, 246.942, 261.626, 277.183, 293.665, 311.127,
  329.628, 349.228, 369.994, 391.995, 415.305, 440.0, 466.164, 493.883,
  523.251, 554.365, 587.33, 622.254, 659.255, 698.456, 739.989, 783.991,
  830.609, 880.0, 932.328, 987.767, 1046.502, 1108.731, 1174.659, 1244.508,
  1318.51, 1396.913, 1479.978, 1567.982, 1661.219, 1760.0, 1864.655, 1975.533,
  2093.005, 2217.461, 2349.318, 2489.016, 2637.02, 2793.826, 2959.955, 3135.963,
  3322.438, 3520.0, 3729.31, 3951.066, 4186.009, 4434.922, 4698.636, 4978.032,
  5274.041, 5587.652, 5919.911, 6271.927, 6644.875, 7040.0, 7458.62, 7902.133,
  8372.018, 8869.844, 9397.273, 9956.063, 10548.082, 11175.303, 11839.822, 12543.854,
  13289.75};

void synth::init(double freq)
{
	adsr.setSustainLevel(std::numeric_limits<double>::min());
	adsr.setAttackRate(0.);
	adsr.setDecayRate(0.0);
	adsr.setSustainLevel(0.75);
	adsr.setReleaseRate(0.00);

	osc_freq = freq;
	osc.setFrequency(osc_freq);
}
synth::synth()
{
	velocity = 0;
	osc_freq = 100;
	flt_freq = 4000;
	flt_res = 0.850;

	filter.setCutoff(flt_freq);
	filter.setRes(flt_res);
	std::fill(std::begin(array), std::end(array), 0);
}

synth::~synth()
{

}

void synth::process()
{
	for (auto &sample : array)
	{
#ifndef __arm__
		auto max_iter = 60;
#else
		auto max_iter = 1;
#endif
		auto output = osc.tick();
		for (int i = 0; i < max_iter; i++)
		{
			osc.setFrequency(osc_freq);

			filter.setCutoff(flt_freq);
			filter.setRes(flt_res);
			output = filter.process(output);
			//output = std::tanh(output);
		}

		output *= adsr.	tick() * velocity;

		if (output > 1)
			output = 1;
		else if (output < -1)
			output = -1;

		sample = output * maxval;
	}
}
void synth::message(MidiMessage *msg)
{
	//msg->print();;


	if (msg->m_type != MidiMessage::NO_MESSAGE)
	{
		//std::cout << "Message ";

		switch (msg->m_type)
		{
		case MidiMessage::Type::CC:
			std::cout << "MidiMessage::Type::CC"; // prints "1"
			break;       // and exits the switchNOTE_ON
		case MidiMessage::Type::NOTE_ON:
			noteOn(msg->m_val_1,msg->m_val_2);
			break;       // and exits the switchNOTE_ON
		case MidiMessage::Type::NOTE_OFF:
			noteOff();
			break;       // and exits the switchNOTE_ON

		};
	}


}
const double divider = 1. / 127.;

void synth::noteOn(double freq, double vel)
{
	velocity = vel * divider;
	osc_freq = Midi2Pitch[(int)freq];
	adsr.keyOn();
}

void synth::noteOff()
{
	adsr.keyOff();
}

