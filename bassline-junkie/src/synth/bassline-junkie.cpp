//============================================================================
// Name        : bassline-junkie.cpp
// Author      : alax
// Version     : 0
// Copyright   : TODO
// Description : libasound + libstk C++ Hello World
//============================================================================
#include <signal.h>
#include <sys/mman.h>
#include <cstdio>
#include <algorithm>

#include "utils/AudioDevice.h"
#include "utils/AudioDeviceRt.h"
#include "utils/cpucounter.h"
#include "utils/wavwriter.h"
#include "utils/concurency_helpers.h"
#include <RtWvOut.h>
#include "engines/engine.h"

int main()
{
	signal(SIGINT, finish);

	Stk::setSampleRate( 44100.0 );
	Stk::showWarnings( true );

	AudioDeviceRt<buffer_size> device;
	cpu_counter licznik;

	Engine<overall_voices_count, buffer_size> engine;

	// Lock memory only after the audio/worker threads are alive.
	// MCL_FUTURE can make thread creation fail on constrained Pi setups.
	if (mlockall(MCL_CURRENT) != 0) {
		perror("mlockall");
	}

	stick_this_thread_to_core(1);
	set_pthread_params();

	g_play = 1;

	while ( g_play )
	{

		// start reporting time
		// licznik.start();
		{
		// process data and send buffer to soundcard
		auto &output = engine.process();
		if (!g_play)
		{
			break;
		}
		device.play(output);
		// stop time reporting
		// write buffer to file
		// wav_out.tick(output);
		}

		// if(!licznik.update())
			;// play=false;
	}
	device.close();
	return 0;
}
