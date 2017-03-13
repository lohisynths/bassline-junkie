/*
 * PerformanceCounter.cpp
 *
 *  Created on: Feb 14, 2017
 *      Author: alax
 */

#include "PerformanceCounter.h"
#include <iostream>

#include <fstream>
std::ofstream myfile;

PerformanceCounter::PerformanceCounter()
{
	 myfile.open ("example.txt");
	 from_begining.restart();
}

PerformanceCounter::~PerformanceCounter()
{
	// TODO Auto-generated destructor stub
}

void PerformanceCounter::trigger()
{
	overal_diff = overal.get_difference();
	overal.restart();
	load.restart();
}



void PerformanceCounter::read()
{
	auto load_time = load.get_difference();
	static int ii = 0;
	ii++;
	if (ii == 1)
	{
		auto from_start =
					std::chrono::duration_cast<std::chrono::microseconds>(from_begining.get_difference()).count();

		ii = 0;
		auto overal_time_ms = std::chrono::duration_cast<
				std::chrono::microseconds>(overal_diff).count();
		auto load_time_ms =
				std::chrono::duration_cast<std::chrono::microseconds>(load_time).count();

		auto cpu_load = (float) load_time_ms / (float) overal_time_ms * 100.;

//		std::cout << std::endl;
//		std::cout << std::setfill('-') << std::setw(44) << "-"
//				<< std::setfill(' ') << std::endl;
//
//		std::cout << std::setw(15) << std::left << "overal time";
//		std::cout << std::setw(15) << std::left << "load time";
//		std::cout << "CPU load";
//		std::cout << std::endl;
//
//		std::cout << std::setw(15) << std::left << overal_time_ms;
//		std::cout << std::setw(15) << std::left << load_time_ms;
//		std::cout << std::setprecision(4) << cpu_load << "%";
//		std::cout << std::endl;


		myfile << from_start << " " << overal_time_ms << " " << cpu_load <<  std::endl;


		auto overal_time_ms2 = std::chrono::duration_cast<
				std::chrono::milliseconds>(overal_diff).count();
//		std::cout << overal_time_ms2 << std::endl;
	}
}
