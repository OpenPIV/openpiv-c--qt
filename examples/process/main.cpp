
// std
#include <atomic>
#include <chrono>
#include <cinttypes>
#include <fstream>
#include <iostream>
#include <thread>
#include <vector>

// utils
#include <cxxopts.hpp>
#include "threadpool.hpp"

#if defined(ASYNCPLUSPLUS)
#  include <async++.h>
#endif

// openpiv
#include "algos/fft.h"
#include "algos/pocket_fft.h"
#include "loaders/image_loader.h"
#include "core/enumerate.h"
#include "core/grid.h"
#include "core/image.h"
#include "core/image_utils.h"
#include "core/log.h"
#include "core/stream_utils.h"
#include "core/vector.h"

using namespace openpiv;
namespace logger = openpiv::core::logger;

int main( int argc, char* argv[] )
{
    // get arguments
    cxxopts::Options options(argv[0]);
    options
        .positional_help("[input files]")
        .show_positional_help();

    uint32_t size;
    double overlap;
    std::vector<std::string> input_files;
    std::string execution;
    uint8_t thread_count = std::thread::hardware_concurrency()-1;
    bool limit_search = false;
    std::string fft_type;
    auto log_level = logger::Level::INFO;

    try
    {
        options
            .add_options()
            ("h, help", "help", cxxopts::value<bool>())
            ("s, size", "interrogation size", cxxopts::value<uint32_t>(size)->default_value("32"))
            ("o, overlap", "interrogation overlap", cxxopts::value<double>(overlap)->default_value("0.5"))
            ("i, input", "input files", cxxopts::value<std::vector<std::string>>(input_files))
            ("t, thread-count", "pool thread count", cxxopts::value<uint8_t>(thread_count)->default_value(std::to_string(thread_count)))
            ("e, exec", "execution method", cxxopts::value<std::string>(execution)->default_value("pool"))
            ("l, limit-search", "limit peak search to central 25% of interrogation area", cxxopts::value<bool>(limit_search))
            ("f, ffttype", "FFT type", cxxopts::value<std::string>(fft_type)->default_value("complex"))
            ("loglevel", "log level", cxxopts::value<logger::Level>(log_level)->default_value("INFO"));

        options.parse_positional({"input"});
        auto result = options.parse(argc, argv);

        // setup logger
        logger::Logger::instance().add_sink(
            [log_level](logger::Level l, const std::string& m) -> bool
            {
                if ( l > log_level )
                    return true;

                std::cerr << m << "\n";
                return true;
            });

        if (result.count("help"))
        {
            std::cout << options.help({""}) << "\n";
            return 0;
        }

        if (result.count("input") != 2)
        {
            logger::error("require two input images");
            return 1;
        }
    }
    catch (const cxxopts::OptionException& e)
    {
        logger::error("error parsing options: {}", e.what());
        return 1;
    }

    // load, process
    logger::info("size: {0} x {0}", size);
    logger::info("overlap: {}", overlap);
    logger::info("input files: {}", core::join(input_files, ", "));
    logger::info("execution: {}", execution);

    // get images
    std::vector<core::gf_image> images;
    try {
        for ( const auto& input_file : input_files )
        {
            std::ifstream is(input_file, std::ios::binary);
            if ( !is.is_open() )
                core::exception_builder<std::runtime_error>() << "failed to open " << input_file;

            auto loader{ core::image_loader_registry::find(is) };
            if ( !loader )
                core::exception_builder<std::runtime_error>() << "failed to find loader for " << input_file;

            core::gf_image image;
            loader->load( is, image );
            images.emplace_back(std::move(image));
        }
    }
    catch ( std::exception& e )
    {
        logger::error("failed to load image: {}", e.what());
        return 1;
    }

    // check image sizes
    if ( images[0].size() != images[1].size() )
    {
        logger::error("image sizes don't match: {}, {}", images[0].size(), images[1].size());
        return 1;
    }

    logger::info("loaded images have size: {}", images[0].size());

    // create a grid for processing
    auto ia = core::size{size, size};
    auto grid = core::generate_cartesian_grid( images[0].size(), ia, overlap );
    logger::info("generated grid for image size: {}, ia: {} ({}% overlap)", images[0].size(), ia, overlap*100);
    logger::info("grid count: {}", grid.size());
    logger::debug("grid: {}", grid);

    // process!
    struct point_vector
    {
        core::point2<double> xy;
        core::vector2<double> vxy;
        double sn = 0.0;
    };
    std::vector<point_vector> found_peaks( grid.size() );

    // wrap correlators
    using correlator_t = std::function<core::gf_image(const core::gf_image&, const core::gf_image&)>;
    std::unordered_map<std::string, correlator_t> correlators = {
        {"complex",
         [ia](const core::gf_image& im_a, const core::gf_image& im_b) -> core::gf_image
             {
                 static algos::FFT fft{ ia };
                 return fft.cross_correlate(im_a, im_b);
             } },
        {"real",
         [ia](const core::gf_image& im_a, const core::gf_image& im_b) -> core::gf_image
             {
                 static algos::FFT fft{ ia };
                 return fft.cross_correlate_real(im_a, im_b);
             } },
        {"pocket",
         [ia](const core::gf_image& im_a, const core::gf_image& im_b) -> core::gf_image
             {
                 static algos::PocketFFT fft{ ia };
                 return fft.cross_correlate(im_a, im_b);
             } },
        {"pocket_real",
         [ia](const core::gf_image& im_a, const core::gf_image& im_b) -> core::gf_image
             {
                 static algos::PocketFFT fft{ ia };
                 return fft.cross_correlate_real(im_a, im_b);
             } } };

    if (correlators.count(fft_type) == 0)
    {
        logger::error("unknown fft type: {}", fft_type);
        return 1;
    }

    auto correlator = correlators[fft_type];

    // processing strategy
    auto processor = [&images, &found_peaks, correlator = std::move(correlator), limit_search]( size_t i, const core::rect& ia )
                     {
                         const auto view_a{ core::extract( images[0], ia ) };
                         const auto view_b{ core::extract( images[1], ia ) };

                         // prepare & correlate
                         // output of correlation has lost positional information
                         const core::gf_image output{ correlator( view_a, view_b ) };

                         // find peaks
                         constexpr uint16_t num_peaks = 2;
                         constexpr uint16_t radius = 1;

                         core::peaks_t<core::g_f> peaks;

                         if (limit_search)
                         {
                             // reduce search radius
                             auto centre = core::create_image_view( output, output.rect().dilate(0.5) );
                             peaks = core::find_peaks( centre, num_peaks, radius );
                         } else {
                             peaks = core::find_peaks( output, num_peaks, radius );
                         }

                         // sub-pixel fitting
                         if ( peaks.size() != num_peaks )
                         {
                             logger::error("failed to find a peak for ia: {}", ia);
                             return;
                         }

                         point_vector result;
                         auto bl = ia.bottomLeft();
                         auto midpoint = ia.midpoint();
                         auto peak = peaks[0];
                         auto peak_location = core::fit_simple_gaussian( peak );

                         result.xy = midpoint;
                         result.vxy = { midpoint[0] - (bl[0] + peak_location[0]), midpoint[1] - (bl[1] + peak_location[1]) };

                         // convert from image normal cartesian
                         result.xy[1] = images[0].height() - result.xy[1];

                         // find s/n (or rather, highest to next highest peak)
                         if ( peaks[1][ {1, 1} ] > 0 )
                             result.sn = peaks[0][ {1, 1} ]/peaks[1][ {1, 1} ];

                         found_peaks[i] = std::move(result);
                     };

    const auto t1 = std::chrono::high_resolution_clock::now();

    // check execution
    if (thread_count <= 1)
    {
        logger::info("processing using single thread");
        size_t i = 0;
        for ( const auto& ia : grid )
        {
            processor(i++, ia);
        }
    }
    else
#if defined(ASYNCPLUSPLUS)
    if ( execution == "async++" )
    {
        logger::info("processing using async++");
        std::atomic<size_t> i = 0;
        async::parallel_for( grid,
                             [&i, processor] (const core::rect& ia)
                             {
                                 processor(i++, ia);
                             } );
    }
    else
#endif
    if ( execution == "pool" )
    {
        logger::info("processing using thread pool");
        ThreadPool pool( thread_count );

        size_t i = 0;
        for ( const auto& ia : grid )
        {
            pool.enqueue( [i, ia, &processor](){ processor(i, ia); } );
            ++i;
        }
    }
    else if ( execution == "bulk-pool" )
    {
        logger::info("processing using thread pool with bulk split");
        ThreadPool pool( thread_count );

        // - split the grid into thread_count chunks
        // - wrap each chunk into a processing for loop and push to thread

        // ensure we don't miss grid locations due to rounding
        size_t chunk_size = grid.size()/thread_count;
        std::vector<size_t> chunk_sizes( thread_count, chunk_size );
        chunk_sizes.back() = grid.size() - (thread_count-1)*chunk_size;

        logger::debug("chunk sizes: {}", core::join(chunk_sizes, ", "));

        size_t i = 0;
        for ( const auto& chunk_size_ : chunk_sizes )
        {
            pool.enqueue(
                [i, chunk_size_, &grid, &processor]() {
                    for ( size_t j=i; j<i + chunk_size_; ++j )
                        processor(j, grid[j]);
                } );
            i += chunk_size_;
        }
    }

    const auto t2 = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double, std::micro> total_us = t2 - t1;
    logger::info(
        "processing time: {}us, {}us per interrogation area",
        total_us,
        total_us/found_peaks.size());

    // dump output
    for ( const auto& pv : found_peaks )
        std::cout << pv.xy[0] << ", " << pv.xy[1] << ", " << pv.vxy[0] << ", " << pv.vxy[1] << ", " << pv.sn << "\n";


    return 0;
};

