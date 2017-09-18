/*
 * modifiers.h
 *
 *  Created on: Sep 16, 2017
 *      Author: alax
 */

#ifndef SRC_SYNTH_MODIFIERS_H_
#define SRC_SYNTH_MODIFIERS_H_

#include <array>
#include "dsp/Lfo.h"
#include "dsp/ADSR/ADSR.h"
#include "dsp/MoogFilter.h"
#include "dsp/Osc.h"

#include "utils/MidiReceiver.h"

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


class modifiers{
public:
	const stk::StkFloat env_range_in_notes = 12 * 4;
	const stk::StkFloat lfo_range_in_notes = 12 * 2;

	struct mod_matrix
	{
		stk::StkFloat env0_amt;
		stk::StkFloat env1_amt;
		stk::StkFloat env2_amt;

		stk::StkFloat lfo0_amt;
		stk::StkFloat lfo1_amt;
		stk::StkFloat lfo2_amt;
	};

	struct osc_mod_struct
	{
		stk::StkFloat freq=0;
		stk::StkFloat detune=0;
		stk::StkFloat octave=0;

		stk::StkFloat sin_level=1;
		stk::StkFloat saw_level=1;
		stk::StkFloat sqr_level=0;
		stk::StkFloat rnd_level=0;

		mod_matrix osc_mod={0};
	};

	struct flt_mod_struct
	{
		stk::StkFloat frequency=0;
		stk::StkFloat resonance=0;
		mod_matrix flt_mod_matrix={0};
	};

	struct amp_mod_struct
	{
		stk::StkFloat velocity=0;
		mod_matrix amp_mod_matrix={0};
	};


