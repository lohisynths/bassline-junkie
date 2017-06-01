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
		ads.setAllTimes(0.1,0.1,1,0.1);
	}

	osc_mod_matrix.main = freq;
}
Voice::Voice()
{
	osc2_detune = 0;

	amp_mod_matrix.main = 0;

	osc_mod_matrix.main = 64;

	flt_mod_matrix.main = 96;

	flt_res = 0.0;
	
	std::fill(std::begin(array), std::end(array), 0);
}

Voice::~Voice()
{

}

const stk::StkFloat env_range_in_notes = 12 * 2;

#include "utils/array_writer.h"
ArrayWriter writer;

void Voice::process()
{
	for (auto &sample : array)
	{

		stk::StkFloat adsr0tick = adsr[0].tick();
		stk::StkFloat adsr1tick = adsr[1].tick();
		stk::StkFloat adsr2tick = adsr[2].tick();

		/////////////////////////////////////////////////////////////////////////////
		///////////////////////////// OSCILLATORS
		stk::StkFloat osc_freq = osc_mod_matrix.main;

		osc_freq += adsr0tick * osc_mod_matrix.adsr0_amt * env_range_in_notes;
		osc_freq += adsr1tick * osc_mod_matrix.adsr1_amt * env_range_in_notes;
		osc_freq += adsr2tick * osc_mod_matrix.adsr2_amt * env_range_in_notes;


		osc_freq = (stk::StkFloat) 220.0 * stk::math::pow( 2.0, (osc_freq - 57.0) / 12.0 );
		///////////////////////////// OSCILLATORS
		/////////////////////////////////////////////////////////////////////////////


		/////////////////////////////////////////////////////////////////////////////
		///////////////////////////// FILTERS
		stk::StkFloat flt_freq = flt_mod_matrix.main;

		flt_freq += adsr0tick * flt_mod_matrix.adsr0_amt * env_range_in_notes;
		flt_freq += adsr1tick * flt_mod_matrix.adsr1_amt * env_range_in_notes;
		flt_freq += adsr2tick * flt_mod_matrix.adsr2_amt * env_range_in_notes;

		flt_freq = (stk::StkFloat) 220.0 * stk::math::pow( 2.0, (flt_freq - 57.0) / 12.0 );

		if(flt_freq > 6000)flt_freq = 6000.; //moogfliter fixed upper limit to avoid aliasing
		///////////////////////////// FILTERS
		/////////////////////////////////////////////////////////////////////////////


		osc.setFrequency(osc_freq);
		osc2.setFrequency(osc_freq+osc2_detune);

		filter.setCutoff(flt_freq);
		filter.setRes(flt_res);


		sralinka output;
		output = osc.tick() * 0.5;
		output += osc2.tick() * 0.5;

		output = filter.process( output );

		auto adsr_tick = adsr[2].tick();


		//writer.process(adsr_tick);

		output *= adsr_tick;

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
	osc_mod_matrix.main = note;
	amp_mod_matrix.main = vel * divider;
	for(auto &ads : adsr)
		ads.keyOn();
}

void Voice::noteOff()
{
	for(auto &ads : adsr)
		ads.keyOff();
}

void Voice::controlCange(uint8_t param, uint8_t value)
{
	stk::StkFloat val=value;
	if(param < 13)
		if(param > 0)
			if(val==0)
				val=0.01*127.; // fix for ADSR not handling 0

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
		osc_mod_matrix.adsr0_amt = val * divider;
	}
		break;


	case 97+16:
	{
		flt_mod_matrix.adsr1_amt = val * divider;
	}
		break;

	}
}
