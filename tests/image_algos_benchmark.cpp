
// google
#include <benchmark/benchmark.h>

// openpiv
#include "algos/fft.h"
#include "loaders/image_loader.h"

// test
#include "test_utils.h"

using namespace openpiv::core;
using namespace openpiv::algos;

static void fft_cross_correlation_view_benchmark(benchmark::State& state)
{
    cf_image im_a{ load_from_file< g_f >( "corr_a.tiff" ) };
    uint32_t d{ (uint32_t)state.range(0) };
    size s{ d, d };
    FFT fft( s );

    auto view_a = create_image_view( im_a, rect{ {0, 0}, s } );
    auto view_b = create_image_view( im_a, rect{ {1, 1}, s } );

    for (auto _ : state)
    {
        // measure FFT speed
        fft.cross_correlate( view_a, view_b );
    }
}
// Register the function as a benchmark
BENCHMARK(fft_cross_correlation_view_benchmark)->Threads(4)->RangeMultiplier(2)->Range(4, 64);

static void fft_cross_correlation_extract_benchmark(benchmark::State& state)
{
    cf_image im_a{ load_from_file< g_f >( "corr_a.tiff" ) };
    uint32_t d{ (uint32_t)state.range(0) };
    size s{ d, d };
    FFT fft( s );

    auto sub_a = extract( im_a, rect{ {0, 0}, s } );
    auto sub_b = extract( im_a, rect{ {1, 1}, s } );

    for (auto _ : state)
    {
        // measure FFT speed
        fft.cross_correlate( sub_a, sub_b );
    }
}
// Register the function as a benchmark
BENCHMARK(fft_cross_correlation_extract_benchmark)->Threads(4)->RangeMultiplier(2)->Range(4, 64);

static void fft_auto_correlation_view_benchmark(benchmark::State& state)
{
    cf_image im_a{ load_from_file< g_f >( "corr_a.tiff" ) };
    uint32_t d{ (uint32_t)state.range(0) };
    size s{ d, d };
    FFT fft( s );

    auto view_a = create_image_view( im_a, rect{ {}, s } );

    for (auto _ : state)
    {
        // measure FFT speed
        fft.auto_correlate( view_a );
    }
}
// Register the function as a benchmark
BENCHMARK(fft_auto_correlation_view_benchmark)->Threads(4)->RangeMultiplier(2)->Range(4, 64);

static void fft_auto_correlation_extract_benchmark(benchmark::State& state)
{
    cf_image im_a{ load_from_file< g_f >( "corr_a.tiff" ) };
    uint32_t d{ (uint32_t)state.range(0) };
    size s{ d, d };
    FFT fft( s );

    auto view_a = extract( im_a, rect{ {}, s } );

    for (auto _ : state)
    {
        // measure FFT speed
        fft.auto_correlate( view_a );
    }
}
// Register the function as a benchmark
BENCHMARK(fft_auto_correlation_extract_benchmark)->Threads(4)->RangeMultiplier(2)->Range(4, 64);

BENCHMARK_MAIN();
