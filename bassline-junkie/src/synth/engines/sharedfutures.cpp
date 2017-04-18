/*
 * sharedfutures.cpp
 *
 *  Created on: Apr 12, 2017
 *      Author: alax
 */

#include "sharedfutures.h"

shared_futures::shared_futures() {

}
shared_futures::~shared_futures() {

}


void shared_futures::process(std::array<synth, voices_count> &voices) {

	std::promise<void> t1_ready_promise;
	std::promise<void> t2_ready_promise;
	std::promise<void> ready_promise;

	std::shared_future<void> ready_future(ready_promise.get_future());

	auto fun1 = [&]()
	{
		stick_this_thread_to_core(councyrent_cores[1]);
		t1_ready_promise.set_value();
		ready_future.wait(); // waits for the signal from main()

			voices[0].process();
			voices[1].process();
		};

	auto fun2 = [&]()
	{
		stick_this_thread_to_core(councyrent_cores[2]);
		t2_ready_promise.set_value();
		ready_future.wait(); // waits for the signal from main()

			voices[2].process();
			voices[3].process();
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

