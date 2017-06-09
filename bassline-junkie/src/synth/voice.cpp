/*
 * voice.cpp
 *
 *  Created on: Mar 14, 2017
 *      Author: alax
 */

#include "voice.h"

const stk::StkFloat divider = 1. / 127.;

Voice::Voice()
{
	osc_tune = 64;
	flt_tune = 64+32;


	osc2_detune = 0.;
	osc3_detune = 0.;

	amp_mod_matrix.main = 0;




	flt_mod_matrix.main = 0.5; // octave switcher
	osc_mod_matrix[0].main = 0.5; // octave switcher


	flt_res = 0.0;
	
	std::fill(std::begin(array), std::end(array), 0);
}

Voice::~Voice()
{

}

const stk::StkFloat env_range_in_notes = 12 * 4;
const stk::StkFloat lfo_range_in_notes = 12 * 2;

#include "utils/array_writer.h"
ArrayWriter writer;

void Voice::process()
{
	for (auto &sample : array)
	{

		stk::StkFloat adsr0tick = env[0].process();
		stk::StkFloat adsr1tick = env[1].process();
		stk::StkFloat adsr2tick = env[2].process();

		stk::StkFloat lfo0tick = lfo[0].tick();
		stk::StkFloat lfo1tick = lfo[1].tick();
		stk::StkFloat lfo2tick = lfo[2].tick();

		/////////////////////////////////////////////////////////////////////////////
		///////////////////////////// OSCILLATORS
		static const stk::StkFloat elko = 128. / 26.; // 4,923076923 * 12 = 5 octaves /    -2 center pitch
		auto ciabejek = (uint_fast8_t(osc_mod_matrix[0].main * elko)- 2) * 12;

		stk::StkFloat osc_freq = osc_tune + ciabejek;

		osc_freq += adsr0tick * osc_mod_matrix[0].env0_amt * env_range_in_notes;
		osc_freq += adsr1tick * osc_mod_matrix[0].env1_amt * env_range_in_notes;
		osc_freq += adsr2tick * osc_mod_matrix[0].env2_amt * env_range_in_notes;

		osc_freq += lfo0tick * osc_mod_matrix[0].lfo0_amt * lfo_range_in_notes;
		osc_freq += lfo1tick * osc_mod_matrix[0].lfo1_amt * lfo_range_in_notes;
		osc_freq += lfo2tick * osc_mod_matrix[0].lfo2_amt * lfo_range_in_notes;

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

		flt_freq += lfo0tick * flt_mod_matrix.lfo0_amt * lfo_range_in_notes;
		flt_freq += lfo1tick * flt_mod_matrix.lfo1_amt * lfo_range_in_notes;
		flt_freq += lfo2tick * flt_mod_matrix.lfo2_amt * lfo_range_in_notes;

		flt_freq = (stk::StkFloat) 220.0 * stk::math::pow( 2.0, (flt_freq - 57.0) / 12.0 );

		if(flt_freq > 6000)flt_freq = 6000.; //moogfliter fixed upper limit to avoid aliasing
		///////////////////////////// FILTERS
		/////////////////////////////////////////////////////////////////////////////


		for(auto &wave : osc)
			wave.setFrequency(osc_freq);


		filter.setCutoff(flt_freq);
		filter.setRes(flt_res);

		sralinka output;
		output = 0;

		for(auto &wave : osc)
			output += wave.tick() * 0.3;

		output = filter.process( output );


		//writer.process(adsr_tick);

		output *= adsr2tick;

		output *= amp_mod_matrix.main; // velocity

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

void Voice::noteOn(stk::StkFloat note, stk::StkFloat vel)
{
	osc_tune = note;
	amp_mod_matrix.main = vel * divider;
	for(auto &ads : env)
		ads.gate(1);
}

void Voice::noteOff()
{
	for(auto &ads : env)
		ads.gate(0);
}

#define ENV_OFFSET 0
#define ENV_NUMBER 3
#define ENV_PARAMS 4


#define OSC_MOD_OFFSET  16
#define OSC_MOD_NUMBER  1
#define OSC_MOD_PARAMS 8


#define FLT_MOD_OFFSET  32
#define FLT_MOD_NUMBER  1
#define FLT_MOD_PARAMS 8


#define LFO_OFFSET 48
#define LFO_NUMBER 3
#define LFO_PARAMS 2


#define OSC_OFFSET 64
#define OSC_NUMBER 3
#define OSC_PARAMS 4

void Voice::controlCange(uint8_t param, uint8_t value)
{
	stk::StkFloat val=value;
	if(param < 13)
		if(param > 0)
			if(val==0)
				val=0.001*127.; // avoid pops and clicks


	if(param >= OSC_OFFSET && param <= OSC_OFFSET+(OSC_NUMBER*OSC_PARAMS) )
	{
		uint_fast8_t tmp_param = param - OSC_OFFSET;
		uint_fast8_t osc_number = tmp_param / OSC_PARAMS;
		tmp_param = tmp_param % OSC_PARAMS;
		switch (tmp_param)
		{
			case 0:
			{
				osc[osc_number].set_sin_level(val*divider);
			}
			break;
			case 1:
			{
				osc[osc_number].set_saw_level(val*divider);
			}
			break;
			case 2:
			{
				osc[osc_number].set_sqr_level(val*divider);
			}
			break;
			case 3:
			{
				osc[osc_number].set_noise_level(val*divider);
			}
			break;
		}
	}



	if(param >= LFO_OFFSET && param <= LFO_OFFSET+(LFO_NUMBER*LFO_PARAMS) )
	{
		uint_fast8_t tmp_param = param - LFO_OFFSET;
		uint_fast8_t lfo_number = tmp_param / LFO_PARAMS;
		tmp_param = tmp_param % LFO_PARAMS;
		switch (tmp_param)
		{
			case 0:
			{
				lfo[lfo_number].setShape(val*divider*4);
			}
			break;
			case 1:
			{
				lfo[lfo_number].setFrequency( (val*divider * 10.) + 0.0001);
			}
			break;
		}
	}

	if(param >= ENV_OFFSET && param <= ENV_OFFSET+(ENV_NUMBER*ENV_PARAMS) )
	{
		uint_fast8_t tmp_param = param - ENV_OFFSET;
		uint_fast8_t adsr_number = tmp_param / ENV_PARAMS;
		tmp_param = tmp_param % ENV_PARAMS;
		switch (tmp_param)
		{
			case 0:
			{
				env[adsr_number].setAttackRate(val*divider);
			}
			break;
			case 1:
			{
				env[adsr_number].setDecayRate(val*divider);
			}
			break;
			case 2:
			{
				env[adsr_number].setSustainLevel(val*divider);
			}
			break;
			case 3:
			{
				env[adsr_number].setReleaseRate(val*divider);
			}
			break;
		}
	}



	if(param >= OSC_MOD_OFFSET && param <= OSC_MOD_OFFSET+(OSC_MOD_NUMBER	*OSC_MOD_PARAMS) )
	{
		uint_fast8_t tmp_param = param - OSC_MOD_OFFSET;
		uint_fast8_t osc_number = tmp_param / OSC_MOD_PARAMS;
		tmp_param = tmp_param % OSC_MOD_PARAMS;
		switch (tmp_param)
		{
			case 0:
			{
				osc_mod_matrix[osc_number].main = val*divider;
			}
			break;
			case 1:
			{
				osc_mod_matrix[osc_number].env0_amt = val*divider;
			}
			break;
			case 2:
			{
				osc_mod_matrix[osc_number].env1_amt = val*divider;
			}
			break;
			case 3:
			{
				osc_mod_matrix[osc_number].env2_amt = val*divider;
			}
			break;
			case 4:
			{
				osc_mod_matrix[osc_number].lfo0_amt = val*divider;
			}
			break;
			case 5:
			{
				osc_mod_matrix[osc_number].lfo1_amt = val*divider;
			}
			break;
			case 6:
			{
				osc_mod_matrix[osc_number].lfo2_amt = val*divider;
			}
			break;
			case 7:
			{
				;
			}

			break;
		}
	}




	if(param >= FLT_MOD_OFFSET && param <= FLT_MOD_OFFSET+(FLT_MOD_NUMBER * FLT_MOD_PARAMS) )
	{
		uint_fast8_t tmp_param = param - FLT_MOD_OFFSET;
		uint_fast8_t osc_number = tmp_param / FLT_MOD_PARAMS;
		tmp_param = tmp_param % FLT_MOD_PARAMS;
		switch (tmp_param)
		{
			case 0:
			{
				osc_mod_matrix[osc_number].main = val*divider;
			}
			break;
			case 1:
			{
				osc_mod_matrix[osc_number].env0_amt = val*divider;
			}
			break;
			case 2:
			{
				osc_mod_matrix[osc_number].env1_amt = val*divider;
			}
			break;
			case 3:
			{
				osc_mod_matrix[osc_number].env2_amt = val*divider;
			}
			break;
			case 4:
			{
				osc_mod_matrix[osc_number].lfo0_amt = val*divider;
			}
			break;
			case 5:
			{
				osc_mod_matrix[osc_number].lfo1_amt = val*divider;
			}
			break;
			case 6:
			{
				osc_mod_matrix[osc_number].lfo2_amt = val*divider;
			}
			break;
			case 7:
			{
				;
			}

			break;
		}
	}


	switch (param)
	{
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
	}

}
