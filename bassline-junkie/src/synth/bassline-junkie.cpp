//============================================================================
// Name        : bassline-junkie.cpp
// Author      : alax
// Version     : 0
// Copyright   : TODO
// Description : libasound + libstk C++ Hello World
//============================================================================

#include <future>

#include <signal.h>
#include "FileWvOut.h"

#include "AudioDevice.h"
#include "synth.h"
#include "cpucounter.h"
#include "wavwriter.h"

const size_t voices_count = 4;

static bool play = true;

static void finish(int ignore)
{
	printf("finish finish finish finish finish finish\n");
	play = false;
}

#include "tbb/tbb.h"

int main()
{
	signal(SIGINT, finish);
	stick_this_thread_to_core(4);
	set_pthread_params();

	stk::Stk::setSampleRate(44100);

	AudioDevice device;
	wav_writer wav_out;
	cpu_counter licznik;

	std::array<synth, voices_count> voices;
	std::array<int, 512> output;

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


			std::promise<void> t1_ready_promise;
			std::promise<void> t2_ready_promise;
			std::promise<void> ready_promise;

			std::shared_future<void> ready_future(ready_promise.get_future());




			// clear output buffer
			std::fill(std::begin(output), std::end(output), 0);




			auto fun1 = [&]()
			{
				stick_this_thread_to_core(5);
				t1_ready_promise.set_value();
				ready_future.wait(); // waits for the signal from main()

				voices[0].process();
				voices[1].process();
			};


			auto fun2 = [&]()
			{
				stick_this_thread_to_core(6);
				t2_ready_promise.set_value();
				ready_future.wait(); // waits for the signal from main()

				voices[2].process();
				voices[3].process();
			};


			auto result1 = std::async(std::launch::async, fun1);
			auto result2 = std::async(std::launch::async, fun2);


			// wait for the threads to become ready
			t1_ready_promise.get_future().wait();
			t2_ready_promise.get_future().wait();
			// signal the threads to go
			ready_promise.set_value();

			result1.get();
			result2.get();



			// proces new samples
//			for (auto &voice : voices)
//			{
//				voice.process();
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

			// stop time reporting
			licznik.stop();

			// write buffer to file
			wav_out.tick(output);

		}
	}
	return 0;
}
