/*
 * Logger.h
 *
 *  Created on: Sep 27, 2017
 *      Author: alax
 */

#ifndef SRC_SYNTH_UTILS_LOGGER_H_
#define SRC_SYNTH_UTILS_LOGGER_H_


#define LOGGER

#ifdef LOGGER
#define LOG_DEBUG(out,a) \
	out->debug("{}() {}", __FUNCTION__, a)
#define LOG_INFO(out,a) \
	out->info("{}() {}", __FUNCTION__, a)
#define LOG_ERROR(out,a) \
	out->error("{}() {}", __FUNCTION__, a)
#define LOG_WARN(out,a) \
	out->warn("{}() {}", __FUNCTION__, a)
#else
#define LOG_DEBUG(out,a) \
		void()
#define LOG_INFO(out,a) \
		void()
#define LOG_ERROR(out,a) \
		void()
#define LOG_WARN(out,a) \
		void()
#endif

#endif /* SRC_SYNTH_UTILS_LOGGER_H_ */
