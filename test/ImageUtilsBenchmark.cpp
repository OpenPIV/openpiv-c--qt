
// google
#include <benchmark/benchmark.h>

// openpiv
#include "ImageUtils.h"
#include "ImageLoader.h"

// test
#include "TestUtils.h"

template <typename ImageT>
static void TransposeBenchmark(benchmark::State& state)
{
    uint32_t d{ (uint32_t)state.range(0) };
    Size s{ d, d };
    ImageT im{ s };

    for (auto _ : state)
    {
        auto t{ transpose(im) };
    }
}

// Register the function as a benchmark
BENCHMARK_TEMPLATE(TransposeBenchmark, G8Image)->Threads(2)->RangeMultiplier(2)->Range(2, 1024);
BENCHMARK_TEMPLATE(TransposeBenchmark, G16Image)->Threads(2)->RangeMultiplier(2)->Range(2, 1024);
BENCHMARK_TEMPLATE(TransposeBenchmark, GFImage)->Threads(2)->RangeMultiplier(2)->Range(2, 1024);
BENCHMARK_TEMPLATE(TransposeBenchmark, RGBA8Image)->Threads(2)->RangeMultiplier(2)->Range(2, 1024);
BENCHMARK_TEMPLATE(TransposeBenchmark, RGBA16Image)->Threads(2)->RangeMultiplier(2)->Range(2, 1024);
BENCHMARK_TEMPLATE(TransposeBenchmark, CFImage)->Threads(2)->RangeMultiplier(2)->Range(2, 1024);

BENCHMARK_MAIN();
