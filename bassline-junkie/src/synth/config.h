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

static volatile sig_atomic_t g_play = 1;

static void finish(int ignore)
{
	(void)ignore;
	static const char msg[] = "finish finish finish finish finish finish\n";
	write(STDERR_FILENO, msg, sizeof(msg) - 1);
	g_play = 0;
	_exit(0);
}

const size_t overall_voices_count = 8;

const size_t first_cpu=1;
const size_t max_cores=3;

const size_t format_bits = 32; //snd_pcm_format_width(*m_format);
const size_t buffer_size = 512;


#endif /* SRC_CONFIG_H_ */
