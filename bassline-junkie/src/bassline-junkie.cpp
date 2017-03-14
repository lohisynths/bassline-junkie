//============================================================================
// Name        : bassline-junkie.cpp
// Author      : alax
// Version     : 0
// Copyright   : TODO
// Description : libasound + libstk C++ Hello World
//============================================================================

#include <signal.h>

#include "AudioDevice.h"
#include "synth.h"
#include "Plot.h"

AudioDevice device;
std::array<int, 512> array;

static bool play = true;

static void finish(int ignore)
{
	printf("finish finish finish finish finish finish\n");
	play = false;
}

int main()
{
	signal(SIGINT, finish);

	Plot plot("ciabejek", 0, 1, Plot::transfer::scroll);
	stick_this_thread_to_core(0);
	//set_pthread_params();
	synth gracz;
	gracz.init();

	while (play)
	{
		if (device.aval())
		{
			auto ciabejek_start = std::chrono::steady_clock::now();

			gracz.process(array);

			device.play(array); // while loop inside

			std::chrono::duration<double, std::milli> fp_ms =
					std::chrono::steady_clock::now() - ciabejek_start;

			plot.update(fp_ms.count());
		}

	}

	return 0;
}
