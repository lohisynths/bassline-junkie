/*
 * config.h
 *
 *  Created on: Apr 19, 2017
 *      Author: alax
 */

#ifndef SRC_CONFIG_H_
#define SRC_CONFIG_H_

#ifndef __arm__
const unsigned int councyrent_cores[3] = {3,6,7};
#else
const unsigned int councyrent_cores[3] = {1,2,3};
#endif


#endif /* SRC_CONFIG_H_ */
