/*
 * AudioDeviceRt.h
 *
 *  Created on: Jul 7, 2017
 *      Author: alax
 */

#ifndef AUDIODEVICE_RT_H_
#define AUDIODEVICE_RT_H_

#include <alsa/asoundlib.h>
#include <string>
#include <array>

#include "Stk.h"
#include "RtWvOut.h"

#include <algorithm>


class AudioDeviceRt
{
public:
	AudioDeviceRt();
	virtual ~AudioDeviceRt();

	void play(std::array<stk::StkFloat, 512> &arr);


private:

	int setup();

	void close();

	  stk::RtWvOut *dac=nullptr;


};

#endif /* AUDIODEVICE_RT_H_ */
