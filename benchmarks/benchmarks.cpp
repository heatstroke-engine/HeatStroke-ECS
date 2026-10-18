#include "benchmarks.hpp"

#include <benchmark/benchmark.h>

static void BM_CREATEENTITES(benchmark::State& state)
{
    const int64_t  n = state.range(0);
    std::optional<GameEngine> GE;
    for (auto _ : state)
    {
        state.PauseTiming();
        GE.emplace();          // GameEngine Creating time is not interesting
        state.ResumeTiming();
        for(int64_t i{}; i < n; ++i)
        {
            benchmark::DoNotOptimize(GE->createEntity());
        }
    }
    state.SetItemsProcessed(state.iterations() * state.range(0));
}
BENCHMARK(BM_CREATEENTITES)->Arg(1000)->Arg(10000)->Arg(100000);



BENCHMARK_MAIN();