
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
BENCHMARK_TEMPLATE(transpose_benchmark, image_g_8)->Threads(2)->RangeMultiplier(2)->Range(2, 1024);
BENCHMARK_TEMPLATE(transpose_benchmark, image_g_16)->Threads(2)->RangeMultiplier(2)->Range(2, 1024);
BENCHMARK_TEMPLATE(transpose_benchmark, image_g_f)->Threads(2)->RangeMultiplier(2)->Range(2, 1024);
BENCHMARK_TEMPLATE(transpose_benchmark, image_rgba_8)->Threads(2)->RangeMultiplier(2)->Range(2, 1024);
BENCHMARK_TEMPLATE(transpose_benchmark, image_rgba_16)->Threads(2)->RangeMultiplier(2)->Range(2, 1024);
BENCHMARK_TEMPLATE(transpose_benchmark, image_c_f)->Threads(2)->RangeMultiplier(2)->Range(2, 1024);

BENCHMARK_MAIN();
