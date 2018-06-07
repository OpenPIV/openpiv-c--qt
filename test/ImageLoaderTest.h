
// std
#include <cstdlib>
#include <fstream>

// gtest
#include "gtest/gtest.h"

// local
#include "TestUtils.h"

// to be tested
#include "ImageLoader.h"
#include "ImageUtils.h"

TEST(ImageLoaderTest, TIFFLoaderMono)
{
    std::ifstream is("test-mono.tiff", std::ios::binary);
    ASSERT_TRUE(is.is_open());

    std::shared_ptr<ImageLoader> loader{ ImageLoader::findLoader(is) };
    ASSERT_TRUE(!!loader);
    ASSERT_EQ(loader->name(), "image/tiff");

    G16Image im;
    loader->load( is, im );

    ASSERT_EQ(im.width(), 511);
    ASSERT_EQ(im.height(), 369);
}

TEST(ImageLoaderTest, TIFFLoaderRGBA)
{
    std::ifstream is("test-rgb.tiff", std::ios::binary);
    ASSERT_TRUE(is.is_open());

    std::shared_ptr<ImageLoader> loader{ ImageLoader::findLoader(is) };
    ASSERT_TRUE(!!loader);
    ASSERT_EQ(loader->name(), "image/tiff");

    RGBA16Image im;
    loader->load( is, im );

    ASSERT_EQ(im.width(), 511);
    ASSERT_EQ(im.height(), 369);
}

TEST(ImageLoaderTest, PNMLoaderSaveP5)
{
    std::ifstream is("test-mono.tiff", std::ios::binary);
    ASSERT_TRUE(is.is_open());

    std::shared_ptr<ImageLoader> loader{ ImageLoader::findLoader(is) };
    ASSERT_TRUE(!!loader);

    G16Image im;
    loader->load( is, im );

    // write data
    std::shared_ptr<ImageLoader> writer{ ImageLoader::findLoader("image/x-portable-anymap") };
    ASSERT_TRUE(!!writer);
    ASSERT_EQ(writer->name(), "image/x-portable-anymap");

    std::fstream os( "test-mono.pgm", std::ios_base::trunc | std::ios_base::out );
    writer->save( os, im );
}

TEST(ImageLoaderTest, PNMLoaderSaveP6)
{
    std::ifstream is("test-rgb.tiff", std::ios::binary);
    ASSERT_TRUE(is.is_open());

    std::shared_ptr<ImageLoader> loader{ ImageLoader::findLoader(is) };
    ASSERT_TRUE(!!loader);

    G16Image im;
    loader->load( is, im );

    // write data
    std::shared_ptr<ImageLoader> writer{ ImageLoader::findLoader("image/x-portable-anymap") };
    ASSERT_TRUE(!!writer);
    ASSERT_EQ(writer->name(), "image/x-portable-anymap");

    std::fstream os( "test-rgb.ppm", std::ios_base::trunc | std::ios_base::out );
    writer->save( os, im );
}
