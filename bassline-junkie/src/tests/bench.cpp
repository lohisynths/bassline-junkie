#include <benchmark/benchmark_api.h>

#include "BlitSaw.h"
#include "Blit.h"
#include "BlitSquare.h"

void BlitSaw_tick(benchmark::State& state)
{
	stk::BlitSaw osc;
	while (state.KeepRunning())
	{
		osc.setFrequency(state.range(0));
		benchmark::DoNotOptimize(osc.tick());
	}
	  state.SetComplexityN(state.range(0));
}
BENCHMARK (BlitSaw_tick)->Arg(110)->Arg(440)->Arg(880);

void Blit_tick(benchmark::State& state)
{
	stk::Blit osc;
	while (state.KeepRunning())
	{
		osc.setFrequency(state.range(0));
		benchmark::DoNotOptimize(osc.tick());
	}
}
BENCHMARK (Blit_tick)->Arg(110)->Arg(440)->Arg(880);

void BlitSquare_tick(benchmark::State& state)
{
	stk::BlitSquare osc;
	while (state.KeepRunning())
	{
		osc.setFrequency(state.range(0));
		benchmark::DoNotOptimize(osc.tick());
	}
}

BENCHMARK (BlitSquare_tick)->Arg(110)->Arg(440)->Arg(880);


BENCHMARK_MAIN()

