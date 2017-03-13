/*
 * Timer.cpp
 *
 *  Created on: Feb 8, 2017
 *      Author: alax
 */

#include "Timer.h"

Timer::Timer()
{
	start = std::chrono::steady_clock::now();
}

std::chrono::steady_clock::duration Timer::get_difference()
{
	return std::chrono::steady_clock::now() - start;
}

void Timer::restart()
{
	start = std::chrono::steady_clock::now();
}

Timer::~Timer()
{
}
