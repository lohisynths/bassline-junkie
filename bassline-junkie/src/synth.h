/*
 * synth.h
 *
 *  Created on: Mar 14, 2017
 *      Author: alax
 */

#ifndef SYNTH_H_
#define SYNTH_H_

#include <array>
#include <stk/BlitSaw.h>

const unsigned int format_bits = 32; //snd_pcm_format_width(*m_format);
const unsigned int maxval = (1U << (format_bits - 1U)) - 1U;

class synth
{
public:
	synth();
	virtual ~synth();

	void process(std::array<int, 512> &input);
	void init();

private:
	stk::BlitSaw sine;

};

#endif /* SYNTH_H_ */
