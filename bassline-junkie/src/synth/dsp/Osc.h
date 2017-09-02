	/*
 * Osc.h
 *
 *  Created on: Oct 3, 2016
 *      Author: alax
 */

#ifndef OSC_H_
#define OSC_H_

#include <memory>
#include "Stk.h"
#include "BlitSaw.h"
#include "BlitSquare.h"
#include "Noise.h"
#include "SineWave.h"

using namespace stk;

class Osc
{
public:
	Osc();
	virtual ~Osc();
	StkFloat tick();

	void setFrequency(StkFloat freq);
	void set_sin_level(StkFloat level);
	void set_saw_level(StkFloat level);
	void set_sqr_level(StkFloat level);
	void set_noise_level(StkFloat level);
	void reset();


private:
	struct waves_level{
		StkFloat sin_level;
		StkFloat saw_level;
		StkFloat sqr_level;
		StkFloat rnd_level;
	};
	waves_level m_osc_ctrl;
	std::unique_ptr<SineWave>m_sine = std::make_unique<SineWave>();
	std::unique_ptr<BlitSaw>m_saw = std::make_unique<BlitSaw>();
	std::unique_ptr<BlitSquare>m_square = std::make_unique<BlitSquare>();
	std::unique_ptr<Noise>m_noise = std::make_unique<Noise>();
};

#endif /* OSC_H_ */
