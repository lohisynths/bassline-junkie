//============================================================================
// Name        : bassline-junkie.cpp
// Author      : alax
// Version     : 0
// Copyright   : TODO
// Description : libasound + libstk C++ Hello World
//============================================================================

#include <signal.h>
#include "stk/FileWvOut.h"

#include "AudioDevice.h"
#include "synth.h"
#include "cpucounter.h"
#include "wavwriter.h"

const size_t voices_count = 5;

AudioDevice device;
wav_writer wav_out;
cpu_counter licznik;

std::array<synth, voices_count> voices;
std::array<int, 512> output;

static bool play = true;

static void finish(int ignore)
{
	printf("finish finish finish finish finish finish\n");
	play = false;
}

int main()
{
	signal(SIGINT, finish);
	stick_this_thread_to_core(0);
	set_pthread_params();

	stk::Stk::setSampleRate(44100);

	for (auto &voice : voices)
	{
		auto elo = std::distance(voices.begin(), &voice);
		voice.init(50 + elo * 50);
	}

	while (play)
	{
		if (device.aval())
		{
			// start time reporting
			licznik.start();

			// clear output buffer
			std::fill(std::begin(output), std::end(output), 0);

			// proces new samples
			for (auto &voice : voices)
			{
				voice.process();
			}

			// fill output buffer with data
			for (auto &voice : voices)
			{
				auto &voice_data = voice.get_array();

				std::transform(voice_data.begin(), voice_data.end(),
						voice_data.begin(),
						std::bind1st(std::multiplies<double>(),
								1. / voices_count));

				std::transform(output.begin(), output.end(), voice_data.begin(),
						output.begin(), std::plus<int>());
			}

			// send buffer to soundcard
			device.play(output); // while loop inside

			// stop time reporting
			licznik.stop();

			// write buffer to file
			wav_out.tick(output);

		}
	}
	return 0;
}
