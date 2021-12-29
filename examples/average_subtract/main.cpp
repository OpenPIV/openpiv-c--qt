
// std
#include <cinttypes>
#include <fstream>
#include <iostream>

// utils
#include <cxxopts.hpp>
#include "spdlog/spdlog.h"
#include "spdlog/cfg/env.h"
#include "spdlog/fmt/ostr.h"
#include "spdlog/sinks/stdout_color_sinks.h"

// openpiv
#include "loaders/image_loader.h"
#include "core/enumerate.h"
#include "core/image.h"
#include "core/image_utils.h"
#include "core/stream_utils.h"

using namespace openpiv;

int main( int argc, char* argv[] )
{
    // setup logger
    auto logger = spdlog::stderr_color_mt("console");
    logger->set_level(spdlog::level::info);
    spdlog::set_pattern("[%H:%M:%S %z] [%l] [thread %t] %v");
    spdlog::set_default_logger(logger);
    spdlog::cfg::load_env_levels();

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
            spdlog::error("require two or more input images");
            return 1;
        }

        use_memory = result.count("usememory");
    }
    catch (const cxxopts::OptionException& e)
    {
        spdlog::error("error parsing options: {}", e.what());
        return 1;
    }

    // load, process
    spdlog::info("input files: {}", input_files);
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
            spdlog::error("failed to load image: {}", e.what());
        }
    }

    if ( images.empty() )
    {
        spdlog::error("failed to load any files!");
        return 1;
    }

    // check image sizes
    core::size s = images[0].size();
    for ( const auto& [i, im] : core::enumerate(images) )
    {
        if ( im.size() != s )
        {
            spdlog::error("image size doesn't match (image {}: {}): got {}, expected {}",
                          i, input_files[i], im.size(), s);
            return 1;
        }
    }

    spdlog::info("loaded images");

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
        spdlog::error("failed to find loader for pnm");
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

