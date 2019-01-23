
// catch
#include "catch.hpp"

// std
#include <cstdlib>
#include <fstream>

// local
#include "TestUtils.h"

// to be tested
#include "ImageLoader.h"
#include "ImageUtils.h"

TEST_CASE("ImageLoaderTest - TIFFLoaderMono")
{
    std::ifstream is("test-mono.tiff", std::ios::binary);
    REQUIRE(is.is_open());

    std::shared_ptr<ImageLoader> loader{ ImageLoader::find_loader(is) };
    REQUIRE(!!loader);
    REQUIRE(loader->name() == std::string("image/tiff") );

    G16Image im;
    loader->load( is, im );

    REQUIRE(im.width() == 511);
    REQUIRE(im.height() == 369);
}

TEST_CASE("ImageLoaderTest - TIFFLoaderRGBA")
{
    std::ifstream is("test-rgb.tiff", std::ios::binary);
    REQUIRE(is.is_open());

    std::shared_ptr<ImageLoader> loader{ ImageLoader::find_loader(is) };
    REQUIRE(!!loader);
    REQUIRE(loader->name() == std::string("image/tiff"));

    RGBA16Image im;
    loader->load( is, im );

    REQUIRE(im.width() == 511);
    REQUIRE(im.height() == 369);
}

TEST_CASE("ImageLoaderTest - PNMLoaderSaveP5")
{
    std::ifstream is("test-mono.tiff", std::ios::binary);
    REQUIRE(is.is_open());

    std::shared_ptr<ImageLoader> loader{ ImageLoader::find_loader(is) };
    REQUIRE(!!loader);

    G16Image im;
    loader->load( is, im );

    // write data
    std::shared_ptr<ImageLoader> writer{ ImageLoader::find_loader("image/x-portable-anymap") };
    REQUIRE(!!writer);
    REQUIRE(writer->name() == std::string("image/x-portable-anymap") );

    std::fstream os( "test-mono.pgm", std::ios_base::trunc | std::ios_base::out );
    writer->save( os, im );
}

TEST_CASE("ImageLoaderTest - PNMLoaderSaveP6")
{
    std::ifstream is("test-rgb.tiff", std::ios::binary);
    REQUIRE(is.is_open());

    std::shared_ptr<ImageLoader> loader{ ImageLoader::find_loader(is) };
    REQUIRE(!!loader);

    G16Image im;
    loader->load( is, im );

    // write data
    std::shared_ptr<ImageLoader> writer{ ImageLoader::find_loader("image/x-portable-anymap") };
    REQUIRE(!!writer);
    REQUIRE(writer->name() == std::string("image/x-portable-anymap") );

    std::fstream os( "test-rgb.ppm", std::ios_base::trunc | std::ios_base::out );
    writer->save( os, im );
}
