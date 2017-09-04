/*
 * AudioDeviceRt.cpp
 *
 *  Created on: Jul 7, 2017
 *      Author: alax
 */

#include "AudioDeviceRt.h"

AudioDeviceRt::AudioDeviceRt()
{
	setup();
}

AudioDeviceRt::~AudioDeviceRt()
{
	this->close();
}

int AudioDeviceRt::setup()
{
	  try {
	    // Define and open the default realtime output device for one-channel playback
	    dac = new stk::RtWvOut( 1, stk::Stk::sampleRate(), 0, 256, 2);

	  }
	  catch ( stk::StkError & ) {
		  // TODO: think whats next
	    exit( 1 );
	  }
}

void AudioDeviceRt::close()
{


}

const unsigned int format_bits = 32; //snd_pcm_format_width(*m_format);
const unsigned int maxval = (1U << (format_bits - 1U)) - 1U;

void AudioDeviceRt::play(std::array<stk::StkFloat, 512> &output)
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
