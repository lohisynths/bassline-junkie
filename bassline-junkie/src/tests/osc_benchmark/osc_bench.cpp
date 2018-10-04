#include <benchmark/benchmark.h>

#include "BlitSaw.h"
#include "Blit.h"
#include "BlitSquare.h"
#include "dsp/PolyBLEPOscillator/PolyBLEPOscillator.h"
#include "dsp/PolyBLEP/PolyBLEP.h"
#include "dsp/stmlib_polybleep.h"

void BlitSaw_tick(benchmark::State& state)
{
    stk::Stk::setSampleRate(96000);
	stk::BlitSaw osc;
    stk::StkFloat  output[512];

	while (state.KeepRunning()) {
		for(int i=0; i<512; i++) {
	        osc.setFrequency(state.range(0));
	        output[i] = osc.tick();
		}
	    benchmark::DoNotOptimize(output);
	}
}
BENCHMARK(BlitSaw_tick)->Arg(110)->Arg(440)->Arg(880);

void PolyBleepOsc_tick(benchmark::State& state)
{
    PolyBLEPOscillator osc;
    osc.setSampleRate(96000);
    osc.setMode(Oscillator::OSCILLATOR_MODE_SAW);
    stk::StkFloat output[512];

    while (state.KeepRunning()) {
        for(int i=0; i<512; i++) {
            osc.setFrequency(state.range(0));
            output[i] = osc.nextSample();
        }
        benchmark::DoNotOptimize(output);
    }
}
BENCHMARK(PolyBleepOsc_tick)->Arg(110)->Arg(440)->Arg(880);

void StmPolyBleepOsc_tick(benchmark::State& state)
{
    stmlib::Oscillator osc;
    osc.Init();
    stk::StkFloat output[512];

    while (state.KeepRunning()) {
        for(int i=0; i<512; i++) {
            float out;
            osc.Render<true>((state.range(0) / 96000.), (state.range(0) / 2. / 96000.), 0.5f, 0.0f, &out, 1);
            output[i] = out;
        }
        benchmark::DoNotOptimize(output);
    }
}
BENCHMARK(StmPolyBleepOsc_tick)->Arg(110)->Arg(440)->Arg(880);

void PolyBleep_tick(benchmark::State& state)
{
    static PolyBLEP osc(96000, PolyBLEP::SAWTOOTH, 440);
    stk::StkFloat output[512];

    while (state.KeepRunning()) {
        for(int i=0; i<512; i++) {
            osc.setFrequency(state.range(0));
            output[i] = osc.getAndInc();
            benchmark::DoNotOptimize(output[i]);
        }
    }
}
BENCHMARK(PolyBleep_tick)->Arg(110)->Arg(440)->Arg(880);


BENCHMARK_MAIN();
