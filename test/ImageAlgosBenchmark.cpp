
// google
#include <benchmark/benchmark.h>

// openpiv
#include "ImageAlgos.h"
#include "ImageLoader.h"

// test
#include "TestUtils.h"


static void FFTCrossCorrelationBenchmark(benchmark::State& state)
{
    auto im_a = load_from_file< GF >( "corr_a.tiff" );
    auto im_b = load_from_file< GF >( "corr_b.tiff" );
    uint32_t d{ (uint32_t)state.range(0) };
    Size s{ d, d };

    auto view_a{ create_image_view( im_a, Rect{ {}, s } ) };
    auto view_b{ create_image_view( im_b, Rect{ {}, s } ) };

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
BENCHMARK(FFTCrossCorrelationBenchmark)->Threads(2)->RangeMultiplier(2)->Range(2, 64);

static void FFTAutoCorrelationBenchmark(benchmark::State& state)
{
    auto im_a = load_from_file< GF >( "corr_a.tiff" );
    uint32_t d{ (uint32_t)state.range(0) };
    Size s{ d, d };

    auto view_a{ create_image_view( im_a, Rect{ {}, s } ) };

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
BENCHMARK(FFTAutoCorrelationBenchmark)->Threads(2)->RangeMultiplier(2)->Range(2, 64);

BENCHMARK_MAIN();
