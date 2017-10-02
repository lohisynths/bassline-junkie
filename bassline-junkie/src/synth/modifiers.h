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


#define OSC_OFFSET 1
#define OSC_NUMBER 3
#define OSC_PARAMS 6

#define ADSR_OFFSET 20
#define ADSR_NUMBER 3
#define ADSR_PARAMS 4

#define FLT_OFFSET  32
#define FLT_NUMBER  1
#define FLT_PARAMS 2

#define LFO_OFFSET 36
#define LFO_NUMBER 3
#define LFO_PARAMS 2


#define OSC_MOD_COUNT   5
#define FLT_MOD_COUNT   2
#define FLT_MOD_OFFSET (OSC_NUMBER*OSC_MOD_COUNT)


#define MATRIX_MOD_OFFSET  96
#define MATIRX_MOD_PARAMS_COUNT  6

#define MATRIX_MOD_MATRIX_ITEMS ( (OSC_NUMBER * OSC_MOD_COUNT) + FLT_MOD_COUNT )


class modifiers{
public:
	const stk::StkFloat env_range_in_notes = 12 * 4;

	struct mod_matrix_s
	{
		stk::StkFloat env0_amt=0;
		stk::StkFloat env1_amt=0;
		stk::StkFloat env2_amt=0;

		stk::StkFloat lfo0_amt=0;
		stk::StkFloat lfo1_amt=0;
		stk::StkFloat lfo2_amt=0;
	};

	struct osc_mod_s
	{
		stk::StkFloat freq=0;
		stk::StkFloat octave=0;

		stk::StkFloat detune=0;
		stk::StkFloat sin_level=0;
		stk::StkFloat saw_level=0;
		stk::StkFloat sqr_level=0;
		stk::StkFloat rnd_level=0;
	};



	mod_matrix_s mod_matrix[OSC_NUMBER*OSC_MOD_COUNT + FLT_MOD_COUNT];



	struct flt_mod_struct
	{
		stk::StkFloat frequency=0;
		stk::StkFloat resonance=0;
	};

	struct amp_mod_struct
	{
		stk::StkFloat velocity=0;
	};


	std::array<Lfo, 3> lfo;
	std::array<ADSR, 3> env;

	std::array<osc_mod_s, 3> osc_m;


