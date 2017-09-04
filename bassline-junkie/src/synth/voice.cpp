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
	flt_mod_matrix.frequency = 64+32;
	flt_mod_matrix.resonance = 0;

	amp_mod_matrix.velocity= 0;

	for (auto &osc_mod : osc_mod_matrix)
	{
		osc_mod.octave = 0.5;
		osc_mod.freq = 64;
	}
	
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
		for(auto &osc_mod : osc_mod_matrix)
		{
			// octave switcher
			// osc_mod.octave range (0:1)
			// multiply by 5 and substract 2 for center pitch scale (zero in (-2:2) range)
			// * 12 for octave multiply
			auto octave = ( (uint_fast8_t)(osc_mod.octave * 5)- 2) * 12;

			stk::StkFloat osc_freq = osc_mod.freq + octave;

			osc_freq += adsr0tick * osc_mod.osc_mod.env0_amt * env_range_in_notes;
			osc_freq += adsr1tick * osc_mod.osc_mod.env1_amt * env_range_in_notes;
			osc_freq += adsr2tick * osc_mod.osc_mod.env2_amt * env_range_in_notes;

			osc_freq += lfo0tick * osc_mod.osc_mod.lfo0_amt * lfo_range_in_notes;
			osc_freq += lfo1tick * osc_mod.osc_mod.lfo1_amt * lfo_range_in_notes;
			osc_freq += lfo2tick * osc_mod.osc_mod.lfo2_amt * lfo_range_in_notes;

			osc_freq += osc_mod.detune * 20;

			osc_freq = (stk::StkFloat) 220.0 * stk::math::pow( 2.0, (osc_freq - 57.0) / 12.0 );

			auto elo = std::distance(osc_mod_matrix.begin(), &osc_mod);

			osc[elo].setFrequency(osc_freq);
		}
		///////////////////////////// OSCILLATORS
		/////////////////////////////////////////////////////////////////////////////


		/////////////////////////////////////////////////////////////////////////////
		///////////////////////////// FILTERS

		stk::StkFloat flt_freq = flt_mod_matrix.frequency;

		flt_freq += adsr0tick * flt_mod_matrix.flt_mod_matrix.env0_amt * env_range_in_notes;
		flt_freq += adsr1tick * flt_mod_matrix.flt_mod_matrix.env1_amt * env_range_in_notes;
		flt_freq += adsr2tick * flt_mod_matrix.flt_mod_matrix.env2_amt * env_range_in_notes;

		flt_freq += lfo0tick * flt_mod_matrix.flt_mod_matrix.lfo0_amt * lfo_range_in_notes;
		flt_freq += lfo1tick * flt_mod_matrix.flt_mod_matrix.lfo1_amt * lfo_range_in_notes;
		flt_freq += lfo2tick * flt_mod_matrix.flt_mod_matrix.lfo2_amt * lfo_range_in_notes;

		flt_freq = (stk::StkFloat) 220.0 * stk::math::pow( 2.0, (flt_freq - 57.0) / 12.0 );

		if(flt_freq > 20000)flt_freq = 20000.; //moogfliter fixed upper limit to avoid aliasing


		filter.setCutoff(flt_freq);
		filter.setRes(flt_mod_matrix.resonance);


		///////////////////////////// FILTERS
		/////////////////////////////////////////////////////////////////////////////



		sralinka output;
		output = 0;

		for(auto &wave : osc)
			output += wave.tick() * 0.3;

		output = filter.process( output );


		//writer.process(adsr_tick);

		output *= adsr2tick;

		output *= amp_mod_matrix.velocity; // velocity

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
	for(auto &osc_mod : osc_mod_matrix)
	{
		osc_mod.freq = note;
	}

	amp_mod_matrix.velocity = vel * divider;
	for(auto &ads : env)
		ads.gate(1);
}

void Voice::noteOff()
{
	for(auto &ads : env)
		ads.gate(0);
}

#define ADSR_OFFSET 0
#define ADSR_NUMBER 3
#define ADSR_PARAMS 4

#define FLT_MOD_OFFSET  32
#define FLT_MOD_NUMBER  1
#define FLT_MOD_PARAMS 8

#define LFO_OFFSET 48
#define LFO_NUMBER 3
#define LFO_PARAMS 2


#define OSC_MOD_OFFSET  96
#define OSC_MOD_NUMBER  3
#define OSC_MOD_PARAMS 8

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


	if(param >= ADSR_OFFSET && param <= ADSR_OFFSET+(ADSR_NUMBER*ADSR_PARAMS) )
	{
		uint_fast8_t tmp_param = param - ADSR_OFFSET;
		uint_fast8_t adsr_number = tmp_param / ADSR_PARAMS;
		tmp_param = tmp_param % ADSR_PARAMS;
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


	if(param >= FLT_MOD_OFFSET && param <= FLT_MOD_OFFSET+(FLT_MOD_NUMBER * FLT_MOD_PARAMS) )
	{
		uint_fast8_t tmp_param = param - FLT_MOD_OFFSET;
		uint_fast8_t osc_number = tmp_param / FLT_MOD_PARAMS;
		tmp_param = tmp_param % FLT_MOD_PARAMS;
		switch (tmp_param)
		{
			case 0:
			{
				flt_mod_matrix.frequency = val;
			}
			break;
			case 1:
			{
				flt_mod_matrix.flt_mod_matrix.env0_amt = val*divider;
			}
			break;
			case 2:
			{
				flt_mod_matrix.flt_mod_matrix.env1_amt = val*divider;
			}
			break;
			case 3:
			{
				flt_mod_matrix.flt_mod_matrix.env2_amt = val*divider;
			}
			break;
			case 4:
			{
				flt_mod_matrix.flt_mod_matrix.lfo0_amt = val*divider;
			}
			break;
			case 5:
			{
				flt_mod_matrix.flt_mod_matrix.lfo1_amt = val*divider;
			}
			break;
			case 6:
			{
				flt_mod_matrix.flt_mod_matrix.lfo2_amt = val*divider;
			}
			break;
			case 7:
			{
				flt_mod_matrix.resonance = val * divider;
			}

			break;
		}
	}



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




	if(param >= OSC_MOD_OFFSET && param <= OSC_MOD_OFFSET+(OSC_MOD_NUMBER * OSC_MOD_PARAMS) )
	{
		uint_fast8_t tmp_param = param - OSC_MOD_OFFSET;
		uint_fast8_t osc_number = tmp_param / OSC_MOD_PARAMS;
		tmp_param = tmp_param % OSC_MOD_PARAMS;
		switch (tmp_param)
		{
			case 0:
			{
				osc_mod_matrix[osc_number].octave = val*divider;
			}
			break;
			case 1:
			{
				osc_mod_matrix[osc_number].osc_mod.env0_amt = val*divider;
			}
			break;
			case 2:
			{
				osc_mod_matrix[osc_number].osc_mod.env1_amt = val*divider;
			}
			break;
			case 3:
			{
				osc_mod_matrix[osc_number].osc_mod.env2_amt = val*divider;
			}
			break;
			case 4:
			{
				osc_mod_matrix[osc_number].osc_mod.lfo0_amt = val*divider;
			}
			break;
			case 5:
			{
				osc_mod_matrix[osc_number].osc_mod.lfo1_amt = val*divider;
			}
			break;
			case 6:
			{
				osc_mod_matrix[osc_number].osc_mod.lfo2_amt = val*divider;
			}
			break;
			case 7:
			{
				osc_mod_matrix[osc_number].detune = val*divider;
			}

			break;
		}
	}





}
