/*
 * ConditionalVar.h
 *
 *  Created on: May 12, 2017
 *      Author: alax
 */

#ifndef SRC_SYNTH_ENGINES_CONDITIONALVAR_H_
#define SRC_SYNTH_ENGINES_CONDITIONALVAR_H_

#include <array>
#include <thread>
#include <condition_variable>
#include "../voice.h"
#include "../config.h"
#include "../utils/concurency_helpers.h"

class ConditionalVar
{
public:
	ConditionalVar(std::array<Voice, voices_count> &voices, const uint8_t cpu,
			const uint8_t voice_first, const uint8_t voices_count) :
			ready(false), processed(false), t(0), m(0), cv(0), m_voices(voices), m_voice_first(
					voice_first), m_voices_count(voices_count), m_cpu(cpu)
	{
		m = new std::mutex;
		cv = new std::condition_variable;
		t = new std::thread([this]
		{	return worker_thread();});
	}

	virtual ~ConditionalVar()
	{
	}
	;

	void worker_thread()
	{
		std::cout << __PRETTY_FUNCTION__ << std::endl;
		stick_this_thread_to_core(m_cpu);
		// Wait until main() sends data
		while (1)
		{
			std::unique_lock<std::mutex> lk(*m);

			cv->wait(lk, [this]
			{	return ready;});

			for (int i = m_voice_first; i < m_voice_first+m_voices_count; i++)
			{
				m_voices[i].process();
			}

			// Send data back to main()
			processed = true;
			ready = false;

			// Manual unlocking is done before notifying, to avoid waking up
			// the waiting thread only to block again (see notify_one for details)
			lk.unlock();
			cv->notify_one();
		}
	}
#include <chrono>

	void request()
	{
		std::lock_guard<std::mutex> lk(*m);
		ready = true;
		processed = false;
		cv->notify_one();
	}
	void wait()
	{
		std::unique_lock<std::mutex> lk(*m);
		cv->wait(lk, [this]
		{	return processed;});
	}

private:
	bool ready;
	bool processed;
	std::thread* t;
	std::mutex* m;
	std::condition_variable* cv;

	std::array<Voice, voices_count> &m_voices;

	uint8_t m_voice_first;
	uint8_t m_voices_count;
	uint8_t m_cpu;

};

#endif /* SRC_SYNTH_ENGINES_CONDITIONALVAR_H_ */
