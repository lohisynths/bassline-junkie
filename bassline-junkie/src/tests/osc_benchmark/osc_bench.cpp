#include <benchmark/benchmark.h>
#include <fstream>
//#define DUMP_RESULTS 1

#include "BlitSaw.h"
#include "Blit.h"
#include "BlitSquare.h"
#include "dsp/PolyBLEPOscillator/PolyBLEPOscillator.h"
#include "dsp/PolyBLEP/PolyBLEP.h"
#include "dsp/stmlib_polybleep.h"

#define ARR_SIZE 512

void BlitSaw_tick(benchmark::State& state)
{
    stk::Stk::setSampleRate(96000);
	stk::BlitSaw osc;
    double  output[ARR_SIZE];

	while (state.KeepRunning()) {
		for(int i=0; i<ARR_SIZE; i++) {
	        osc.setFrequency(state.range(0));
	        output[i] = osc.tick();
		}
	    benchmark::DoNotOptimize(output);
	}
#if DUMP_RESULTS
    std::string filename("stk_BlitSaw");
    filename += "_data_";
    filename += std::to_string(state.range(0));
    filename += ".bin";

    std::ofstream outfile_m(filename, std::ios::binary);
            outfile_m.write((const char*)output, sizeof(output));
#endif
}
BENCHMARK(BlitSaw_tick)->Arg(110)->Arg(440)->Arg(880);

void PolyBleepOsc_tick(benchmark::State& state)
{
    PolyBLEPOscillator osc;
    osc.setSampleRate(96000);
    osc.setMode(Oscillator::OSCILLATOR_MODE_SAW);
    double output[ARR_SIZE];

    while (state.KeepRunning()) {
        for(int i=0; i<ARR_SIZE; i++) {
            osc.setFrequency(state.range(0));
            output[i] = osc.nextSample();
        }
        benchmark::DoNotOptimize(output);
    }
#if DUMP_RESULTS
    std::string filename("PolyBLEPOscillator");
    filename += "_data_";
    filename += std::to_string(state.range(0));
    filename += ".bin";

    std::ofstream outfile_m(filename, std::ios::binary);
            outfile_m.write((const char*)output, sizeof(output));
#endif
}
BENCHMARK(PolyBleepOsc_tick)->Arg(110)->Arg(440)->Arg(880);

void StmPolyBleepOsc_tick(benchmark::State& state)
{
    stmlib::Oscillator osc;
    osc.Init();
    double output[ARR_SIZE];

    while (state.KeepRunning()) {
        for(int i=0; i<ARR_SIZE; i++) {
            float out;
            osc.Render<true>((state.range(0) / 96000.), 0.5f, 0.0f, &out, 1);
            output[i] = out;
        }
        benchmark::DoNotOptimize(output);
    }
#if DUMP_RESULTS
    std::string filename("stmlib_Oscillator");
    filename += "_data_";
    filename += std::to_string(state.range(0));
    filename += ".bin";

    std::ofstream outfile_m(filename, std::ios::binary);
            outfile_m.write((const char*)output, sizeof(output));
#endif
}
BENCHMARK(StmPolyBleepOsc_tick)->Arg(110)->Arg(440)->Arg(880);

void PolyBleep_tick(benchmark::State& state)
{
    static PolyBLEP osc(96000, PolyBLEP::SAWTOOTH, 440);
    double output[ARR_SIZE];

    while (state.KeepRunning()) {
        for(int i=0; i<ARR_SIZE; i++) {
            osc.setFrequency(state.range(0));
            output[i] = osc.getAndInc();
            benchmark::DoNotOptimize(output[i]);
        }
    }
#if DUMP_RESULTS
    std::string filename("PolyBLEP");
    filename += "_data_";
    filename += std::to_string(state.range(0));
    filename += ".bin";

    std::ofstream outfile_m(filename, std::ios::binary);
            outfile_m.write((const char*)output, sizeof(output));
#endif
}
BENCHMARK(PolyBleep_tick)->Arg(110)->Arg(440)->Arg(880);


BENCHMARK_MAIN();
