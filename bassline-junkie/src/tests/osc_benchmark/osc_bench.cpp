#include <benchmark/benchmark.h>

#include "BlitSaw.h"
#include "Blit.h"
#include "BlitSquare.h"

void BlitSaw_tick(benchmark::State& state)
{
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

void Blit_tick(benchmark::State& state)
{
	stk::Blit osc;
	while (state.KeepRunning())
	{
		osc.setFrequency(state.range(0));
		benchmark::DoNotOptimize(osc.tick());
	}
}
//BENCHMARK(Blit_tick)->Arg(110)->Arg(440)->Arg(880);

void BlitSquare_tick(benchmark::State& state)
{
	stk::BlitSquare osc;
	while (state.KeepRunning())
	{
		osc.setFrequency(state.range(0));
		benchmark::DoNotOptimize(osc.tick());
	}
}

//BENCHMARK(BlitSquare_tick)->Arg(110)->Arg(440)->Arg(880);


BENCHMARK_MAIN();
