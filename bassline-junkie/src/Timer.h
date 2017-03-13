/*
 * Timer.h
 *
 *  Created on: Feb 8, 2017
 *      Author: alax
 */

#ifndef TIMER_H_
#define TIMER_H_
#include <chrono>

class Timer
{
public:
	Timer();
	virtual ~Timer();
	void restart();
	std::chrono::steady_clock::duration get_difference();

	std::chrono::steady_clock::time_point start;

};

#endif /* TIMER_H_ */
