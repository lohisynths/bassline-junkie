/*
 * wavwriter.cpp
 *
 *  Created on: Mar 24, 2017
 *      Author: alax
 */

#include "wavwriter.h"

wav_writer::wav_writer()
{
	wave_output.openFile("bassline.wav", 1, stk::FileWrite::FILE_WAV,
			stk::Stk::STK_FLOAT64);
	wave_output.printErrors(true);
	output_frames.resize(512, 1);

}

wav_writer::~wav_writer()
{

}

void wav_writer::tick(std::array<stk::StkFloat, 512> &output)
{
	for (auto &sample : output)
	{
		auto elo = std::distance(output.begin(), &sample);
		output_frames[elo] = sample;
	}

	wave_output.tick(output_frames);
}
