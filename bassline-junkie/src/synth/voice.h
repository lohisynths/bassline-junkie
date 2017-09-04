/*
 * synth.h
 *
 *  Created on: Mar 14, 2017
 *      Author: alax
 */

#ifndef SYNTH_H_
#define SYNTH_H_

#include <array>
#include <BlitSaw.h>
#include <BlitSquare.h>
#include <SineWave.h>

#include "dsp/ADSR/ADSR.h"
#include "dsp/Lfo.h"
#include "dsp/Osc.h"

#include "dsp/MoogFilter.h"
#include "dsp/VAStateVariableFilter.h"

#include "utils/MidiReceiver.h"
#include "utils/clipping.h"


class Voice
{
	struct mod_matrix
	{
		stk::StkFloat env0_amt;
		stk::StkFloat env1_amt;
		stk::StkFloat env2_amt;

		stk::StkFloat lfo0_amt;
		stk::StkFloat lfo1_amt;
		stk::StkFloat lfo2_amt;
	};

public:
	Voice();

	virtual ~Voice();

	std::array<stk::StkFloat, 512>& get_array()
	{
		return array;
	}

	void process();

	void message(MidiMessage *msg);

	void noteOn(stk::StkFloat note, stk::StkFloat vel);
	void noteOff();
	void controlCange(uint8_t param, uint8_t value);


	struct osc_mod_struct
	{
		stk::StkFloat freq=0;
		stk::StkFloat detune=0;
		stk::StkFloat octave=0;
		mod_matrix osc_mod={0};
	};
	std::array<osc_mod_struct, 3> osc_mod_matrix;

	struct amp_mod_struct
	{
		stk::StkFloat velocity=0;
		mod_matrix amp_mod_matrix={0};
	};
	amp_mod_struct amp_mod_matrix;


	struct flt_mod_struct
	{
		stk::StkFloat frequency=0;
		stk::StkFloat resonance=0;
		mod_matrix flt_mod_matrix={0};
	};
	flt_mod_struct flt_mod_matrix;



private:

	MoogFilter filter;
	//VAStateVariableFilter filter;

	std::array<Osc, 3> osc;
	std::array<Lfo, 3> lfo;
	std::array<ADSR, 3> env;

	std::array<stk::StkFloat, 512> array;
};

#endif /* SYNTH_H_ */
