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

const size_t voices_count = 6;


#ifndef __arm__
const unsigned int councyrent_cores[3] = {3,6,7};
#else
const unsigned int councyrent_cores[3] = {1,2,3};
#endif


#endif /* SRC_CONFIG_H_ */
