
// std
#include <cinttypes>
#include <fstream>
#include <iostream>

// utils
#include <cxxopts.hpp>

// openpiv
#include "loaders/image_loader.h"
#include "core/enumerate.h"
#include "core/image.h"
#include "core/image_utils.h"

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

    std::vector<std::string> input_files;
    bool use_memory = true;

    try
    {
        options
            .add_options()
            ("h, help", "help", cxxopts::value<bool>())
            ("m, usememory", "keep images in memory", cxxopts::value<bool>())
            ("i, input", "input files", cxxopts::value<std::vector<std::string>>(input_files));

        options.parse_positional({"input"});
        auto result = options.parse(argc, argv);

        if (result.count("help"))
        {
            std::cout << options.help({""}) << "\n";
            return 0;
        }

        if (result.count("input") < 2)
        {
            std::cerr << "require two or more input images\n";
            return 1;
        }

        use_memory = result.count("usememory");
    }
    catch (const cxxopts::OptionException& e)
    {
        std::cerr << "error parsing options: " << e.what() << "\n";
        return 1;
    }

    // load, process
    std::cout << "input files: " << input_files << "\n";
    // std::cout << "using memory: " << std::boolalpha << use_memory << "\n";

    // get images
    std::vector<core::gf_image> images;
    for ( const auto& input_file : input_files )
    {
        try {
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
        catch ( std::exception& e )
        {
            std::cerr << "failed to load image: " << e.what() << "\n";
        }
    }

    if ( images.empty() )
    {
        std::cerr << "failed to load any files!\n";
        return 1;
    }

    // check image sizes
    core::size s = images[0].size();
    for ( const auto& [i, im] : core::enumerate(images) )
    {
        if ( im.size() != s )
        {
            std::cerr << "image size doesn't match (image " << i << ": " << input_files[i] << "): got " << im.size() << ", expected " << s << "\n";
            return 1;
        }
    }

    std::cout << "loaded images\n";

    // process:
    // - find average
    // - for each image, subtract average and write as <name>.sub
    core::gf_image avg{s};
    for ( const auto& im : images )
        avg = avg + im;

    avg = avg / core::g_f{images.size()};

    std::shared_ptr<core::image_loader> writer{ core::image_loader_registry::find("image/x-portable-anymap") };
    if ( !writer )
    {
        std::cerr << "failed to find loader for pnm\n";
        return 1;
    }

    // iterate over all images, subtract average and write
    for ( auto [i, im] : core::enumerate(images) )
    {
        im = im - avg;
        std::fstream os( input_files[i] + ".avg_sub.pnm", std::ios_base::trunc | std::ios_base::out | std::ios_base::binary );
        writer->save( os, im );
    }

    return 0;
};

