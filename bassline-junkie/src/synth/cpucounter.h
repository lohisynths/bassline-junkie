/*
 * cpucounter.h
 *
 *  Created on: Mar 24, 2017
 *      Author: alax
 */

#ifndef CPUCOUNTER_H_
#define CPUCOUNTER_H_

#include <chrono>

class cpu_counter
{
public:
	cpu_counter();
	virtual ~cpu_counter();

	void start();
	std::chrono::duration<double, std::milli> stop();

private:
	std::chrono::steady_clock::time_point start_time;

};

#endif /* CPUCOUNTER_H_ */
