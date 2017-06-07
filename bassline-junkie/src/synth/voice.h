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

#include "ADSR.h"

#include "dsp/MoogFilter.h"
#include "utils/MidiReceiver.h"
#include "utils/clipping.h"


class Voice
{
	struct mod_matrix
	{
		stk::StkFloat main;
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

	void init(stk::StkFloat freq);


	void message(MidiMessage *msg);

	void noteOn(stk::StkFloat note, stk::StkFloat vel);
	void noteOff();
	void controlCange(uint8_t param, uint8_t value);


	mod_matrix osc_mod_matrix={0};
	mod_matrix flt_mod_matrix={0};

	mod_matrix amp_mod_matrix={0};


	stk::StkFloat flt_tune;
	stk::StkFloat flt_res;

	stk::StkFloat osc_tune;
	stk::StkFloat osc2_detune;

private:
	//stk::FileLoop *waves_;

	stk::BlitSaw osc;
	stk::BlitSaw osc2;

	MoogFilter filter;

	std::array<stk::ADSR, 3> env;

	std::array<stk::StkFloat, 512> array;
};

#endif /* SYNTH_H_ */
