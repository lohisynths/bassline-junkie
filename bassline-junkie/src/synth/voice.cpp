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

	osc1_mod_matrix.main = 64;

	flt_mod_matrix.main = 64;

	flt_res = 0.0;
	
	std::fill(std::begin(array), std::end(array), 0);
}

Voice::~Voice()
{

}

const stk::StkFloat osc_env_range = 12 * 2;

void Voice::process()
{
	for (auto &sample : array)
	{

		stk::StkFloat adsr0tick = adsr[0].tick();
		stk::StkFloat adsr1tick = adsr[1].tick();
		stk::StkFloat adsr2tick = adsr[2].tick();


		stk::StkFloat osc1_freq = osc1_mod_matrix.main;

		osc1_mod_matrix.adsr2_amt = 1;

		osc1_freq += adsr0tick * osc1_mod_matrix.adsr0_amt * osc_env_range;
		osc1_freq += adsr1tick * osc1_mod_matrix.adsr1_amt * osc_env_range;
		osc1_freq += adsr2tick * osc1_mod_matrix.adsr2_amt * osc_env_range;


		osc1_freq = (stk::StkFloat) 220.0 * stk::math::pow( 2.0, (osc1_freq - 57.0) / 12.0 );

		stk::StkFloat flt_freq = flt_mod_matrix.main;
		flt_freq = (stk::StkFloat) 220.0 * stk::math::pow( 2.0, (flt_freq - 57.0) / 12.0 );



		osc.setFrequency(osc1_freq);

		filter.setCutoff(flt_freq);
		filter.setRes(flt_res);


		stk::StkFloat output = osc.tick();


		output = filter.process(output);

		output *= adsr[2].tick() * 2.; // stk::adsr gives 0-0.5 lol

		output *= amp_mod_matrix.main; // velocity

		sample = output;
	}
}

void Voice::message(MidiMessage *msg)
{
	msg->print();

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



	case 9:
	{
		adsr[2].setAttackTime(val*divider);
	}
	break;
	case 10:
	{
		adsr[2].setDecayTime(val*divider);
	}
	break;
	case 11:
	{
		adsr[2].setSustainLevel(val*divider);
	}
	break;
	case 12:
	{
		adsr[2].setReleaseTime(val*divider);
	}
	break;






	case 96:
	{
		flt_mod_matrix.main = val;
	}
		break;
	case 97:
	{
		flt_res = val * divider;
	}
		break;
	}
}
