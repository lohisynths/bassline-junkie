#include <benchmark/benchmark.h>
#include <fstream>
#include <string>
//#define DUMP_RESULTS 1

#include "dsp/BlitSaw.h"
#include "dsp/MinBLEPOscillator.h"
#include "dsp/PolyBLEPOscillator/PolyBLEPOscillator.h"
#include "dsp/PolyBLEP/PolyBLEP.h"
#include "dsp/WavetableOscillator.h"
#include "dsp/stmlib_polybleep.h"

#define ARR_SIZE 512

void BlitSaw_tick(benchmark::State& state)
{
	bassline::dsp::BlitSaw osc;
    osc.setSampleRate(96000);
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

void MinBLEP_saw_tick(benchmark::State& state)
{
    bassline::dsp::MinBLEPOscillator osc;
    osc.setSampleRate(96000);
    double output[ARR_SIZE];

    while (state.KeepRunning()) {
        for(int i=0; i<ARR_SIZE; i++) {
            osc.setFrequency(state.range(0));
            output[i] = osc.tick();
        }
        benchmark::DoNotOptimize(output);
    }
#if DUMP_RESULTS
    std::string filename("MinBLEP");
    filename += "_data_";
    filename += std::to_string(state.range(0));
    filename += ".bin";

    std::ofstream outfile_m(filename, std::ios::binary);
            outfile_m.write((const char*)output, sizeof(output));
#endif
}
BENCHMARK(MinBLEP_saw_tick)->Arg(110)->Arg(440)->Arg(880);

void MinBLEP_square_tick(benchmark::State& state)
{
    bassline::dsp::MinBLEPOscillator osc;
    osc.setSampleRate(96000);
    osc.setWaveform(bassline::dsp::MinBLEPOscillator::SQUARE);
    osc.setPulseWidth(0.5);
    double output[ARR_SIZE];

    while (state.KeepRunning()) {
        for(int i=0; i<ARR_SIZE; i++) {
            osc.setFrequency(state.range(0));
            output[i] = osc.tick();
        }
        benchmark::DoNotOptimize(output);
    }
#if DUMP_RESULTS
    std::string filename("MinBLEP_square");
    filename += "_data_";
    filename += std::to_string(state.range(0));
    filename += ".bin";

    std::ofstream outfile_m(filename, std::ios::binary);
            outfile_m.write((const char*)output, sizeof(output));
#endif
}
BENCHMARK(MinBLEP_square_tick)->Arg(110)->Arg(440)->Arg(880);

void WavetableSine_tick(benchmark::State& state)
{
    bassline::dsp::WavetableOscillator osc;
    osc.setSampleRate(96000);
    osc.setWaveform(bassline::dsp::WavetableOscillator::SINE);
    double output[ARR_SIZE];

    while (state.KeepRunning()) {
        for(int i=0; i<ARR_SIZE; i++) {
            osc.setFrequency(state.range(0));
            output[i] = osc.tick();
        }
        benchmark::DoNotOptimize(output);
    }
}
BENCHMARK(WavetableSine_tick)->Arg(110)->Arg(440)->Arg(880);

void WavetableSaw_tick(benchmark::State& state)
{
    bassline::dsp::WavetableOscillator osc;
    osc.setSampleRate(96000);
    osc.setWaveform(bassline::dsp::WavetableOscillator::SAW);
    double output[ARR_SIZE];

    while (state.KeepRunning()) {
        for(int i=0; i<ARR_SIZE; i++) {
            osc.setFrequency(state.range(0));
            output[i] = osc.tick();
        }
        benchmark::DoNotOptimize(output);
    }
}
BENCHMARK(WavetableSaw_tick)->Arg(110)->Arg(440)->Arg(880);

void WavetableTriangle_tick(benchmark::State& state)
{
    bassline::dsp::WavetableOscillator osc;
    osc.setSampleRate(96000);
    osc.setWaveform(bassline::dsp::WavetableOscillator::TRIANGLE);
    double output[ARR_SIZE];

    while (state.KeepRunning()) {
        for(int i=0; i<ARR_SIZE; i++) {
            osc.setFrequency(state.range(0));
            output[i] = osc.tick();
        }
        benchmark::DoNotOptimize(output);
    }
}
BENCHMARK(WavetableTriangle_tick)->Arg(110)->Arg(440)->Arg(880);

void WavetableSquare_tick(benchmark::State& state)
{
    bassline::dsp::WavetableOscillator osc;
    osc.setSampleRate(96000);
    osc.setWaveform(bassline::dsp::WavetableOscillator::SQUARE);
    double output[ARR_SIZE];

    while (state.KeepRunning()) {
        for(int i=0; i<ARR_SIZE; i++) {
            osc.setFrequency(state.range(0));
            output[i] = osc.tick();
        }
        benchmark::DoNotOptimize(output);
    }
}
BENCHMARK(WavetableSquare_tick)->Arg(110)->Arg(440)->Arg(880);


BENCHMARK_MAIN();
