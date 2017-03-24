/*
 * cpucounter.h
 *
 *  Created on: Mar 24, 2017
 *      Author: alax
 */

#ifndef CPUCOUNTER_H_
#define CPUCOUNTER_H_

#include <chrono>
#include "Plot.h"

class cpu_counter
{
public:
	cpu_counter();
	virtual ~cpu_counter();

	void start();
	void stop();

private:
	std::chrono::steady_clock::time_point start_time;
	Plot *plot;

};

#endif /* CPUCOUNTER_H_ */
