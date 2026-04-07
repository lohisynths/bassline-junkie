//============================================================================
// Name        : bassline-junkie.cpp
// Author      : alax
// Version     : 0
// Copyright   : TODO
// Description : libasound C++ Hello World
//============================================================================
#include <signal.h>
#include <sys/mman.h>
#include <cstdio>
#include <algorithm>

#include "utils/AudioDevice.h"
#include "utils/cpucounter.h"
#include "utils/wav_writer.h"
#include "utils/concurency_helpers.h"
#include "engines/engine.h"

int main()
{
	signal(SIGINT, finish);

	AudioDevice<buffer_size> device;
//	stmlib::WavWriter wav_out(1, sample_rate, 0);
//	if (!wav_out.Open2("bassline.wav")) {
//		perror("bassline.wav");
//		return 1;
//	}
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
//		wav_out.Write(output.data(), buffer_size);
		}

		// if(!licznik.update())
			;// play=false;
	}
	engine.stop();
	device.close();
	return 0;
}
