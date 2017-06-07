/*
 * voice.cpp
 *
 *  Created on: Mar 14, 2017
 *      Author: alax
 */

#include "voice.h"

const stk::StkFloat divider = 1. / 127.;



void Voice::init(stk::StkFloat freq)
{
	for(auto &ads : env)
	{
		ads.setAllTimes(0.1,0.1,1,0.1);
	}

	osc_tune = freq;
}
Voice::Voice()
{
	osc_tune = 64;
	flt_tune = 64;


	osc2_detune = 10;

	amp_mod_matrix.main = 0;




	flt_mod_matrix.main = 0.5; // octave switcher
	osc_mod_matrix.main = 0.5; // octave switcher


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

		stk::StkFloat adsr0tick = env[0].tick();
		stk::StkFloat adsr1tick = env[1].tick();
		stk::StkFloat adsr2tick = env[2].tick();

		/////////////////////////////////////////////////////////////////////////////
		///////////////////////////// OSCILLATORS
		static const stk::StkFloat elko = 128. / 26.; // 4,923076923 * 12 = 5 octaves /    -2 center pitch
		auto ciabejek = (uint_fast8_t(osc_mod_matrix.main * elko)- 2) * 12;

		stk::StkFloat osc_freq = osc_tune + ciabejek;

		osc_freq += adsr0tick * osc_mod_matrix.env0_amt * env_range_in_notes;
		osc_freq += adsr1tick * osc_mod_matrix.env1_amt * env_range_in_notes;
		osc_freq += adsr2tick * osc_mod_matrix.env2_amt * env_range_in_notes;


		osc_freq = (stk::StkFloat) 220.0 * stk::math::pow( 2.0, (osc_freq - 57.0) / 12.0 );
		///////////////////////////// OSCILLATORS
		/////////////////////////////////////////////////////////////////////////////


		/////////////////////////////////////////////////////////////////////////////
		///////////////////////////// FILTERS
		ciabejek = (uint_fast8_t(flt_mod_matrix.main * elko)- 2) * 12;

		stk::StkFloat flt_freq = flt_tune + ciabejek;

		flt_freq += adsr0tick * flt_mod_matrix.env0_amt * env_range_in_notes;
		flt_freq += adsr1tick * flt_mod_matrix.env1_amt * env_range_in_notes;
		flt_freq += adsr2tick * flt_mod_matrix.env2_amt * env_range_in_notes;

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

		auto adsr_tick = env[2].tick();


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

void Voice::noteOn(stk::StkFloat note, stk::StkFloat vel)
{
	osc_tune = note;
	amp_mod_matrix.main = vel * divider;
	for(auto &ads : env)
		ads.keyOn();
}

void Voice::noteOff()
{
	for(auto &ads : env)
		ads.keyOff();
}

#define ENV_OFFSET 0
#define OSC_MOD_OFFSET  16
#define FLT_MOD_OFFSET  32


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
	case 0 + ENV_OFFSET:
	{
		env[0].setAttackTime(val*divider);
	}
	break;
	case 1 + ENV_OFFSET:
	{
		env[0].setDecayTime(val*divider);
	}
	break;
	case 2 + ENV_OFFSET:
	{
		env[0].setSustainLevel(val*divider);
	}
	break;
	case 3 + ENV_OFFSET:
	{
		env[0].setReleaseTime(val*divider);
	}
	break;


	case 4 + ENV_OFFSET:
	{
		env[1].setAttackTime(val*divider);
	}
	break;
	case 5 + ENV_OFFSET:
	{
		env[1].setDecayTime(val*divider);
	}
	break;
	case 6 + ENV_OFFSET:
	{
		env[1].setSustainLevel(val*divider);
	}
	break;
	case 7 + ENV_OFFSET:
	{
		env[1].setReleaseTime(val*divider);
	}
	break;


	case 8 + ENV_OFFSET:
	{
		env[2].setAttackTime(val*divider);
	}
	break;
	case 9 + ENV_OFFSET:
	{
		env[2].setDecayTime(val*divider);
	}
	break;
	case 10 + ENV_OFFSET:
	{
		env[2].setSustainLevel(val*divider);
	}
	break;
	case 11 + ENV_OFFSET:
	{
		env[2].setReleaseTime(val*divider);
	}
	break;
	/// *glupota




	
	case 0 + OSC_MOD_OFFSET:
	{
		osc_mod_matrix.main = val*divider;
	}
	break;
	case 1 + OSC_MOD_OFFSET:
	{
		osc_mod_matrix.env0_amt = val*divider;
	}
	break;
	case 2 + OSC_MOD_OFFSET:
	{
		osc_mod_matrix.env1_amt = val*divider;
	}
	break;
	case 3 + OSC_MOD_OFFSET:
	{
		osc_mod_matrix.env2_amt = val*divider;
	}
	break;
	case 4 + OSC_MOD_OFFSET:
	{
		osc_mod_matrix.lfo0_amt = val*divider;
	}
	break;
	case 5 + OSC_MOD_OFFSET:
	{
		osc_mod_matrix.lfo1_amt = val*divider;
	}
	break;
	case 6 + OSC_MOD_OFFSET:
	{
		osc_mod_matrix.lfo2_amt = val*divider;
	}
	break;
	case 7 + OSC_MOD_OFFSET:
	{
		;
	}
	break;
	
	
	
	
	case 0 + FLT_MOD_OFFSET:
	{
		flt_mod_matrix.main = val*divider;
	}
	break;
	case 1 + FLT_MOD_OFFSET:
	{
		flt_mod_matrix.env0_amt = val*divider;
	}
	break;
	case 2 + FLT_MOD_OFFSET:
	{
		flt_mod_matrix.env1_amt = val*divider;
	}
	break;
	case 3 + FLT_MOD_OFFSET:
	{
		flt_mod_matrix.env2_amt = val*divider;
	}
	break;
	case 4 + FLT_MOD_OFFSET:
	{
		flt_mod_matrix.lfo0_amt = val*divider;
	}
	break;
	case 5 + FLT_MOD_OFFSET:
	{
		flt_mod_matrix.lfo0_amt = val*divider;
	}
	break;
	case 6 + FLT_MOD_OFFSET:
	{
		flt_mod_matrix.lfo1_amt = val*divider;
	}
	break;
	case 7 + FLT_MOD_OFFSET:
	{
		;
	}
	break;
	
	
	
	case 96:
	{
		flt_tune = val;
	}
		break;


	case 97:
	{
		flt_res = val * divider;
	}
		break;

	case 96+16:
	{
		osc_mod_matrix.env0_amt = val * divider;
	}
		break;


	case 97+16:
	{
		flt_mod_matrix.env1_amt = val * divider;
	}
		break;

	}
}
