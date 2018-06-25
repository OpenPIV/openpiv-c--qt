
// gtest
#include "gtest/gtest.h"

// std
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>

// local
#include "TestUtils.h"

// to be tested
#include "Image.h"
#include "ImageUtils.h"
#include "ImageStats.h"
#include "ImageLoader.h"


TEST(ImageTest, IntTest)
{
    G8Image im( 200, 100 );
    ASSERT_EQ( im.width(), 200 );
    ASSERT_EQ( im.height(), 100 );
}

TEST(ImageTest, FillTest)
{
    G8Image im; G8 v;
    std::tie( im, v ) = createAndFill( Size( 200, 100 ), 128);
    bool result = true;
    for ( uint32_t i=0; i<im.pixel_count(); ++i )
        result &= (im[i] == v);

    ASSERT_EQ( result, true );
}

TEST(ImageTest, ResizeTest)
{
    G8Image im;
    ASSERT_EQ( im.width(), 0 );
    ASSERT_EQ( im.height(), 0 );

    im.resize( 100, 100 );
    ASSERT_EQ( im.width(), 100 );
    ASSERT_EQ( im.height(), 100 );

    G8 v{ 128 };
    fill( im, v );
    bool result = true;
    for ( uint32_t i=0; i<im.pixel_count(); ++i )
        result &= (im[i] == v);

    ASSERT_EQ( result, true );
}

TEST(ImageTest, CopyTest)
{
    G8Image im; G8 v;
    std::tie( im, v ) = createAndFill( Size( 200, 100 ), 128);

    G8Image im2{ im };
    ASSERT_EQ(im.width(), im2.width());
    ASSERT_EQ(im.height(), im2.height());

    bool result = true;
    for ( uint32_t i=0; i<im2.pixel_count(); ++i )
        result &= (im2[i] == v);

    ASSERT_EQ( result, true );
}

TEST(ImageTest, MoveTest)
{
    G8Image im; G8 v;
    std::tie( im, v ) = createAndFill( Size( 200, 100 ), 128);

    G8Image im2{ std::move(im) };

    bool result = true;
    for ( uint32_t i=0; i<im.pixel_count(); ++i )
        result &= (im2[i] == v);

    ASSERT_EQ( result, true );
}

TEST(ImageTest, ConvertTest)
{
    G8Image im; G8 v;
    std::tie( im, v ) = createAndFill( Size( 200, 200 ), 128);

    DoubleImage im2{ im };

    bool result = true;
    for ( uint32_t i=0; i<im.pixel_count(); ++i )
        result &= (im2[i] == v);

    ASSERT_EQ( result, true );
}

TEST(ImageTest, LineOutOfBoundsTest)
{
    G8Image im; G8 v;
    std::tie( im, v ) = createAndFill( Size( 200, 100 ), 128);

    _ASSERT_DEATH( im.line(101), std::range_error, "line out of range" );
}

TEST(ImageTest, LineTest)
{
    G8Image im; G8 v;
    std::tie( im, v ) = createAndFill( Size( 2, 2 ), 0);
    int64_t sum1 = pixel_sum(im) / im.pixel_count();
    ASSERT_EQ(sum1, 0);

    G8* p = im.line(1);
    for ( size_t i=0; i<im.width(); ++i )
        *p++ = 128;

    int64_t sum2 = pixel_sum(im) / im.pixel_count();
    ASSERT_EQ(sum2, 64);
}

TEST(ImageTest, EqualityTest)
{
    G8Image im1;
    std::tie( im1, std::ignore ) = createAndFill( Size( 200, 100 ), 128);

    G8Image im2;
    std::tie( im2, std::ignore ) = createAndFill( Size( 200, 100 ), 128);

    ASSERT_EQ( im1, im2 );

    // modify a pixel
    im2[ Point2<uint32_t>( 100u, 50u ) ] = 100;
    ASSERT_NE( im1, im2 );
}

TEST(ImageTest, ApplyTest)
{
    G8Image im; G8 v;
    std::tie( im, v ) = createAndFill( Size( 200, 200 ), 128);
    im[Point2<uint32_t>(100, 100)] = 129;

    G8 min, max;
    std::tie(min, max) = findImageRange( im );
    G8 scale{ (max-min)==0 ? 255 : 255/(max-min) };

    im.apply( [&min, &scale](auto v){return scale*(v-min);} );

    std::tie(min, max) = findImageRange( im );
    ASSERT_EQ(min, 0);
    ASSERT_EQ(max, 255);
}


TEST(ImageTest, ScaleTest)
{
    std::ifstream is("A_00001_a.tif", std::ios::binary);
    ASSERT_TRUE(is.is_open());

    std::shared_ptr<ImageLoader> loader{ ImageLoader::findLoader(is) };
    ASSERT_TRUE(!!loader);

    G16Image im;
    loader->load( is, im );

    // scale
    G16 min, max;
    std::tie(min, max) = findImageRange( im );
    G16 scale{ (max-min)==0 ? G16::max() : G16::max()/(max-min) };
    std::cout << "min: " << min << ", max: " << max << ", scale: " << scale << "\n";

    im.apply( [&min, &scale](auto v){return scale*(v-min);} );

    std::tie(min, max) = findImageRange( im );
    std::cout << "min: " << min << ", max: " << max << "\n";

    // write data
    std::shared_ptr<ImageLoader> writer{ ImageLoader::findLoader("image/x-portable-anymap") };
    ASSERT_TRUE(!!writer);
    ASSERT_EQ(writer->name(), "image/x-portable-anymap");

    std::fstream os( "A_00001_a.pgm", std::ios_base::trunc | std::ios_base::out );
    writer->save( os, im );
}