	std::array<Lfo, 3> lfo;
	std::array<ADSR, 3> env;
	std::array<osc_mod_struct, 3> osc_m;
	flt_mod_struct flt_mod_matrix;
	amp_mod_struct amp_mod_matrix;



void updateOsc(Osc &osc, size_t osc_nr)
{
	// octave switcher
	// osc_mod.octave range (0:1)
	// multiply by 5 and substract 2 for center pitch scale (zero in (-2:2) range)
	// * 12 for octave multiply
	auto octave = ( (uint_fast8_t)(osc_m[osc_nr].octave * 5)- 2) * 12;

	stk::StkFloat osc_freq = osc_m[osc_nr].freq + octave;

	osc_freq += env[0].getOutput() * osc_m[osc_nr].osc_mod.env0_amt * env_range_in_notes;
	osc_freq += env[1].getOutput() * osc_m[osc_nr].osc_mod.env1_amt * env_range_in_notes;
	osc_freq += env[2].getOutput() * osc_m[osc_nr].osc_mod.env2_amt * env_range_in_notes;

	osc_freq += lfo[0].get_value() * osc_m[osc_nr].osc_mod.lfo0_amt * lfo_range_in_notes;
	osc_freq += lfo[1].get_value() * osc_m[osc_nr].osc_mod.lfo1_amt * lfo_range_in_notes;
	osc_freq += lfo[2].get_value() * osc_m[osc_nr].osc_mod.lfo2_amt * lfo_range_in_notes;

	osc_freq += osc_m[osc_nr].detune * 20;

	osc_freq = (stk::StkFloat) 220.0 * stk::math::pow( 2.0, (osc_freq - 57.0) / 12.0 );


	osc.set_noise_level(osc_m[osc_nr].rnd_level);
	osc.set_sin_level(osc_m[osc_nr].sin_level);
	osc.set_saw_level(osc_m[osc_nr].saw_level);
	osc.set_sqr_level(osc_m[osc_nr].sqr_level);

	osc.setFrequency(osc_freq);
}

void updateFilter(MoogFilter *filter)
{


	stk::StkFloat flt_freq = flt_mod_matrix.frequency;

	flt_freq += env[0].getOutput() * flt_mod_matrix.flt_mod_matrix.env0_amt * env_range_in_notes;
	flt_freq += env[1].getOutput() * flt_mod_matrix.flt_mod_matrix.env1_amt * env_range_in_notes;
	flt_freq += env[2].getOutput() * flt_mod_matrix.flt_mod_matrix.env2_amt * env_range_in_notes;

	flt_freq += lfo[0].get_value() * flt_mod_matrix.flt_mod_matrix.lfo0_amt * lfo_range_in_notes;
	flt_freq += lfo[1].get_value() * flt_mod_matrix.flt_mod_matrix.lfo1_amt * lfo_range_in_notes;
	flt_freq += lfo[2].get_value() * flt_mod_matrix.flt_mod_matrix.lfo2_amt * lfo_range_in_notes;

	flt_freq = (stk::StkFloat) 220.0 * stk::math::pow( 2.0, (flt_freq - 57.0) / 12.0 );

	if(flt_freq > 20000)flt_freq = 20000.; //moogfliter fixed upper limit to avoid aliasing


	filter->setCutoff(flt_freq);
	filter->setRes(flt_mod_matrix.resonance);
}

void controlCange(uint8_t param, uint8_t value)
{
	stk::StkFloat val=value;
	if(param < 13)
		if(param > 0)
			if(val==0)
				val=0.001*127.; // avoid pops and clicks

	const stk::StkFloat divider = 1. / 127.;

	if(param >= ADSR_OFFSET && param <= ADSR_OFFSET+(ADSR_NUMBER*ADSR_PARAMS) )
	{
		uint_fast8_t tmp_param = param - ADSR_OFFSET;
		uint_fast8_t adsr_number = tmp_param / ADSR_PARAMS;
		tmp_param = tmp_param % ADSR_PARAMS;

		switch (tmp_param)
		{

			case 0:
			{
				this->env[adsr_number].setAttackRate(val*divider);
			}
			break;
			case 1:
			{
				this->env[adsr_number].setDecayRate(val*divider);
			}
			break;
			case 2:
			{
				this->env[adsr_number].setSustainLevel(val*divider);
			}
			break;
			case 3:
			{
				this->env[adsr_number].setReleaseRate(val*divider);
			}
			break;
		}
	}


	if(param >= FLT_MOD_OFFSET && param <= FLT_MOD_OFFSET+(FLT_MOD_NUMBER * FLT_MOD_PARAMS) )
	{
		uint_fast8_t tmp_param = param - FLT_MOD_OFFSET;
		tmp_param = tmp_param % FLT_MOD_PARAMS;
		switch (tmp_param)
		{
			case 0:
			{
				this->flt_mod_matrix.frequency = val;
			}
			break;
			case 1:
			{
				this->flt_mod_matrix.flt_mod_matrix.env0_amt = val*divider;
			}
			break;
			case 2:
			{
				this->flt_mod_matrix.flt_mod_matrix.env1_amt = val*divider;
			}
			break;
			case 3:
			{
				this->flt_mod_matrix.flt_mod_matrix.env2_amt = val*divider;
			}
			break;
			case 4:
			{
				this->flt_mod_matrix.flt_mod_matrix.lfo0_amt = val*divider;
			}
			break;
			case 5:
			{
				this->flt_mod_matrix.flt_mod_matrix.lfo1_amt = val*divider;
			}
			break;
			case 6:
			{
				this->flt_mod_matrix.flt_mod_matrix.lfo2_amt = val*divider;
			}
			break;
			case 7:
			{
				this->flt_mod_matrix.resonance = val * divider;
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
				this->osc_m[osc_number].sin_level=(val*divider);
			}
			break;
			case 1:
			{
				this->osc_m[osc_number].saw_level=(val*divider);
			}
			break;
			case 2:
			{
				this->osc_m[osc_number].sqr_level=(val*divider);
			}
			break;
			case 3:
			{
				this->osc_m[osc_number].rnd_level=(val*divider);
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
				this->lfo[lfo_number].setShape(val*divider*4);
			}
			break;
			case 1:
			{
				this->lfo[lfo_number].setFrequency( (val*divider * 10.) + 0.0001);
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
				this->osc_m[osc_number].octave = val*divider;
			}
			break;
			case 1:
			{
				this->osc_m[osc_number].osc_mod.env0_amt = val*divider;
			}
			break;
			case 2:
			{
				this->osc_m[osc_number].osc_mod.env1_amt = val*divider;
			}
			break;
			case 3:
			{
				this->osc_m[osc_number].osc_mod.env2_amt = val*divider;
			}
			break;
			case 4:
			{
				this->osc_m[osc_number].osc_mod.lfo0_amt = val*divider;
			}
			break;
			case 5:
			{
				this->osc_m[osc_number].osc_mod.lfo1_amt = val*divider;
			}
			break;
			case 6:
			{
				this->osc_m[osc_number].osc_mod.lfo2_amt = val*divider;
			}
			break;
			case 7:
			{
				this->osc_m[osc_number].detune = val*divider;
			}

			break;
		}
	}
}


void message(MidiMessage *msg)
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
		case MidiMessage::Type::NO_MESSAGE:
			break;
		};
	}
}

const stk::StkFloat divider = 1. / 127.;

void noteOn(stk::StkFloat note, stk::StkFloat vel)
{
	for(auto &osc_ : osc_m )
	{
		osc_.freq = note;
	}

	amp_mod_matrix.velocity = vel * divider;
	for(auto &ads : env)
		ads.gate(1);
}

void noteOff()
{
	for(auto &ads : env)
		ads.gate(0);
}



};

#endif /* SRC_SYNTH_MODIFIERS_H_ */