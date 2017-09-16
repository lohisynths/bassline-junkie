/*
 * thread.h
 *
 *  Created on: May 12, 2017
 *      Author: alax
 */

#ifndef SRC_SYNTH_ENGINES_THREAD_H_
#define SRC_SYNTH_ENGINES_THREAD_H_

#include <vector>
#include <thread>
#include <condition_variable>
#include "../voice.h"
#include "../config.h"
#include "../utils/concurency_helpers.h"

template<size_t buffer_size>
class thread
{
public:
	thread() :
			ready(false), processed(false), t(0), m_cpu(0)
	{}

	void start()
	{
		t = new std::thread([this]
		{	return worker_thread();});
	}

	virtual ~thread()
	{};

	void set_cpu_affinity(uint8_t cpu)
	{
		m_cpu=cpu;
	}
	void worker_thread()
	{
		std::cout << __PRETTY_FUNCTION__ << " on cpu " << +m_cpu << std::endl;
		stick_this_thread_to_core(m_cpu);
		// Wait until main() sends data
		while (1)
		{
			std::unique_lock<std::mutex> lk(m);

			cv.wait(lk, [this]
			{	return ready;});

			for (auto &voice : m_voices)
			{
				voice->process();
			}

			// Send data back to main()
			processed = true;
			ready = false;

			// Manual unlocking is done before notifying, to avoid waking up
			// the waiting thread only to block again (see notify_one for details)
			lk.unlock();
			cv.notify_one();
		}
	}

	void request()
	{
		std::lock_guard<std::mutex> lk(m);
		ready = true;
		processed = false;
		cv.notify_one();
	}
	void wait()
	{
		std::unique_lock<std::mutex> lk(m);
		cv.wait(lk, [this]
		{	return processed;});
	}

	void add_voice(Voice<buffer_size>* voice)
	{
		m_voices.push_back(voice);
	}

private:
	bool ready;
	bool processed;
	std::thread* t;
	std::mutex m;
	std::condition_variable cv;

	std::vector<Voice<buffer_size>*> m_voices;

	uint8_t m_cpu;

};

#endif /* SRC_SYNTH_ENGINES_THREAD_H_ */
