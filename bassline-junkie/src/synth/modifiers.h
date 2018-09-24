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


#define OSC_OFFSET 0
#define OSC_NUMBER 3
#define OSC_PARAMS 5

#define ADSR_OFFSET 18
#define ADSR_NUMBER 3
#define ADSR_PARAMS 5

#define FLT_OFFSET  33
#define FLT_NUMBER  1
#define FLT_PARAMS 3

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



	mod_matrix_s mod_matrix[MATRIX_MOD_MATRIX_ITEMS];



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
	//auto octave = ( (uint_fast8_t)(osc_m[osc_nr].octave * 5)- 2) * 12;

	//stk::StkFloat osc_freq = osc_m[osc_nr].freq + octave;
    stk::StkFloat osc_freq = osc_m[osc_nr].freq;


	osc_freq += getModVal(osc_nr*OSC_MOD_COUNT) * env_range_in_notes;
	osc_freq += osc_m[osc_nr].detune * 1;
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

	flt_freq += tmp * 48;


	flt_freq = (stk::StkFloat) 220.0 * stk::math::pow( 2.0, (flt_freq - 57.0) / 12.0 );

	if(flt_freq > 15000)flt_freq = 15000.; //moogfliter fixed upper limit to avoid aliasing


	filter->setCutoff(flt_freq);
	filter->setRes(flt_mod_matrix.resonance);
}

struct MyCout
 {
   std::stringstream s;

   template <typename T>
   MyCout& operator << (const T &x) {
     s << x;
     return *this;
   }

   ~MyCout() {
     //std::cout << s.str();
   }
 };

void controlCange(uint8_t param, uint8_t value)
{
	const stk::StkFloat divider = 1. / 127.;

	stk::StkFloat val=value;

	if(param >= OSC_OFFSET && param < OSC_OFFSET+(OSC_NUMBER * OSC_PARAMS) )
	{
		uint_fast8_t tmp_param = (param - OSC_OFFSET)%OSC_PARAMS;
		uint_fast8_t osc_number = (param - OSC_OFFSET) / OSC_PARAMS;
		switch (tmp_param)
		{
			case 0:
			{
			    MyCout() << "osc " << +osc_number <<  " detune\n";
                this->osc_m[osc_number].detune = val*divider;
				//this->osc_m[osc_number].octave = val*divider;
			}
			break;
			case 1:
			{
			    MyCout() << "osc " << +osc_number <<  " sin level\n";
				this->osc_m[osc_number].sin_level=(val*divider);
			}
			break;
			case 2:
			{
			    MyCout() << "osc " << +osc_number <<  " saw level\n";
				this->osc_m[osc_number].saw_level=(val*divider);
			}
			break;
			case 3:
			{
			    MyCout() << "osc " << +osc_number <<  " sqr level\n";
				this->osc_m[osc_number].sqr_level=(val*divider);
			}
			break;
			case 4:
			{
			    MyCout() << "osc " << +osc_number <<  " rnd level\n";
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
			    MyCout() << "adsr " << +adsr_number <<  " attack\n";
				this->env[adsr_number].setAttackRate(val*divider);
			}
			break;
			case 1:
			{
			    MyCout() << "adsr " << +adsr_number <<  " decay\n";
				this->env[adsr_number].setDecayRate(val*divider);
			}
			break;
			case 2:
			{
			    MyCout() << "adsr " << +adsr_number <<  " sustain\n";
				this->env[adsr_number].setSustainLevel(val*divider);
			}
			break;
			case 3:
			{
			    MyCout() << "adsr " << +adsr_number <<  " release\n";
				this->env[adsr_number].setReleaseRate(val*divider);
			}
            break;
            case 4:
            {
                MyCout() << "adsr " << +adsr_number <<  " loop\n";
                //this->env[adsr_number].setReleaseRate(val*divider);
            }
			break;
		}
	}


	if(param >= FLT_OFFSET && param < FLT_OFFSET+(FLT_NUMBER * FLT_PARAMS) )
	{
		uint_fast8_t tmp_param = param - FLT_OFFSET;
		tmp_param = tmp_param % FLT_PARAMS;
		switch (tmp_param)
		{
			case 0:
			{
			    MyCout() << "filter freq " << val << "\n";
				this->flt_mod_matrix.frequency = val;
			}
			break;
			case 1:
			{
			    MyCout() << "filter res " << val << "\n";
				this->flt_mod_matrix.resonance = val * divider;
			}
			break;
            case 2:
            {
                MyCout() << "filter shape " << val << "\n";
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
			    MyCout() << "lfo " << +lfo_number <<  " frequency " <<  val << "\n";
                this->lfo[lfo_number].setFrequency( (val*divider * 10.) + 0.0001);
			}
			break;
			case 1:
			{
			    MyCout() << "lfo " << +lfo_number <<  " shape " << val << "\n";
                this->lfo[lfo_number].setShape(val*divider*4);
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
			case 0: {
			    MyCout() << "env0_amt dst " << +mod_param_matrix_nr << " value " << val << "\n";
				mod_matrix[mod_param_matrix_nr].env0_amt = val*divider;
			}
			break;
			case 1: {
			    MyCout() << "env1_amt dst " << +mod_param_matrix_nr << " value " << val << "\n";
				mod_matrix[mod_param_matrix_nr].env1_amt = val*divider;
			}
			break;
			case 2: {
			    MyCout() << "env2_amt dst " << +mod_param_matrix_nr << " value " << val << "\n";
				mod_matrix[mod_param_matrix_nr].env2_amt = val*divider;
			}
			break;
			case 3: {
			    MyCout() << "lfo0_amt dst " << +mod_param_matrix_nr << " value " << val << "\n";
				mod_matrix[mod_param_matrix_nr].lfo0_amt = val*divider;
			}
			break;
			case 4: {
			    MyCout() << "lfo1_amt dst " << +mod_param_matrix_nr << " value " << val << "\n";
				mod_matrix[mod_param_matrix_nr].lfo1_amt = val*divider;
			}
			break;
			case 5: {
			    MyCout() << "lfo2_amt dst " << +mod_param_matrix_nr << " value " << val << "\n";
				mod_matrix[mod_param_matrix_nr].lfo2_amt = val*divider;
			}
			break;
			default: {
			    MyCout() << "error w huj dst " << +mod_param_matrix_nr << " value " << val << "\n";

			}
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
