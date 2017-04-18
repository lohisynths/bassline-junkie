/*
 * sharedfutures.cpp
 *
 *  Created on: Apr 12, 2017
 *      Author: alax
 */

#include "sharedfutures_sleep.h"

shared_futures_sleep::shared_futures_sleep() {

}
shared_futures_sleep::~shared_futures_sleep() {

}
#include <chrono>

std::vector<float> data;


void shared_futures_sleep::process(std::array<synth, voices_count> &voices) {


	std::promise<void> t1_ready_promise;
	std::promise<void> t2_ready_promise;
	std::promise<void> ready_promise;

	std::shared_future<void> ready_future(ready_promise.get_future());

	auto fun1 = [&]()
	{
		stick_this_thread_to_core(6);
		t1_ready_promise.set_value();
		ready_future.wait(); // waits for the signal from main()

		    using namespace std::chrono_literals;
		    std::this_thread::sleep_for(8ms);
		};

	auto fun2 = [&]()
	{
		stick_this_thread_to_core(7);
		t2_ready_promise.set_value();
		ready_future.wait(); // waits for the signal from main()

		   using namespace std::chrono_literals;
		   std::this_thread::sleep_for(8ms);
		};

	auto result1 = std::async(std::launch::async, fun1);
	auto result2 = std::async(std::launch::async, fun2);

	// wait for the threads to become ready
	t1_ready_promise.get_future().wait();
	t2_ready_promise.get_future().wait();
	// signal the threads to go
	ready_promise.set_value();

	result1.get();
	result2.get();


}

