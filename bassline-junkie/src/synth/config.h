/*
 * config.h
 *
 *  Created on: Apr 19, 2017
 *      Author: alax
 */

#ifndef SRC_CONFIG_H_
#define SRC_CONFIG_H_


static bool play = true;

static void finish(int ignore)
{
	printf("finish finish finish finish finish finish\n");
	play = false;
}

const size_t overall_voices_count = 8;

const size_t first_cpu=1;
const size_t max_cores=3;
const size_t max_voices_per_core=3;

const size_t format_bits = 32; //snd_pcm_format_width(*m_format);
const size_t buffer_size = 32;


#endif /* SRC_CONFIG_H_ */
