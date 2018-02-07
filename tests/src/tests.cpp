#include <benchmark/benchmark.h>
#include <array>
#include <iostream>
#include <fstream>

typedef double StkFloat;

inline StkFloat fast_sin(StkFloat x);

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


static void BM_StkFramesTick_Frames(benchmark::State& state) {
	set_pthread_params();
	StkFloat * output = new StkFloat[state.range(0)];
	const StkFloat one_over_length = 1. / (StkFloat)state.range(0);

	for (auto _ : state)
	{
		for(int i=0; i<state.range(0); i++) {
			//std::cout << i*one_over_length*M_PI*2 << std::endl;
			output[i] = fast_sin(i*one_over_length*M_PI*2);
		}
		benchmark::DoNotOptimize(output);
	}

	std::string filename("data_");
	filename += std::to_string(state.range(0));
	filename += ".bin";

	std::ofstream outfile_m(filename, std::ios::binary);
			outfile_m.write((const char*)output, state.range(0)*sizeof(*output));


	delete [] output;
}

BENCHMARK(BM_StkFramesTick_Frames)->Arg(100*441000);//->Arg(256)->Arg(512)->Arg(1024*16);
BENCHMARK_MAIN();



union el
{
	StkFloat z1;
	uint32_t z2;
};

StkFloat fast_sin(StkFloat x) {
	uint32_t k;
	StkFloat y;
	StkFloat z;

	el zz;

	z  = x;
	z *= 0.3183098861837907;
	z += 6755399441055744.0;

	zz.z1 = z;

	//k  = *((int *) &z);
	k = zz.z2;

	z  = k;
	z *= 3.1415926535897932;
	x -= z;
	y  = x;
	y *= x;
	z  = 0.0073524681968701;
	z *= y;
	z -= 0.1652891139701474;
	z *= y;
	z += 0.9996919862959676;
	x *= z;
	k &= 1;
	k += k;
	z  = k;
	z *= x;
	x -= z;

	return x;
}
