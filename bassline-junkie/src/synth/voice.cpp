/*
 * voice.cpp
 *
 *  Created on: Mar 14, 2017
 *      Author: alax
 */

#include "voice.h"

const double divider = 1. / 127.;



void Voice::init(double freq)
{
	adsr[2].setAttackTime(0.01);
	adsr[2].setDecayTime(0.1);
	adsr[2].setSustainLevel(1);
	adsr[2].setReleaseTime(0.1);

	osc1_mod_matrix.main = freq;
}
Voice::Voice()
{
	amp_mod_matrix.main = 0;

	osc1_mod_matrix.main = 100;

	flt_mod_matrix.main = 10000;

	flt_res = 0.0;
	
	std::fill(std::begin(array), std::end(array), 0);
}

Voice::~Voice()
{

}

void Voice::process()
{
	for (auto &sample : array)
	{
		stk::StkFloat osc1_freq = osc1_mod_matrix.main;

		stk::StkFloat frequency = (stk::StkFloat) 220.0 * stk::math::pow( 2.0, (osc1_freq - 57.0) / 12.0 );

		osc.setFrequency(frequency);

		filter.setCutoff(flt_mod_matrix.main);
		filter.setRes(flt_res);

		auto output = osc.tick();
		output = filter.process(output);

		output *=  amp_mod_matrix.main * adsr[2].tick() * 2.; // stk::adsr gives 0-0.5 lol

		sample = output;
	}
}

void Voice::message(MidiMessage *msg)
{
	//msg->print();

	if (msg->m_type != MidiMessage::NO_MESSAGE)
	{
		//std::cout << "Message ";

		switch (msg->m_type)
		{
		case MidiMessage::Type::CC:
			controlCange(msg->m_val_1,msg->m_val_2);
			break;       // and exits the switchNOTE_ON
		case MidiMessage::Type::NOTE_ON:
			noteOn(msg->m_val_1,msg->m_val_2);
			break;
		case MidiMessage::Type::NOTE_OFF:
			noteOff();
			break;       // and exits the switchNOTE_ON

		};
	}
}

void Voice::noteOn(double note, double vel)
{
	osc1_mod_matrix.main = note;
	amp_mod_matrix.main = vel * divider;
	adsr[2].keyOn();
}

void Voice::noteOff()
{
	adsr[2].keyOff();
}

void Voice::controlCange(uint8_t param, uint8_t val)
{
	switch (param)
	{
	case 96:
	{
		auto out = val * divider;
		out = out * 5000.;
		out = out + 50.;
		flt_mod_matrix.main = out;
	}
		break;
	case 97:
	{
		flt_res = val * divider;
	}
		break;
	}
}
