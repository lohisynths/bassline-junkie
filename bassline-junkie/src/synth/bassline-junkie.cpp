//============================================================================
// Name        : bassline-junkie.cpp
// Author      : alax
// Version     : 0
// Copyright   : TODO
// Description : libasound + libstk C++ Hello World
//============================================================================


#include <signal.h>
#include <sys/mman.h>

#include <fstream>
#include <algorithm>


#include "AudioDevice.h"
#include "synth.h"
#include "cpucounter.h"
#include "wavwriter.h"
#include "concurency_helpers.h"

#include "FileWvOut.h"

#include "engines/sharedfutures_sleep.h"
#include "engines/sharedfutures.h"
#include "engines/serial_sleep.h"
#include "engines/serial.h"


static bool play = true;
const unsigned int start_threshold = 100;
const unsigned int iter = 1000 + start_threshold;
std::vector <std::tuple<Engine*, std::string> > silniki;


static void finish(int ignore)
{
	printf("finish finish finish finish finish finish\n");
	play = false;
}


int main()
{
	mlockall(MCL_FUTURE | MCL_CURRENT);

	signal(SIGINT, finish);
	stick_this_thread_to_core(councyrent_cores[0]);
	set_pthread_params();


	stk::Stk::setSampleRate(44100);

	AudioDevice device;
	
	silniki.push_back(std::make_pair(new shared_futures,"shared_futures"));
//	silniki.push_back(std::make_pair(new serial,"serial"));
//	silniki.push_back(std::make_pair(new shared_futures_sleep,"shared_futures_sleep"));
//	silniki.push_back(std::make_pair(new serial_sleep,"serial_sleep"));

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

	for (auto &engine : silniki)
	{
		play = true;

		auto silnik = std::get<0>(engine);

		while ( play )
		{
			if (device.aval())
			{
				// start reporting time
				licznik.start();
				for(int i = 0; i < 100; i++)
					silnik->updateMessages(voices);
				// clear output buffer
				std::fill(std::begin(output), std::end(output), 0);
				// process data
				silnik->process(voices);

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
				if(i>start_threshold)
					times.push_back(duration);

				// write buffer to file
				// wav_out.tick(output);

				i++;
				if(i>iter)
				{
					play = false;
					i=0;

					auto data = reinterpret_cast<char *>(times.data());
					auto data_size = sizeof(times.at(0)) * times.size();

					auto dupka = std::get<1>(engine);

					dupka += ".bin";
					std::cout << dupka << std::endl;
					std::ofstream outfile_m(dupka, std::ios::binary);
					outfile_m.write(data, data_size);
					times.clear();
				}
			}
		}
	}
	return 0;
}
