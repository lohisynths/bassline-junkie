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
#include "utils/AudioDeviceRt.h"
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

	AudioDeviceRt device;
	
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
