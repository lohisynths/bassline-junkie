/*
 * cpucounter.cpp
 *
 *  Created on: Mar 24, 2017
 *      Author: alax
 */

#include "cpucounter.h"

cpu_counter::cpu_counter()
{
	times.reserve(iter);
}

cpu_counter::~cpu_counter()
{
}

void cpu_counter::start()
{
	start_time = std::chrono::steady_clock::now();
}

std::chrono::duration<double, std::milli> cpu_counter::stop()
{
	std::chrono::duration<double, std::milli> fp_ms =
			std::chrono::steady_clock::now() - start_time;
	return fp_ms;
}

bool cpu_counter::update()
{
	auto duration = stop().count();

	if(i>start_threshold)
				times.push_back(duration);
	i++;
	if(i>iter)
	{
		i=0;
		auto data = reinterpret_cast<char *>(times.data());
		auto data_size = sizeof(times.at(0)) * times.size();
		std::string dupka = "data.bin";
		std::ofstream outfile_m(dupka, std::ios::binary);
		outfile_m.write(data, data_size);
		times.clear();
		return false;
	}
	return true;
}

