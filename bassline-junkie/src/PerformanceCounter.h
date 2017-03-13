/*
 * PerformanceCounter.h
 *
 *  Created on: Feb 14, 2017
 *      Author: alax
 */

#ifndef SRC_PERFORMANCECOUNTER_H_
#define SRC_PERFORMANCECOUNTER_H_

#include "Timer.h"
#include <chrono>
#include <iostream>
#include <iomanip>

class PerformanceCounter
{
public:
	PerformanceCounter();
	virtual ~PerformanceCounter();

	void trigger();
	void read();

private:
	Timer overal, load, from_begining;
	std::chrono::steady_clock::duration overal_diff;

};

#endif /* SRC_PERFORMANCECOUNTER_H_ */
