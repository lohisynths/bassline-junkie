/*
 * wavwriter.cpp
 *
 *  Created on: Mar 24, 2017
 *      Author: alax
 */

#include "wavwriter.h"

wav_writer::wav_writer()
{
	wave_output.openFile("/home/pi/bassline.wav", 1, stk::FileWrite::FILE_WAV,
			stk::Stk::STK_SINT32);
	wave_output.printErrors(true);
	lolo.resize(512, 1);

}

wav_writer::~wav_writer()
{

}

const unsigned int format_bits = 32; //snd_pcm_format_width(*m_format);
const unsigned int maxval = (1U << (format_bits - 1U)) - 1U;
#include <algorithm>

std::array<stk::StkFloat, 512> tmp;
void wav_writer::tick(std::array<unsigned int, 512> &output)
{
	for (auto &sample : output)
	{
		auto elo = std::distance(output.begin(), &sample);

		uint32_t piesek = sample;
		piesek -= maxval + 1;

		double czesc = 	(double)piesek / (double)maxval;//(double) sample / (double) maxval;

		lolo[elo] = czesc - 1.;
	}

	wave_output.tick(lolo);
}
