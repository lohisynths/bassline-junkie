/*
 * cpucounter.cpp
 *
 *  Created on: Mar 24, 2017
 *      Author: alax
 */

#include "cpucounter.h"

cpu_counter::cpu_counter()
{
	plot = 	new Plot("ciabejek", 0, 20, Plot::transfer::scroll);


}

cpu_counter::~cpu_counter()
{

}

void cpu_counter::start()
{
	start_time = std::chrono::steady_clock::now();
}

void cpu_counter::stop()
{
	std::chrono::duration<double, std::milli> fp_ms =
			std::chrono::steady_clock::now() - start_time;

	plot->update(fp_ms.count());
}

