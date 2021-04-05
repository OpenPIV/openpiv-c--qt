
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
#include "loaders/image_loader.h"
#include "core/enumerate.h"
#include "core/grid.h"
#include "core/image.h"
#include "core/image_utils.h"
#include "core/vector.h"

using namespace openpiv;

namespace std {
    template <typename T>
    ostream& operator<<(ostream& os, const vector<T>& ts)
    {
        os << "[";
        static const char separator[] = ", ";
        const char* ps = nullptr;
        for ( const auto& t : ts )
        {
            os << ( ps ? ps : "") << t;
            ps = separator;
        }
        os << "]";

        return os;
    }
}

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

    try
    {
        options
            .add_options()
            ("h, help", "help", cxxopts::value<bool>())
            ("s, size", "interrogation size", cxxopts::value<uint32_t>(size)->default_value("32"))
            ("o, overlap", "interrogation overlap", cxxopts::value<double>(overlap)->default_value("0.5"))
            ("i, input", "input files", cxxopts::value<std::vector<std::string>>(input_files))
            ("t, thread-count", "pool thread count", cxxopts::value<uint8_t>(thread_count)->default_value(std::to_string(thread_count)))
            ("e, exec", "execution method", cxxopts::value<std::string>(execution)->default_value("pool"));

        options.parse_positional({"input"});
        auto result = options.parse(argc, argv);

        if (result.count("help"))
        {
            std::cout << options.help({""}) << "\n";
            return 0;
        }

        if (result.count("input") != 2)
        {
            std::cerr << "require two input images\n";
            return 1;
        }
    }
    catch (const cxxopts::OptionException& e)
    {
        std::cerr << "error parsing options: " << e.what() << "\n";
        return 1;
    }

    // load, process
    std::cout << "size: " << size << "x" << size << "\n";
    std::cout << "overlap: " << overlap << "\n";
    std::cout << "input files: " << input_files << "\n";
    std::cout << "execution: " << execution << "\n";

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
        std::cerr << "failed to load image: " << e.what() << "\n";
        return 1;
    }

    // check image sizes
    if ( images[0].size() != images[1].size() )
    {
        std::cerr << "image sizes don't match: " << images[0].size() << ", " << images[1].size() << "\n";
        return 1;
    }

    std::cout << "loaded images: " << images[0].size() << "\n";

    // create a grid for processing
    auto ia = core::size{size, size};
    auto grid = core::generate_cartesian_grid( images[0].size(), ia, overlap );
    std::cout << "generated grid for image size: " << images[0].size() << ", ia: " << ia << " (" << overlap*100 << "% overlap)\n";
    std::cout << "grid count: " << grid.size() << "\n";

    // process!
    struct point_vector
    {
        core::point2<double> xy;
        core::vector2<double> vxy;
        double sn = 0.0;
    };
    std::vector<point_vector> found_peaks( grid.size() );
    auto fft = algos::FFT( ia );
    auto processor = [&images, &fft, &found_peaks]( size_t i, const core::rect& ia )
                     {
                         // auto view_a{ core::create_image_view( images[0], ia ) };
                         // auto view_b{ core::create_image_view( images[1], ia ) };
                         auto view_a{ core::extract( images[0], ia ) };
                         auto view_b{ core::extract( images[1], ia ) };

                         // prepare & correlate
                         core::gf_image output{ fft.cross_correlate( view_a, view_b ) };

                         // find peaks
                         constexpr uint16_t num_peaks = 2;
                         constexpr uint16_t radius = 1;
                         core::peaks_t<core::g_f> peaks = core::find_peaks( output, num_peaks, radius );

                         // sub-pixel fitting
                         if ( peaks.size() != num_peaks )
                         {
                             std::cerr << "failed to find a peak for ia: " << ia << "\n";
                             return;
                         }

                         point_vector result;
                         result.xy = ia.midpoint();
                         result.vxy =
                             core::fit_simple_gaussian( peaks[0] ) -
                             core::point2<double>{ ia.width()/2, ia.height()/2 };

                         // convert from image normal cartesian
                         result.xy[1] = images[0].height() - result.xy[1];
                         result.vxy[1] = -result.vxy[1];
                         if ( peaks[1][ {1, 1} ] > 0 )
                             result.sn = peaks[0][ {1, 1} ]/peaks[1][ {1, 1} ];

                         found_peaks[i] = std::move(result);
                     };

    // check execution
#if defined(ASYNCPLUSPLUS)
    if ( execution == "async++" )
    {
        std::cout << "processing using async++\n";
        std::atomic<size_t> i = 0;
        async::parallel_for( grid,
                             [&i, processor] (const core::rect& ia)
                             {
                                 processor(i++, ia);
                             } );
    }
    else
#endif
    {
        std::cout << "processing using thread pool\n";
        ThreadPool pool( thread_count );

        for ( const auto& [i, ia] : core::enumerate(grid) )
            pool.enqueue( [i, ia, &processor](){ processor(i, ia); } );

        using namespace std::chrono_literals;
        while ( !pool.is_idle() )
        {
            std::this_thread::sleep_for(1ms);
        }
    }

    // dump output
    for ( const auto& pv : found_peaks )
        std::cout << pv.xy[0] << ", " << pv.xy[1] << ", " << pv.vxy[0] << ", " << pv.vxy[1] << ", " << pv.sn << "\n";


    return 0;
};

