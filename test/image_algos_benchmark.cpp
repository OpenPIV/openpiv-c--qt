
// google
#include <benchmark/benchmark.h>

// openpiv
#include "algos/fft.h"
#include "loaders/image_loader.h"

// test
#include "test_utils.h"

using namespace openpiv::core;
using namespace openpiv::algos;

static void fft_cross_correlation_benchmark(benchmark::State& state)
{
    cf_image im_a{ load_from_file< g_f >( "corr_a.tiff" ) };
    cf_image im_b{ load_from_file< g_f >( "corr_b.tiff" ) };
    uint32_t d{ (uint32_t)state.range(0) };
    size s{ d, d };

    auto view_a = create_image_view( im_a, rect{ {}, s } );
    auto view_b = create_image_view( im_b, rect{ {}, s } );

    for (auto _ : state)
    {
        state.PauseTiming();
        FFT fft( s );
        state.ResumeTiming();

        // measure FFT speed
        fft.cross_correlate( view_a, view_b );
    }
}
// Register the function as a benchmark
BENCHMARK(fft_cross_correlation_benchmark)->Threads(2)->RangeMultiplier(2)->Range(2, 64);

static void fft_auto_correlation_benchmark(benchmark::State& state)
{
    cf_image im_a{ load_from_file< g_f >( "corr_a.tiff" ) };
    uint32_t d{ (uint32_t)state.range(0) };
    size s{ d, d };

    auto view_a = create_image_view( im_a, rect{ {}, s } );

    for (auto _ : state)
    {
        state.PauseTiming();
        FFT fft( s );
        state.ResumeTiming();

        // measure FFT speed
        fft.auto_correlate( view_a );
    }
}
// Register the function as a benchmark
BENCHMARK(fft_auto_correlation_benchmark)->Threads(2)->RangeMultiplier(2)->Range(2, 64);

BENCHMARK_MAIN();
