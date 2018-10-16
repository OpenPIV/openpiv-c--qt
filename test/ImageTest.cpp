
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

TEST_CASE("ImageTest - FillTest")
{
    G8Image im; G8 v;
    std::tie( im, v ) = createAndFill( Size( 200, 100 ), 128_g8 );
    bool result = true;
    for ( uint32_t i=0; i<im.pixel_count(); ++i )
        result &= (im[i] == v);

    REQUIRE( result );
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
    std::tie( im, v ) = createAndFill( Size( 200, 100 ), 128_g8 );

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
    std::tie( im, v ) = createAndFill( Size( 200, 100 ), 128_g8 );

    G8Image im2{ std::move(im) };

    bool result = true;
    for ( uint32_t i=0; i<im.pixel_count(); ++i )
        result &= (im2[i] == v);

    REQUIRE( result );
}

TEST_CASE("ImageTest - ConvertTest")
{
    G8Image im; G8 v;
    std::tie( im, v ) = createAndFill( Size( 200, 200 ), 128_g8 );

    GFImage im2{ im };

    bool result = true;
    for ( uint32_t i=0; i<im.pixel_count(); ++i )
        result &= (im2[i] == v);

    REQUIRE( result );
}

TEST_CASE("ImageTest - LineOutOfBoundsTest")
{
    G8Image im; G8 v;
    std::tie( im, v ) = createAndFill( Size( 200, 100 ), 128_g8 );

    _REQUIRE_THROWS_MATCHES( im.line(101), std::range_error, Contains( "line out of range" ) );
}

TEST_CASE("ImageTest - LineTest")
{
    G8Image im; G8 v;
    std::tie( im, v ) = createAndFill( Size( 2, 2 ), 0_g8 );
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
    std::tie( im1, std::ignore ) = createAndFill( Size( 200, 100 ), 128_g8 );

    G8Image im2;
    std::tie( im2, std::ignore ) = createAndFill( Size( 200, 100 ), 128_g8 );

    REQUIRE( im1 == im2 );

    // modify a pixel
    im2[ Point2<uint32_t>( 100u, 50u ) ] = 100;
    REQUIRE( im1 != im2 );
}

TEST_CASE("ImageTest - ApplyTest")
{
    G8Image im; G8 v;
    std::tie( im, v ) = createAndFill( Size( 200, 200 ), 128_g8 );
    im[Point2<uint32_t>(100, 100)] = 129;

    G8 min, max;
    std::tie(min, max) = find_image_range( im );
    auto scale{ (max == min) ? G8::max() : G8::max()/(max-min) };

    im.apply( [&min, &scale](auto i, auto v){return scale*(v-min);} );

    std::tie(min, max) = find_image_range( im );
    REQUIRE(min == 0);
    REQUIRE(max == 255);
}


TEST_CASE("ImageTest - ScaleTest")
{
    std::ifstream is("A_00001_a.tif", std::ios::binary);
    REQUIRE(is.is_open());

    std::shared_ptr<ImageLoader> loader{ ImageLoader::findLoader(is) };
    REQUIRE(!!loader);

    G16Image im;
    loader->load( is, im );

    // scale
    G16 min, max;
    std::tie(min, max) = find_image_range( im );
    auto scale{ (max == min) ? G16::max() : G16::max()/(max-min) };
    std::cout << "min: " << min << ", max: " << max << ", scale: " << scale << "\n";

    im.apply( [min, scale](auto i, auto v){return scale*(v-min);} );

    std::tie(min, max) = find_image_range( im );
    std::cout << "min: " << min << ", max: " << max << "\n";

    // write data
    std::shared_ptr<ImageLoader> writer{ ImageLoader::findLoader("image/x-portable-anymap") };
    REQUIRE(!!writer);
    REQUIRE(writer->name() == "image/x-portable-anymap");

    std::fstream os( "A_00001_a.pgm", std::ios_base::trunc | std::ios_base::out );
    writer->save( os, im );
}

TEST_CASE("ImageTest - PNMLoadSaveTest")
{
    std::ifstream is("A_00001_a.tif", std::ios::binary);
    REQUIRE(is.is_open());

    std::shared_ptr<ImageLoader> loader{ ImageLoader::findLoader(is) };
    REQUIRE(!!loader);

    G16Image im;
    loader->load( is, im );

    // write data
    std::shared_ptr<ImageLoader> writer{ ImageLoader::findLoader("image/x-portable-anymap") };
    {
        std::fstream os( "A_00001_a.pgm", std::ios_base::trunc | std::ios_base::out );
        writer->save( os, im );
    }

    // re-read data, from PNM
    is = std::ifstream("A_00001_a.pgm", std::ios::binary);
    REQUIRE(is.is_open());

    loader = ImageLoader::findLoader(is);
    REQUIRE(!!loader);

    G16Image reloaded;
    loader->load( is, reloaded );

    {
        std::ofstream os( "reloaded.pgm", std::ios_base::trunc | std::ios_base::out );
        writer->save( os, reloaded );
    }

    // and check the two images are equal
    REQUIRE( im == reloaded );
}

