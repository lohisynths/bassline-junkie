/*
 * lelok.h
 *
 *  Created on: Mar 7, 2017
 *      Author: alax
 */
// http://stackoverflow.com/questions/14718124/how-to-easily-make-stdcout-thread-safe
#ifndef CONCURENCY_HELPERS_H_
#define CONCURENCY_HELPERS_H_
#include <sstream>      // std::ostringstream

/** Thread safe cout class
 * Exemple of use:
 *    PrintThread{} << "Hello world!" << std::endl;
 */
//class PrintThread: public std::ostringstream
//{
//public:
//	PrintThread() = default;
//
//	~PrintThread()
//	{
//		std::lock_guard<std::mutex> guard(_mutexPrint);
//		std::cout << this->str();
//	}
//
//private:
//	static std::mutex _mutexPrint;
//};
//
//std::mutex PrintThread::_mutexPrint
//{ };
//////////////////////////////////////////////////////////////////////////////////
#include <unistd.h> //_SC_NPROCESSORS_ONLN

#include <iostream>

inline int stick_this_thread_to_core(int core_id)
{
	int num_cores = sysconf(_SC_NPROCESSORS_ONLN);
	if (core_id < 0 || core_id >= num_cores)
		return EINVAL;

	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(core_id, &cpuset);

	pthread_t current_thread = pthread_self();
	return pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset);
}

//////////////////////////////////////////////////////////////////////////////////

inline void set_pthread_params()
{
	int policy = SCHED_FIFO;
	int priority = sched_get_priority_max(policy);

	sched_param sch_params;
	sch_params.sched_priority = priority;
	if (pthread_setschedparam(pthread_self(), policy, &sch_params))
	{
		std::cout << "Failed to set Thread scheduling" << std::endl;
	}
}

//////////////////////////////////////////////////////////////////////////////////

inline void getinfo()
{
	struct sched_param param;
	int policy;

	sched_getparam(0, &param);
	std::cout << "Priority of this process: " << param.sched_priority
			<< std::endl;

	pthread_getschedparam(pthread_self(), &policy, &param);

	std::cout << "Priority of the thread:" << param.sched_priority
			<< " current policy is: " << policy << "\n";
}

#endif /* CONCURENCY_HELPERS_H_ */
