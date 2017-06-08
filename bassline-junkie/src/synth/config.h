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

const size_t voices_count = 8;



#endif /* SRC_CONFIG_H_ */
