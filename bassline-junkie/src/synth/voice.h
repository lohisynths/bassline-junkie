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
#include <FileLoop.h>

#include "ADSR.h"

#include <cmath>
#include "dsp/MoogFilter.h"
#include "utils/MidiReceiver.h"



class Voice
{
	struct mod_matrix
	{
		stk::StkFloat main;
		stk::StkFloat adsr1_amt;
		stk::StkFloat adsr2_amt;
		stk::StkFloat adsr3_amt;

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
	void controlCange(uint8_t param, uint8_t val);


	mod_matrix osc1_mod_matrix={};
	mod_matrix amp_mod_matrix={};
	mod_matrix flt_mod_matrix={};


private:
	//stk::FileLoop *waves_;

	stk::BlitSaw osc;
	MoogFilter filter;

	stk::ADSR adsr[3];


	double flt_res;
	std::array<double, 512> array;
};

#endif /* SYNTH_H_ */
