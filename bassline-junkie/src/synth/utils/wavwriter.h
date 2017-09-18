/*
 * wavwriter.h
 *
 *  Created on: Mar 24, 2017
 *      Author: alax
 */

#ifndef WAVWRITER_H_
#define WAVWRITER_H_

#include <array>
#include "FileWvOut.h"

class wav_writer
{
public:
	wav_writer();
	virtual ~wav_writer();

	void tick(std::array<stk::StkFloat, 512> &output);

private:
	stk::FileWvOut wave_output;
	stk::StkFrames output_frames;

};

#endif /* WAVWRITER_H_ */
