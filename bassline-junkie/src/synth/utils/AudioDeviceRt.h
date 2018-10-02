/*
 * AudioDeviceRt.h
 *
 *  Created on: Jul 7, 2017
 *      Author: alax
 */

#ifndef AUDIODEVICE_RT_H_
#define AUDIODEVICE_RT_H_

#include <alsa/asoundlib.h>
#include <iostream>
#include <string>
#include <array>
#include <algorithm>

#include "Stk.h"
#include "RtWvOut.h"


template<size_t buffer_size>
class AudioDeviceRt
{
public:
	AudioDeviceRt()
	{
		setup();
	}
	~AudioDeviceRt()
	{
		this->close();
	}

	void play(std::array<stk::StkFloat, buffer_size> &output)
	{
		for(auto &it : output)
		{
		    try {
		      dac->tick( it );
		     }
		    catch ( stk::StkError & ) {
		    	std::cout << "errrorrorororororoo n''n'n'n'\n\\n\n\n\n";
		    }
		}
	}

    void mute()
    {
        dac->tick(0);
    }

private:
	void setup()
	{
	  try {
		// Define and open the default realtime output device for one-channel playback
		dac = new stk::RtWvOut( 2, stk::Stk::sampleRate(), 0, buffer_size, 2);
	  }
	  catch ( stk::StkError & ) {
		  // TODO: think whats next
		exit( 1 );
	  }
	}
	void close()
	{
		delete dac;
	}

	stk::RtWvOut *dac=nullptr;
};

#endif /* AUDIODEVICE_RT_H_ */
