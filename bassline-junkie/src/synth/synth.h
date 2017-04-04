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
#include <cmath>
#include "MoogFilter.h"

const unsigned int format_bits = 32; //snd_pcm_format_width(*m_format);
const unsigned int maxval = (1U << (format_bits - 1U)) - 1U;

class synth
{
public:
	synth();

	virtual ~synth();

	std::array<int, 512>& get_array()
	{
		return array;
	}

	void process()
	{
		for (auto &arr : array)
		{
			auto max_iter = 60;
			auto lolo = osc.tick();
			for (int i = 0; i < max_iter; i++)
			{
				osc.setFrequency(osc_freq);

				filter.setCutoff(flt_freq);
				filter.setRes(flt_res);
				lolo = filter.process(lolo);
				lolo = std::tanh(lolo);

			}

			if (lolo > 1)
				lolo = 1;
			else if (lolo < -1)
				lolo = -1;

			arr = lolo * maxval;
		}
	}

	void init(double freq);

private:
	stk::BlitSaw osc;
	MoogFilter filter;

	double osc_freq;
	double flt_freq;
	double flt_res;
	std::array<int, 512> array;
};

#endif /* SYNTH_H_ */
