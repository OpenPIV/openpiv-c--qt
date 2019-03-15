
// catch
#include "catch.hpp"

// std
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <tuple>

// local
#include "TestUtils.h"

// to be tested
#include "Image.h"
#include "ImageLoader.h"
#include "ImageStats.h"
#include "ImageUtils.h"
#include "ImageView.h"
#include "Range.h"
#include "Util.h"

using namespace Catch;


TEST_CASE("ImageTest - IntTest")
{
    G8Image im( 200, 100 );
    REQUIRE( im.width()  == 200 );
    REQUIRE( im.height() == 100 );
}

TEST_CASE("ImageTest - ResizeTest")
{
    G8Image im;
    REQUIRE( im.width()  == 0 );
    REQUIRE( im.height() == 0 );

    im.resize( 100, 100 );
    REQUIRE( im.width()  == 100 );
    REQUIRE( im.height() == 100 );
    REQUIRE( im.pixel_count() == 100*100 );

    G8 v{ 128 };
    fill( im, v );
    bool result = true;
    for ( uint32_t i=0; i<im.pixel_count(); ++i )
        result &= (im[i] == v);

    REQUIRE( result );
}

TEST_CASE("ImageTest - CopyTest")
{
    G8Image im; G8 v;
    std::tie( im, v ) = create_and_fill( Size( 200, 100 ), 128_g8 );

    G8Image im2{ im };
    REQUIRE(im.width()  == im2.width());
    REQUIRE(im.height() == im2.height());

    bool result = true;
    for ( uint32_t i=0; i<im2.pixel_count(); ++i )
        result &= (im2[i] == v);

    REQUIRE( result );
}

TEST_CASE("ImageTest - MoveTest")
{
    G8Image im; G8 v;
    std::tie( im, v ) = create_and_fill( Size( 200, 100 ), 128_g8 );

    G8Image im2{ std::move(im) };

    bool result = true;
    for ( uint32_t i=0; i<im.pixel_count(); ++i )
        result &= (im2[i] == v);

    REQUIRE( result );
}

TEST_CASE("ImageTest - ConvertTest")
{
    G8Image im; G8 v;
    std::tie( im, v ) = create_and_fill( Size( 200, 200 ), 128_g8 );

    GFImage im2{ im };

    bool result = true;
    for ( uint32_t i=0; i<im.pixel_count(); ++i )
        result &= (im2[i] == v);

    REQUIRE( result );
}

TEST_CASE("ImageTest - LineOutOfBoundsTest")
{
    G8Image im; G8 v;
    std::tie( im, v ) = create_and_fill( Size( 200, 100 ), 128_g8 );

    _REQUIRE_THROWS_MATCHES( im.line(101), std::range_error, Contains( "line out of range" ) );
}

TEST_CASE("ImageTest - LineTest")
{
    G8Image im; G8 v;
    std::tie( im, v ) = create_and_fill( Size( 2, 2 ), 0_g8 );
    int64_t sum1 = pixel_sum(im) / im.pixel_count();
    REQUIRE(sum1 == 0);

    G8* p = im.line(1);
    for ( size_t i=0; i<im.width(); ++i )
        *p++ = 128;

    int64_t sum2 = pixel_sum(im) / im.pixel_count();
    REQUIRE(sum2 == 64);
}

TEST_CASE("ImageTest - EqualityTest")
{
    G8Image im1;
    std::tie( im1, std::ignore ) = create_and_fill( Size( 200, 100 ), 128_g8 );

    G8Image im2;
    std::tie( im2, std::ignore ) = create_and_fill( Size( 200, 100 ), 128_g8 );

    REQUIRE( im1 == im2 );

    // modify a pixel
    im2[ Point2<uint32_t>( 100u, 50u ) ] = 100;
    REQUIRE( im1 != im2 );
}

TEST_CASE("ImageTest - ApplyTest")
{
    G8Image im; G8 v;
    std::tie( im, v ) = create_and_fill( Size( 200, 200 ), 128_g8 );
    im[Point2<uint32_t>(100, 100)] = 129;

    G8 min, max;
    std::tie(min, max) = find_image_range( im );
    auto scale{ (max == min) ? G8::max() : G8::max()/(max-min) };

    apply( im, [&min, &scale](auto i, auto v){return scale*(v-min);} );

    std::tie(min, max) = find_image_range( im );
    REQUIRE(min == 0);
    REQUIRE(max == 255);
}


TEST_CASE("ImageTest - ScaleTest")
{
    std::ifstream is("A_00001_a.tif", std::ios::binary);
    REQUIRE(is.is_open());

    std::shared_ptr<ImageLoader> loader{ ImageLoader::find_loader(is) };
    REQUIRE(!!loader);

    G16Image im;
    loader->load( is, im );

    // scale
    G16 min, max;
    std::tie(min, max) = find_image_range( im );
    auto scale{ (max == min) ? G16::max() : G16::max()/(max-min) };
    std::cout << "min: " << min << ", max: " << max << ", scale: " << scale << "\n";

    apply( im, [min, scale](auto i, auto v){return scale*(v-min);} );

    std::tie(min, max) = find_image_range( im );
    std::cout << "min: " << min << ", max: " << max << "\n";

    // write data
    std::shared_ptr<ImageLoader> writer{ ImageLoader::find_loader("image/x-portable-anymap") };
    REQUIRE(!!writer);
    REQUIRE(writer->name() == "image/x-portable-anymap");

    std::fstream os( "A_00001_a.pgm", std::ios_base::trunc | std::ios_base::out );
    writer->save( os, im );
}

TEST_CASE("ImageTest - ComplexConversionTest")
{
    auto [ g_im, v ] = create_and_fill< G8 >( { 200, 200 }, 127 );
    CFImage c_im{ g_im };
    {
        auto [ min, max ] = find_image_range( c_im );
        REQUIRE( min == max );
        REQUIRE( (min == CF{ 127, 0 }) );
    }

    auto [ real, imag ] = split_to_channels( c_im );
    {
        auto [ min, max ] = find_image_range( real );
        REQUIRE( min == max );
        REQUIRE( min == 127 );
    }

    {
        auto [ min, max ] = find_image_range( imag );
        REQUIRE( min == max );
        REQUIRE( min == 0 );
    }
}

TEST_CASE("ImageTest - IteratorTest")
{
    G16Image im{ 100, 100 };
    std::iota( std::begin( im ), std::end( im ), 0 );

    for ( auto h : range_start_at(0).length(im.height()) )
        REQUIRE( *im.line(h) == h*100 );
}