TEST_CASE("ImageTest - RGBASplitTest")
{
    RGBA16Image im{ 200, 200, RGBA16{100, 200, 300, 400} };
    auto [ r, g, b, a ] = split_to_channels(im);

    {
        auto [ min, max ] = find_image_range( r );
        REQUIRE( min == max );
        REQUIRE( min == 100 );
    }

    {
        auto [ min, max ] = find_image_range( g );
        REQUIRE( min == max );
        REQUIRE( min == 200 );
    }

    {
        auto [ min, max ] = find_image_range( b );
        REQUIRE( min == max );
        REQUIRE( min == 300 );
    }

    {
        auto [ min, max ] = find_image_range( a );
        REQUIRE( min == max );
        REQUIRE( min == 400 );
    }
}

TEST_CASE("ImageTest - RGBAJoinTest")
{
    std::ifstream is("test-mono.tiff", std::ios::binary);
    REQUIRE(is.is_open());

    std::shared_ptr<ImageLoader> loader{ ImageLoader::findLoader(is) };
    REQUIRE(!!loader);

    G16Image im;
    loader->load( is, im );

    G16 min, max;
    std::tie(min, max) = find_image_range( im );
    auto scale = (max == min) ? G16::max() : G16::max()/(max-min);
    im.apply( [min, scale](auto i, auto v){return scale*(v-min);} );
    std::cout << "min: " << min << ", max: " << max << ", scale: " << scale << "\n";

    Size outputSize = im.size() - Size(40, 40);
    ImageView< G16 > r{ im, Rect{ { 0,  0}, outputSize } };
    ImageView< G16 > g{ im, Rect{ {10, 10}, outputSize } };
    ImageView< G16 > b{ im, Rect{ {20, 20}, outputSize } };
    ImageView< G16 > a{ im, Rect{ {30, 30}, outputSize } };

    RGBA16Image rgba = join_from_channels( r, g, b, a);

    std::shared_ptr<ImageLoader> writer{ ImageLoader::findLoader("image/x-portable-anymap") };
    std::fstream os( "test-rgbjoin.ppm", std::ios_base::trunc | std::ios_base::out );
    writer->save( os, rgba );
}

TEST_CASE("ImageTest - ComplexConversionTest")
{
    auto [ g_im, v ] = createAndFill< G8 >( { 200, 200 }, 127 );
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

TEST_CASE("ImageTest - SimpleTransposeTest")
{
    G16Image im{ 100, 200 };
    std::iota( std::begin( im ), std::end( im ), 0 );

    REQUIRE( saveToFile( "transpose_input.pgm", im ) );

    G16Image transposed{ transpose( im ) };
    REQUIRE( saveToFile( "transpose_output.pgm", transposed ) );

    REQUIRE( transposed.width()  == im.height() );
    REQUIRE( transposed.height() == im.width() );

    for ( auto h : range_start_at(0).length(transposed.height()) )
        REQUIRE( *transposed.line(h) == h );
}

TEST_CASE("ImageTest - IdentityTransposeTest")
{
    G16Image im{ 100, 200 };
    std::iota( std::begin( im ), std::end( im ), 0 );

    G16Image transposed{ transpose( im ) };
    G16Image identity{ transpose( transposed ) };

    auto i = std::cbegin(im);
    auto e = std::cend(im);
    auto id = std::cbegin(identity);
    while ( i!=e )
    {
        REQUIRE( *i == *id );
        ++i; ++id;
    }
}

TEST_CASE("ImageTest - ComplexTransposeTest")
{
    CFImage im{ 100, 200 };
    std::generate( std::begin( im ), std::end( im ),
                   [i=0]() mutable {
                       return CF{ i, i };
                   } );


    CFImage transposed{ transpose( im ) };

    auto i = std::cbegin(im);
    auto e = std::cend(im);
    auto id = std::cbegin(transposed);
    while ( i!=e )
    {
        REQUIRE( *i == *id );
        ++i; ++id;
    }
}

TEST_CASE("ImageTest - SwapQuadrantsTest")
{
    GFImage im{ 100, 100 };

    im.apply(
        [w=im.width(), h=im.height()]( auto i, auto v )
        {
            GF x = ( i % w ) < w/2 ? 1 : 2;
            GF y = ( i / w ) < h/2 ? 1 : 4;

            return x*y;
        }
        );

    REQUIRE( saveToFile( "swap_quadrants_input.pgm", im ) );

    Size s{ im.width()/2, im.height()/2 };
    GFImageView q1{ createImageView( im, { {0, 0}, s } ) };
    GFImageView q2{ createImageView( im, { {50, 0}, s } ) };
    GFImageView q3{ createImageView( im, { {0, 50}, s } ) };
    GFImageView q4{ createImageView( im, { {50, 50}, s } ) };

    REQUIRE( pixel_sum( q1 ) == 1 * q1.pixel_count() );
    REQUIRE( pixel_sum( q2 ) == 2 * q2.pixel_count() );
    REQUIRE( pixel_sum( q3 ) == 4 * q3.pixel_count() );
    REQUIRE( pixel_sum( q4 ) == 8 * q4.pixel_count() );

    swap_quadrants( im );

    REQUIRE( saveToFile( "swap_quadrants_output.pgm", im ) );

    REQUIRE( pixel_sum( q1 ) == 8 * q1.pixel_count() );
    REQUIRE( pixel_sum( q2 ) == 4 * q2.pixel_count() );
    REQUIRE( pixel_sum( q3 ) == 2 * q3.pixel_count() );
    REQUIRE( pixel_sum( q4 ) == 1 * q4.pixel_count() );
}
