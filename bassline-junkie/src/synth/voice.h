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
		stk::StkFloat adsr0_amt;
		stk::StkFloat adsr1_amt;
		stk::StkFloat adsr2_amt;

		stk::StkFloat lfo1_amt;
		stk::StkFloat lfo2_amt;
		stk::StkFloat lfo3_amt;
	};

public:
	Voice();

	virtual ~Voice();

	std::array<double, 512>& get_array()
	{
		return array;
	}

	void process();

	void init(double freq);


	void message(MidiMessage *msg);

	void noteOn(double note, double vel);
	void noteOff();
	void controlCange(uint8_t param, uint8_t value);


	mod_matrix osc_mod_matrix={0};
	mod_matrix amp_mod_matrix={0};
	mod_matrix flt_mod_matrix={0};

	double flt_res;
	double osc2_detune;

private:
	//stk::FileLoop *waves_;

	stk::BlitSaw osc;
	stk::BlitSaw osc2;

	MoogFilter filter;

	std::array<stk::ADSR, 3> adsr;

	std::array<double, 512> array;
};

#endif /* SYNTH_H_ */
