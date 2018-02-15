/*
 * cpucounter.h
 *
 *  Created on: Mar 24, 2017
 *      Author: alax
 */

#ifndef CPUCOUNTER_H_
#define CPUCOUNTER_H_

#include <chrono>
#include <vector>
#include <fstream>

class cpu_counter
{
public:
	cpu_counter();
	virtual ~cpu_counter();
	bool update();

	void start();
	std::chrono::duration<double, std::milli> stop();

private:
	std::chrono::steady_clock::time_point start_time;

	unsigned int i = 0;

	std::vector<double> times;

	const unsigned int start_threshold = 100;
	const unsigned int iter = 1000 + start_threshold;
};

#endif /* CPUCOUNTER_H_ */
