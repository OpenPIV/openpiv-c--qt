
// std
#include <atomic>
#include <chrono>
#include <cinttypes>
#include <fstream>
#include <iostream>
#include <thread>

// utils
#include <cxxopts.hpp>
#include <async++.h>

// openpiv
#include "algos/fft.h"
#include "loaders/image_loader.h"
#include "core/grid.h"
#include "core/image.h"
#include "core/image_view.h"
#include "core/image_utils.h"
#include "core/vector.h"

using namespace openpiv;

namespace std {
    template <typename T>
    std::ostream& operator<<(std::ostream& os, const std::vector<T>& ts)
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

    try
    {
        options
            .add_options()
            ("h, help", "help", cxxopts::value<bool>())
            ("s, size", "interrogation size", cxxopts::value<uint32_t>(size)->default_value("32"))
            ("o, overlap", "interrogation overlap", cxxopts::value<double>(overlap)->default_value("0.5"))
            ("i, input", "input files", cxxopts::value<std::vector<std::string>>(input_files));

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

    // get images
    std::vector<core::gf_image> images;
    try {
        for ( const auto& input_file : input_files )
        {
            std::ifstream is(input_file, std::ios::binary);
            if ( !is.is_open() )
                core::exception_builder<std::runtime_error>() << "failed to open " << input_file;

            auto loader{ core::image_loader::find_loader(is) };
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
    auto grid{ core::generate_cartesian_grid( images[0].size(), ia, overlap ) };

    // process!
    struct point_vector
    {
        core::point2<double> xy;
        core::vector2<double> vxy;
    };
    std::vector<point_vector> found_peaks( grid.size() );
    std::atomic<int> peak_count = 0;
    auto fft = algos::FFT( ia );
    async::parallel_for( grid,
                         [&images, &fft, &found_peaks, &peak_count]( const core::rect& ia )
                         {
                             auto view_a{ core::create_image_view( images[0], ia ) };
                             auto view_b{ core::create_image_view( images[1], ia ) };

                             // prepare & correlate
                             core::gf_image output{ fft.cross_correlate( view_a, view_b ) };

                             // find peaks
                             core::peaks_t<core::g_f> peaks = core::find_peaks( output, 1, 1 );

                             // sub-pixel fitting
                             if ( peaks.empty() )
                             {
                                 std::cerr << "failed to find a peak for ia: " << ia << "\n";
                                 return;
                             }

                             point_vector result;
                             result.xy = ia.midpoint();
                             result.vxy =
                                 core::fit_simple_gaussian( peaks[0] ) -
                                 core::point2<double>{ ia.width()/2, ia.height()/2 };

                             found_peaks[peak_count++] = std::move(result);
                         } );

    // dump output
    for ( const auto& pv : found_peaks )
        std::cout << pv.xy[0] << ", " << pv.xy[1] << ", " << pv.vxy[0] << ", " << pv.vxy[1] << "\n";


    return 0;
};

