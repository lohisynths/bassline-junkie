//============================================================================
// Name        : bassline-junkie.cpp
// Author      : alax
// Version     : 0
// Copyright   : TODO
// Description : libasound + libstk C++ Hello World
//============================================================================

#include <signal.h>

#include "AudioDevice.h"
#include <stk/SineWave.h>

AudioDevice device;
stk::SineWave sine;
std::array<int,512> array;

static bool play=true;



static void finish(int ignore)
{
	printf("finish finish finish finish finish finish\n");
	play = false;
}



int main()
{
	signal(SIGINT, finish);

	stk::Stk::setSampleRate(44100);
	sine.setFrequency(440);

	while (play)
	{
		if(device.aval())
		{
			for (unsigned int i = 0; i < array.size(); i++)
				array[i] = sine.tick() * maxval;

			device.play(array); // while loop inside
		}

	}

	return 0;
}
