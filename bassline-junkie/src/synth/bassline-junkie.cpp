//============================================================================
// Name        : bassline-junkie.cpp
// Author      : alax
// Version     : 0
// Copyright   : TODO
// Description : libasound + libstk C++ Hello World
//============================================================================


#include <signal.h>
#include "FileWvOut.h"

#include "AudioDevice.h"
#include "synth.h"
#include "cpucounter.h"
#include "wavwriter.h"
#include "concurency_helpers.h"
#include <fstream>


static bool play = true;

static void finish(int ignore)
{
	printf("finish finish finish finish finish finish\n");
	play = false;
}

#include "tbb/tbb.h"


const unsigned int iter = 1000;

#include <sys/mman.h>

#include "engines/sharedfutures_sleep.h"
#include "engines/sharedfutures.h"


int main()
{
	shared_futures_sleep silnik;

	mlockall(MCL_FUTURE | MCL_CURRENT);


	signal(SIGINT, finish);
	stick_this_thread_to_core(3);
	set_pthread_params();


	stk::Stk::setSampleRate(44100);

	AudioDevice device;
	//wav_writer wav_out;
	cpu_counter licznik;

	std::array<synth, voices_count> voices;
	std::array<int, 512> output;

	for (auto &voice : voices)
	{
		auto elo = std::distance(voices.begin(), &voice);
		voice.init(50 + elo * 50);
	}


	std::vector<double> times;
	times.reserve(iter);

	while ( play )
	{
		if (device.aval())
		{
			// start reporting time
			licznik.start();

			// clear output buffer
			std::fill(std::begin(output), std::end(output), 0);


			silnik.process(voices);



			// proces new samples
//			for (auto &voice : voices)
//			{
//			voices[0].process();
//			voices[1].process();
//			}


//			tbb::parallel_for(
//					size_t(0), voices_count, size_t(1), [=](size_t i)
//					{
//						voices[i].process();
//					}
//			);


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




			static unsigned int i = 0;

			// stop time reporting
			auto duration = licznik.stop().count();
			if(i>100)
				times.push_back(duration);



			// write buffer to file
			// wav_out.tick(output);


			i++;
			if(i>iter)
				play = false;


		}



	}



	auto data = reinterpret_cast<char *>(times.data());
	auto data_size = sizeof(times.at(0)) * times.size();

	std::ofstream outfile_m("data.bin", std::ios::binary);

	outfile_m.write(data, data_size);

	return 0;
}
