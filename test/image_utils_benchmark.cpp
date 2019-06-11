
// google
#include <benchmark/benchmark.h>

// openpiv
#include "core/image_utils.h"
#include "loaders/image_loader.h"

// test
#include "test_utils.h"

using namespace openpiv::core;

template <typename ImageT>
static void transpose_benchmark(benchmark::State& state)
{
    uint32_t d{ (uint32_t)state.range(0) };
    size s{ d, d };
    ImageT im{ s };

    for (auto _ : state)
    {
        auto t{ transpose(im) };
    }
}

// Register the function as a benchmark
BENCHMARK_TEMPLATE(transpose_benchmark, g8_image)->Threads(2)->RangeMultiplier(2)->Range(2, 1024);
BENCHMARK_TEMPLATE(transpose_benchmark, g16_image)->Threads(2)->RangeMultiplier(2)->Range(2, 1024);
BENCHMARK_TEMPLATE(transpose_benchmark, gf_image)->Threads(2)->RangeMultiplier(2)->Range(2, 1024);
BENCHMARK_TEMPLATE(transpose_benchmark, rgba8_image)->Threads(2)->RangeMultiplier(2)->Range(2, 1024);
BENCHMARK_TEMPLATE(transpose_benchmark, rgba16_image)->Threads(2)->RangeMultiplier(2)->Range(2, 1024);
BENCHMARK_TEMPLATE(transpose_benchmark, cf_image)->Threads(2)->RangeMultiplier(2)->Range(2, 1024);

BENCHMARK_MAIN();
