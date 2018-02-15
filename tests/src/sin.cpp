#include <benchmark/benchmark.h>
#include <array>
#include <iostream>
#include <fstream>
#include "concurency_helpers.h"

// http://lolengine.net/blog/2011/12/21/better-function-approximations
template<typename T>
T taylorsin(T x)
{
    static const
	T a0 =  1.0,
           a1 = -1.666666666666666666666666666666e-1,  /* -1/3! */
           a2 =  8.333333333333333333333333333333e-3,  /*  1/5! */
           a3 = -1.984126984126984126984126984126e-4,  /* -1/7! */
           a4 =  2.755731922398589065255731922398e-6,  /*  1/9! */
           a5 = -2.505210838544171877505210838544e-8,  /* -1/11! */
           a6 =  1.605904383682161459939237717015e-10; /*  1/13! */
    T x2 = x * x;
    return x * (a0 + x2 * (a1 + x2 * (a2 + x2
             * (a3 + x2 * (a4 + x2 * (a5 + x2 * a6))))));
}

// https://stackoverflow.com/a/6091846
template<typename T>
T fastSin(T x ){
    x = fmod(x + M_PI, M_PI * 2) - M_PI; // restrict x so that -M_PI < x < M_PI
    const T B = 4.0f/M_PI;
    const T C = -4.0f/(M_PI*M_PI);

    float y = B * x + C * x * std::abs(x);

    const T P = 0.225f;

    return P * (y * std::abs(y) - y) + y;
}

// https://www.gamedev.net/forums/topic/621589-extremely-fast-sin-approximation/
double fast_sin(double x) {
	int k;
	double y;
	double z;
	z  = x;
	z *= 0.3183098861837907;
	z += 6755399441055744.0;
	k  = *((int *) &z);
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

// https://www.gamedev.net/forums/topic/621589-extremely-fast-sin-approximation/
// on raspi have to define NO_FAST_TRUNCATE
#define NO_FAST_TRUNCATE
int32_t fast_round(double x) {
#ifndef NO_FAST_TRUNCATE
	const double MAGIC_ROUND = 6755399441055744.0; /* http://stereopsis.com/sree/fpu2006.html */
	union {
		double d;
		struct {
#ifdef BIG_ENDIAN
			int32_t hw;
			int32_t lw;
#else // BIG_ENDIAN
			int32_t lw;
			int32_t hw;
#endif
		};
	} fast_trunc;
	fast_trunc.d = x;
	fast_trunc.d += MAGIC_ROUND;
	return fast_trunc.lw;
#else //
	if (x < 0) {
		return (int32_t) (x - 0.5);
	} else {
		return (int32_t) (x + 0.5);
	}
#endif // NO_FAST_TRUNCATE
}

double fast_sin_long(double x) {
	const double PI = 3.14159265358979323846264338327950288;
	const double INVPI = 0.31830988618379067153776752674502872;
	const double A = 0.00735246819687011731341356165096815;
	const double B = -0.16528911397014738207016302002888890;
	const double C = 0.99969198629596757779830113868360584;
	int32_t k;
	double x2;
	/* find offset of x from the range -pi/2 to pi/2 */
	k = fast_round(INVPI * x);
	/* bring x into range */
	x -= k * PI;
	/* calculate sine */
	x2 = x * x;
	x = x * (C + x2 * (B + A * x2));
	/* if x is in an odd pi count we must flip */
	if (k % 2)
		x = -x;
	return x;
}

template<typename T> T sin_std(T a) { return std::sin(a); }


template<typename T, typename OP>
void sine_test(OP op, benchmark::State& state, std::string func) {

	set_pthread_params();
	stick_this_thread_to_core(1);
	T * output = new T[state.range(0)];
	const T one_over_length = 1. / (T)state.range(0);

	for (auto _ : state) {
		for(int i=0; i<state.range(0); i++) {
			T index = i*one_over_length*M_PI*2;
			output[i] = op(index);
		}
		benchmark::DoNotOptimize(output);
	}
	std::string filename(func);
	filename += "_data_";
	filename += std::to_string(state.range(0));
	filename += ".bin";

	std::ofstream outfile_m(filename, std::ios::binary);
			outfile_m.write((const char*)output, state.range(0)*sizeof(*output));

	delete [] output;
}

void BM_math_sin_std_float(benchmark::State& state){
	sine_test<float>(sin_std<float>, state, __func__);
}
void BM_math_sin_std_double(benchmark::State& state){
	sine_test<double>(sin_std<double>, state, __func__);
}
void BM_math_sin_taylor_float(benchmark::State& state){
	sine_test<float>(taylorsin<float>, state, __func__);
}
void BM_math_sin_taylor_double(benchmark::State& state){
	sine_test<double>(taylorsin<double>, state, __func__);
}
void BM_math_sin_fastsin_float(benchmark::State& state){
	sine_test<float>(fastSin<float>, state, __func__);
}
void BM_math_sin_fastsin_double(benchmark::State& state){
	sine_test<double>(fastSin<double>, state, __func__);
}

void BM_math_sin_fastsin2_double(benchmark::State& state){
	sine_test<double>(fast_sin, state, __func__);
}

void BM_math_sin_fastsinlong_double(benchmark::State& state){
	sine_test<double>(fast_sin_long, state, __func__);
}


#define LENGTH 128

//BENCHMARK(BM_math_sin_std_float)->Arg(LENGTH);
BENCHMARK(BM_math_sin_std_double)->Arg(LENGTH);

//BENCHMARK(BM_math_sin_taylor_float)->Arg(LENGTH);
BENCHMARK(BM_math_sin_taylor_double)->Arg(LENGTH);

//BENCHMARK(BM_math_sin_fastsin_float)->Arg(LENGTH);
BENCHMARK(BM_math_sin_fastsin_double)->Arg(LENGTH);

BENCHMARK(BM_math_sin_fastsin2_double)->Arg(LENGTH);

BENCHMARK(BM_math_sin_fastsinlong_double)->Arg(LENGTH);






