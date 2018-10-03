#include <benchmark/benchmark.h>
#include <fstream>
#include <cmath>

#include "impl/fast_sin.h"
#include "impl/fast_sin_w_round.h"
#include "impl/fastSin.h"
#include "impl/sin_taylor.h"
#include "impl/std_sin.h"

const int LENGTH = 512;
const int MIN_TIME = 1;


template<typename T, typename OP>
void sine_test(OP op, benchmark::State& state, std::string func) {
    T * output = new T[state.range(0)];
    const T one_over_length = 1. / ((T)state.range(0) / 2);
    T _PI = M_PI;

    for (auto _ : state) {
        for(int i=0; i<state.range(0); i++) {
            T index = i*one_over_length*_PI*(T)(2);
            output[i] = op(index);
        }
        benchmark::DoNotOptimize(output);
    }
#if !DUMP_RESULTS
    std::string filename(func);
    filename += "_data_";
    filename += std::to_string(state.range(0));
    filename += ".bin";

    std::ofstream outfile_m(filename, std::ios::binary);
            outfile_m.write((const char*)output, state.range(0)*sizeof(*output));
#endif
    delete [] output;
}

void sin_std_float(benchmark::State& state){
    sine_test<float>(sin_std<float>, state, __func__);
}
void sin_std_double(benchmark::State& state){
    sine_test<double>(sin_std<double>, state, __func__);
}
void sin_taylor_float(benchmark::State& state){
    sine_test<float>(sin_taylor<float>, state, __func__);
}
void sin_taylor_double(benchmark::State& state){
    sine_test<double>(sin_taylor<double>, state, __func__);
}
void fastSin_float(benchmark::State& state){
    sine_test<float>(fastSin<float>, state, __func__);
}
void fastSin_double(benchmark::State& state){
    sine_test<double>(fastSin<double>, state, __func__);
}
void fast_sin_double(benchmark::State& state){
    sine_test<double>(fast_sin, state, __func__);
}
void fast_sin_w_round_double(benchmark::State& state){
    sine_test<double>(fast_sin_w_round<double, uint32_t>, state, __func__);
}
void fast_sin_w_round_float(benchmark::State& state){
    sine_test<float>(fast_sin_w_round<float, uint16_t>, state, __func__);
}

BENCHMARK(sin_std_float)->Arg(LENGTH)->MinTime(MIN_TIME);
BENCHMARK(sin_std_double)->Arg(LENGTH)->MinTime(MIN_TIME);

BENCHMARK(sin_taylor_float)->Arg(LENGTH)->MinTime(MIN_TIME);
BENCHMARK(sin_taylor_double)->Arg(LENGTH)->MinTime(MIN_TIME);

BENCHMARK(fastSin_float)->Arg(LENGTH)->MinTime(MIN_TIME);
BENCHMARK(fastSin_double)->Arg(LENGTH)->MinTime(MIN_TIME);

BENCHMARK(fast_sin_w_round_float)->Arg(LENGTH)->MinTime(MIN_TIME);
BENCHMARK(fast_sin_w_round_double)->Arg(LENGTH)->MinTime(MIN_TIME);

BENCHMARK(fast_sin_double)->Arg(LENGTH)->MinTime(MIN_TIME);


BENCHMARK_MAIN();
