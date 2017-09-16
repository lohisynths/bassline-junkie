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
const size_t max_cores=3;
const size_t max_voices_per_core=3;

#endif /* SRC_CONFIG_H_ */
