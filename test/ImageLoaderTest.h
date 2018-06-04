
// std
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
    ASSERT_EQ(loader->name(), "TIFF image loader");

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
    ASSERT_EQ(loader->name(), "TIFF image loader");

    RGBA16Image im;
    loader->load( is, im );

    ASSERT_EQ(im.width(), 511);
    ASSERT_EQ(im.height(), 369);
}
