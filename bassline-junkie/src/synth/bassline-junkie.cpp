//============================================================================
// Name        : bassline-junkie.cpp
// Author      : alax
// Version     : 0
// Copyright   : TODO
// Description : libasound + libstk C++ Hello World
//============================================================================
#include <signal.h>
#include <sys/mman.h>
#include <algorithm>

#include "utils/AudioDevice.h"
#include "utils/cpucounter.h"
#include "utils/wavwriter.h"
#include "utils/concurency_helpers.h"
#include "engines/conditional_variable.h"

#include <RtWvOut.h>

int main()
{
	mlockall(MCL_FUTURE | MCL_CURRENT);

	signal(SIGINT, finish);
	stick_this_thread_to_core(1);
	set_pthread_params();

	Stk::setSampleRate( 44100.0 );
	Stk::showWarnings( true );

	AudioDevice device;
//	  stk::RtWvOut *dac=nullptr;
//	  try {
//	    // Define and open the default realtime output device for one-channel playback
//	    dac = new RtWvOut( 1, Stk::sampleRate(), 0, 256, 2);
//
//	  }
//	  catch ( StkError & ) {
//	    exit( 1 );
//	  }

	
	wav_writer wav_out;
	cpu_counter licznik;

	std::array<Voice, voices_count> voices;
	ConditionalVarEngine engine(voices);

	play = true;

	while ( play )
	{

		// start reporting time
		// licznik.start();
		{
		// process data
		auto output = engine.process(voices);
		// send buffer to soundcard



//			for(auto &it : output)
//			{
//			    try {
//			      dac->tick( it );
//			     }
//			    catch ( StkError & ) {
//			    	std::cout << "errrorrorororororoo n''n'n'n'\n\\n\n\n\n";
//			    }
//
//			}


		device.play(output); // while loop inside
		// stop time reporting
		// write buffer to file
		// wav_out.tick(output);
		}

		// if(!licznik.update())
			;// play=false;
	}
	return 0;
}
