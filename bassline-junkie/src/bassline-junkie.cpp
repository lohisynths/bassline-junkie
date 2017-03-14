//============================================================================
// Name        : bassline-junkie.cpp
// Author      : alax
// Version     : 0
// Copyright   : TODO
// Description : libasound + libstk C++ Hello World
//============================================================================


#include "AudioDevice.h"



#include <iostream>
#include <algorithm>
#include <signal.h>



#include <stk/SineWave.h>


AudioDevice* device;

//


static bool done=false;
static void finish(int ignore)
{
	printf("finish finish finish finish finish finish\n");
	done = true;
}


#include "PerformanceCounter.h"

PerformanceCounter cpu;

#include <unistd.h>

std::array<int,512> array;




#include <stk/SineWave.h>
stk::SineWave sine;



int main()
{
	signal(SIGINT, finish);

	stk::Stk::setSampleRate(44100);

	device = new AudioDevice;


	sine.setFrequency(440);


	std::array<int32_t, 512> tmp_data;


	while (!done)
	{
		if(device->aval())
		{
			for (unsigned int i = 0; i < tmp_data.size(); i++)
				tmp_data[i] = sine.tick() * maxval;

			device->play(tmp_data); // while loop inside
		}
	}

	return 0;
}
