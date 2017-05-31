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
	for(auto &ads : adsr)
	{
		ads.setAllTimes(0.01,0.1,1,0.1);
	}

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

const stk::StkFloat env_range_in_notes = 12 * 2;


void Voice::process()
{
	for (auto &sample : array)
	{

		stk::StkFloat adsr0tick = adsr[0].tick();
		stk::StkFloat adsr1tick = adsr[1].tick();
		stk::StkFloat adsr2tick = adsr[2].tick();

		/////////////////////////////////////////////////////////////////////////////
		///////////////////////////// OSCILLATORS
		stk::StkFloat osc1_freq = osc1_mod_matrix.main;

		osc1_freq += adsr0tick * osc1_mod_matrix.adsr0_amt * env_range_in_notes;
		osc1_freq += adsr1tick * osc1_mod_matrix.adsr1_amt * env_range_in_notes;
		osc1_freq += adsr2tick * osc1_mod_matrix.adsr2_amt * env_range_in_notes;


		osc1_freq = (stk::StkFloat) 220.0 * stk::math::pow( 2.0, (osc1_freq - 57.0) / 12.0 );
		///////////////////////////// OSCILLATORS
		/////////////////////////////////////////////////////////////////////////////


		/////////////////////////////////////////////////////////////////////////////
		///////////////////////////// FILTERS
		stk::StkFloat flt_freq = flt_mod_matrix.main;

		flt_freq += adsr0tick * flt_mod_matrix.adsr0_amt * env_range_in_notes;
		flt_freq += adsr1tick * flt_mod_matrix.adsr1_amt * env_range_in_notes;
		flt_freq += adsr2tick * flt_mod_matrix.adsr2_amt * env_range_in_notes;

		flt_freq = (stk::StkFloat) 220.0 * stk::math::pow( 2.0, (flt_freq - 57.0) / 12.0 );

		if(flt_freq > 20000)flt_freq = 20000.; //moogfliter bug when input out out fangeTODO:
		///////////////////////////// FILTERS
		/////////////////////////////////////////////////////////////////////////////


		osc.setFrequency(osc1_freq);
		osc2.setFrequency(osc1_freq+1);

		filter.setCutoff(flt_freq);
		filter.setRes(flt_res);


		sralinka output;
		output = osc.tick();
		output += osc2.tick();

		output = filter.process( output() );

		output *= adsr[2].tick() * 2.; // stk::adsr gives 0-0.5 lol

		output *= amp_mod_matrix.main; // velocity


		sample = output();
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
	for(auto &ads : adsr)
		ads.keyOn();
}

void Voice::noteOff()
{
	for(auto &ads : adsr)
		ads.keyOff();
}

void Voice::controlCange(uint8_t param, uint8_t val)
{
	if(param < 13)
		if(param > 0)
			if(val==0)
				val=1; // fix for ADSR not handling 0

	switch (param)
	{
	/// glupota
	case 1:
	{
		adsr[0].setAttackTime(val*divider);
	}
	break;
	case 2:
	{
		adsr[0].setDecayTime(val*divider);
	}
	break;
	case 3:
	{
		adsr[0].setSustainLevel(val*divider);
	}
	break;
	case 4:
	{
		adsr[0].setReleaseTime(val*divider);
	}
	break;


	case 5:
	{
		adsr[1].setAttackTime(val*divider);
	}
	break;
	case 6:
	{
		adsr[1].setDecayTime(val*divider);
	}
	break;
	case 7:
	{
		adsr[1].setSustainLevel(val*divider);
	}
	break;
	case 8:
	{
		adsr[1].setReleaseTime(val*divider);
	}
	break;


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
	/// *glupota





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

	case 96+16:
	{
		osc1_mod_matrix.adsr0_amt = val * divider;
	}
		break;


	case 97+16:
	{
		flt_mod_matrix.adsr1_amt = val * divider;
	}
		break;

	}
}
