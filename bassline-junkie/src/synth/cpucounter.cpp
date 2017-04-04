/*
 * cpucounter.cpp
 *
 *  Created on: Mar 24, 2017
 *      Author: alax
 */

#include "cpucounter.h"

cpu_counter::cpu_counter()
{
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

