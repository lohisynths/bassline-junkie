//============================================================================
// Name        : bassline-junkie.cpp
// Author      : alax
// Version     : 0
// Copyright   : TODO
// Description : libasound + libstk C++ Hello World
//============================================================================
#include <signal.h>
#include <sys/mman.h>
#include <algorithm>
#include <memory>
#include "spdlog/spdlog.h"

#include "utils/AudioDevice.h"
#include "utils/AudioDeviceRt.h"
#include "utils/cpucounter.h"
#include "utils/wavwriter.h"
#include "utils/concurency_helpers.h"
#include <RtWvOut.h>
#include "engines/engine.h"
#include "utils/Logger.h"



int main()
{

	std::shared_ptr<spdlog::logger> engine_logger=nullptr;
	std::shared_ptr<spdlog::logger> thread_logger=nullptr;
	std::shared_ptr<spdlog::logger> synth_logger=nullptr;
	std::shared_ptr<spdlog::logger> serial_logger=nullptr;


	synth_logger = spdlog::stdout_color_mt("synth");
	engine_logger = spdlog::stdout_color_mt("engine");
	thread_logger = spdlog::stdout_color_mt("thread");
	serial_logger = spdlog::stdout_color_mt("serial");

	synth_logger->set_level(spdlog::level::debug); // Set specific logger's log level
	engine_logger->set_level(spdlog::level::debug); // Set specific logger's log level
	thread_logger->set_level(spdlog::level::debug); // Set specific logger's log level
	serial_logger->set_level(spdlog::level::debug); // Set specific logger's log level


	mlockall(MCL_FUTURE | MCL_CURRENT);
	signal(SIGINT, finish);


	auto logger = spdlog::get("synth");
	if(!logger)
	{
		std::cerr << "failed to get synth logger.\n";
		exit(1);
	}


	if(stick_this_thread_to_core(1) < 0)
	{
		LOG_WARN(logger, "failed to set phtread params");
	}
	if(set_pthread_params() < 0)
	{
		LOG_WARN(logger, "failed to set phtread params");
	}
	Stk::setSampleRate( 44100.0 );
	Stk::showWarnings( true );

	AudioDeviceRt<buffer_size> device;
	
	wav_writer wav_out;
	cpu_counter licznik;

	Engine<overall_voices_count, buffer_size> engine;



	play = true;

	while ( play )
	{

		// start reporting time
		// licznik.start();
		{
		// process data and send buffer to soundcard
		auto &output = engine.process();
		device.play(output);
		// stop time reporting
		// write buffer to file
		// wav_out.tick(output);
		}

		// if(!licznik.update())
			;// play=false;
	}
	return 0;
}