	flt_mod_struct flt_mod_matrix;
	amp_mod_struct amp_mod_matrix;



stk::StkFloat  getModVal(int mod_nr)
{
	stk::StkFloat tmp;
	tmp =  env[0].getOutput() * mod_matrix[mod_nr].env0_amt;
	tmp += env[1].getOutput() * mod_matrix[mod_nr].env1_amt;
	tmp += env[2].getOutput() * mod_matrix[mod_nr].env2_amt;

	tmp += lfo[0].get_value() * mod_matrix[mod_nr].lfo0_amt;
	tmp += lfo[1].get_value() * mod_matrix[mod_nr].lfo1_amt;
	tmp += lfo[2].get_value() * mod_matrix[mod_nr].lfo2_amt;
	return tmp;
}



void updateOsc(Osc &osc, size_t osc_nr)
{
	// octave switcher
	// osc_mod.octave range (0:1)
	// multiply by 5 and substract 2 for center pitch scale (zero in (-2:2) range)
	// * 12 for octave multiply
	auto octave = ( (uint_fast8_t)(osc_m[osc_nr].octave * 5)- 2) * 12;

	stk::StkFloat osc_freq = osc_m[osc_nr].freq + octave;


	//osc_freq += getModVal(osc_nr*OSC_MOD_COUNT) * env_range_in_notes;
	osc_freq += osc_m[osc_nr].detune * 20;
	osc_freq = (stk::StkFloat) 220.0 * stk::math::pow( 2.0, (osc_freq - 57.0) / 12.0 );

	osc.setFrequency(osc_freq);

	stk::StkFloat osc_level;
	osc_level = getModVal(1+(osc_nr*OSC_MOD_COUNT)) + osc_m[osc_nr].sin_level;
	osc.set_sin_level(osc_level);

	osc_level = getModVal(2+(osc_nr*OSC_MOD_COUNT)) + osc_m[osc_nr].saw_level;
	osc.set_saw_level(osc_level);

	osc_level = getModVal(3+(osc_nr*OSC_MOD_COUNT)) + osc_m[osc_nr].sqr_level;
	osc.set_sqr_level(osc_level);

	osc_level = getModVal(4+(osc_nr*OSC_MOD_COUNT)) + osc_m[osc_nr].rnd_level;
	osc.set_noise_level(osc_level);
}

void updateFilter(MoogFilter *filter)
{
	stk::StkFloat flt_freq = flt_mod_matrix.frequency;

	auto tmp = getModVal(FLT_MOD_OFFSET) ;

	flt_freq += tmp * 12;


	flt_freq = (stk::StkFloat) 220.0 * stk::math::pow( 2.0, (flt_freq - 57.0) / 12.0 );

	if(flt_freq > 20000)flt_freq = 20000.; //moogfliter fixed upper limit to avoid aliasing


	filter->setCutoff(flt_freq);
	filter->setRes(flt_mod_matrix.resonance);
}

void controlCange(uint8_t param, uint8_t value)
{
	const stk::StkFloat divider = 1. / 127.;

	stk::StkFloat val=value;
	if(param < 13)
		if(param > 0)
			if(val==0)
				val=0.001*127.; // avoid pops and clicks

	if(param >= OSC_OFFSET && param <= OSC_OFFSET+(OSC_NUMBER * OSC_PARAMS) )
	{
		uint_fast8_t tmp_param = (param - OSC_OFFSET)%OSC_PARAMS;
		uint_fast8_t osc_number = (param - OSC_OFFSET) / OSC_PARAMS;
		switch (tmp_param)
		{
			case 0:
			{
				//std::cout << "osc " << +osc_number <<  " octave" << std::endl;
				this->osc_m[osc_number].octave = val*divider;
			}
			break;
			case 1:
			{
				//std::cout << "osc " << +osc_number <<  " detune" << std::endl;
				this->osc_m[osc_number].detune = val*divider;
			}
			break;

			case 2:
			{
				//std::cout << "osc " << +osc_number <<  " sin level" << std::endl;
				this->osc_m[osc_number].sin_level=(val*divider);
			}
			break;
			case 3:
			{
				//std::cout << "osc " << +osc_number <<  " saw level" << std::endl;
				this->osc_m[osc_number].saw_level=(val*divider);
			}
			break;
			case 4:
			{
				//std::cout << "osc " << +osc_number <<  " sqr level" << std::endl;
				this->osc_m[osc_number].sqr_level=(val*divider);
			}
			break;
			case 5:
			{
				//std::cout << "osc " << +osc_number <<  " rnd level" << std::endl;
				this->osc_m[osc_number].rnd_level=(val*divider);
			}
			break;
		}
	}




	if(param >= ADSR_OFFSET && param < ADSR_OFFSET+(ADSR_NUMBER*ADSR_PARAMS) )
	{
		uint_fast8_t tmp_param = param - ADSR_OFFSET;
		uint_fast8_t adsr_number = tmp_param / ADSR_PARAMS;
		tmp_param = tmp_param % ADSR_PARAMS;

		switch (tmp_param)
		{

			case 0:
			{
				//std::cout << "adsr " << adsr_number <<  " attack" << std::endl;
				this->env[adsr_number].setAttackRate(val*divider);
			}
			break;
			case 1:
			{
				//std::cout << "adsr " << adsr_number <<  " decay" << std::endl;
				this->env[adsr_number].setDecayRate(val*divider);
			}
			break;
			case 2:
			{
				//std::cout << "adsr " << adsr_number <<  " sustain" << std::endl;
				this->env[adsr_number].setSustainLevel(val*divider);
			}
			break;
			case 3:
			{
				//std::cout << "adsr " << adsr_number <<  " release" << std::endl;
				this->env[adsr_number].setReleaseRate(val*divider);
			}
			break;
		}
	}


	if(param >= FLT_OFFSET && param <= FLT_OFFSET+(FLT_NUMBER * FLT_PARAMS) )
	{
		uint_fast8_t tmp_param = param - FLT_OFFSET;
		tmp_param = tmp_param % FLT_PARAMS;
		switch (tmp_param)
		{
			case 0:
			{
				//std::cout << "filter freq " << val << std::endl;
				this->flt_mod_matrix.frequency = val;
			}
			break;
			case 1:
			{
				//std::cout << "filter res" << std::endl;
				this->flt_mod_matrix.resonance = val * divider;
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
				//std::cout << "lfo " << lfo_number <<  " shape" << std::endl;
				this->lfo[lfo_number].setShape(val*divider*4);
			}
			break;
			case 1:
			{
				//std::cout << "lfo " << lfo_number <<  " frequency" << std::endl;
				this->lfo[lfo_number].setFrequency( (val*divider * 10.) + 0.0001);
			}
			break;
		}
	}

	




	if(param >= MATRIX_MOD_OFFSET && param < MATRIX_MOD_OFFSET+(MATRIX_MOD_MATRIX_ITEMS * MATIRX_MOD_PARAMS_COUNT))
	{
		// tmp_param = 0 for first item starting from MATRIX_MOD_OFFSET
		uint_fast8_t tmp_param = param - MATRIX_MOD_OFFSET;

		int mod_param_matrix_nr = tmp_param / MATIRX_MOD_PARAMS_COUNT;
		int param_nr = tmp_param % MATIRX_MOD_PARAMS_COUNT;


		switch (param_nr)
		{
			case 0:
			{
				mod_matrix[mod_param_matrix_nr].env0_amt = val*divider;
			}
			break;
			case 1:
			{
				mod_matrix[mod_param_matrix_nr].env1_amt = val*divider;
			}
			break;
			case 2:
			{
				mod_matrix[mod_param_matrix_nr].env2_amt = val*divider;
			}
			break;
			case 3:
			{
				mod_matrix[mod_param_matrix_nr].lfo0_amt = val*divider;
			}
			break;
			case 4:
			{
				mod_matrix[mod_param_matrix_nr].lfo1_amt = val*divider;
			}
			break;
			case 5:
			{
				mod_matrix[mod_param_matrix_nr].lfo2_amt = val*divider;
			}
			break;
		}
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
