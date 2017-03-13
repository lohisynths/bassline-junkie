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

std::array<double,512> array;


int main()
{
	signal(SIGINT, finish);

	stk::Stk::setSampleRate(44100);

	device = new AudioDevice;





	while (!done)
	{
//		for(int i=0; i<array.size();i++)
//			array[i] = sine.tick();

		device->play(array);
	}

	return 0;
}
