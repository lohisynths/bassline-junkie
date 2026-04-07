/*
 * config.h
 *
 *  Created on: Apr 19, 2017
 *      Author: alax
 */

#ifndef SRC_CONFIG_H_
#define SRC_CONFIG_H_

#include <signal.h>
#include <unistd.h>
#include <atomic>

extern std::atomic<sig_atomic_t> g_play;

inline void finish(int ignore)
{
	(void)ignore;
	static const char msg[] = "finish finish finish finish finish finish\n";
	const ssize_t ignored = write(STDERR_FILENO, msg, sizeof(msg) - 1);
	(void)ignored;
	g_play.store(0, std::memory_order_relaxed);
}

const size_t overall_voices_count = 8;

const size_t first_cpu=1;
const size_t max_cores=3;

const size_t format_bits = 32; //snd_pcm_format_width(*m_format);
const size_t buffer_size = 128;
const size_t sample_rate = 48000;


#endif /* SRC_CONFIG_H_ */
