
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


TEST_CASE("ImageUtilsTest - ConstantFillTest")
{
    G8Image im; G8 v;
    std::tie( im, v ) = create_and_fill( Size( 200, 100 ), 128_g8 );
    bool result = true;
    for ( uint32_t i=0; i<im.pixel_count(); ++i )
        result &= (im[i] == v);

    REQUIRE( result );
}

TEST_CASE("ImageUtilsTest - GeneratorFillTest")
{
    G8Image im{128, 128};
    fill( im, [](auto x, auto y){ return y; } );

    bool result = true;
    for ( uint32_t h=0; h<im.height(); ++h )
        for ( uint32_t w=0; w<im.width(); ++w )
            result &= (im[{w, h}] == h);

    REQUIRE( result );
}

TEST_CASE("ImageTest - PNMLoadSaveTest")
{
    std::ifstream is("A_00001_a.tif", std::ios::binary);
    REQUIRE(is.is_open());

    std::shared_ptr<ImageLoader> loader{ ImageLoader::find_loader(is) };
    REQUIRE(!!loader);

    G16Image im;
    loader->load( is, im );

    // write data
    std::shared_ptr<ImageLoader> writer{ ImageLoader::find_loader("image/x-portable-anymap") };
    {
        std::fstream os( "A_00001_a.pgm", std::ios_base::trunc | std::ios_base::out );
        writer->save( os, im );
    }

    // re-read data, from PNM
    is = std::ifstream("A_00001_a.pgm", std::ios::binary);
    REQUIRE(is.is_open());

    loader = ImageLoader::find_loader(is);
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

    std::shared_ptr<ImageLoader> loader{ ImageLoader::find_loader(is) };
    REQUIRE(!!loader);

    G16Image im;
    loader->load( is, im );

    G16 min, max;
    std::tie(min, max) = find_image_range( im );
    auto scale = (max == min) ? G16::max() : G16::max()/(max-min);
    apply( im, [min, scale](auto i, auto v){return scale*(v-min);} );
    std::cout << "min: " << min << ", max: " << max << ", scale: " << scale << "\n";

    Size outputSize = im.size() - Size(40, 40);
    ImageView< G16 > r{ im, Rect{ { 0,  0}, outputSize } };
    ImageView< G16 > g{ im, Rect{ {10, 10}, outputSize } };
    ImageView< G16 > b{ im, Rect{ {20, 20}, outputSize } };
    ImageView< G16 > a{ im, Rect{ {30, 30}, outputSize } };

    RGBA16Image rgba = join_from_channels( r, g, b, a);

    std::shared_ptr<ImageLoader> writer{ ImageLoader::find_loader("image/x-portable-anymap") };
    std::fstream os( "test-rgbjoin.ppm", std::ios_base::trunc | std::ios_base::out );
    writer->save( os, rgba );
}

TEST_CASE("ImageTest - SimpleTransposeTest")
{
    G16Image im{ 100, 200 };
    std::iota( std::begin( im ), std::end( im ), 0 );

    REQUIRE( save_to_file( "transpose_input.pgm", im ) );

    G16Image transposed{ transpose( im ) };
    REQUIRE( save_to_file( "transpose_output.pgm", transposed ) );

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

    apply(
        im,
        [w=im.width(), h=im.height()]( auto i, auto v )
        {
            GF x = ( i % w ) < w/2 ? 1 : 2;
            GF y = ( i / w ) < h/2 ? 1 : 4;

            return x*y;
        }
        );

    REQUIRE( save_to_file( "swap_quadrants_input.pgm", im ) );

    Size s{ im.width()/2, im.height()/2 };
    GFImageView q1{ create_image_view( im, { {0, 0}, s } ) };
    GFImageView q2{ create_image_view( im, { {50, 0}, s } ) };
    GFImageView q3{ create_image_view( im, { {0, 50}, s } ) };
    GFImageView q4{ create_image_view( im, { {50, 50}, s } ) };

    REQUIRE( pixel_sum( q1 ) == 1 * q1.pixel_count() );
    REQUIRE( pixel_sum( q2 ) == 2 * q2.pixel_count() );
    REQUIRE( pixel_sum( q3 ) == 4 * q3.pixel_count() );
    REQUIRE( pixel_sum( q4 ) == 8 * q4.pixel_count() );

    swap_quadrants( im );

    REQUIRE( save_to_file( "swap_quadrants_output.pgm", im ) );

    REQUIRE( pixel_sum( q1 ) == 8 * q1.pixel_count() );
    REQUIRE( pixel_sum( q2 ) == 4 * q2.pixel_count() );
    REQUIRE( pixel_sum( q3 ) == 2 * q3.pixel_count() );
    REQUIRE( pixel_sum( q4 ) == 1 * q4.pixel_count() );
}

TEST_CASE("ImageTest - PeakFindTest")
{
    GFImage im{ 100, 100 };

    // add some peaks
    im[ {20, 20} ] = 20.0;
    im[ {30, 30} ] = 30.0;
    im[ {40, 40} ] = 40.0;
    im[ {50, 50} ] = 50.0;

    // find the peaks - in order
    auto peaks{ find_peaks( im, 3, 1 ) };
    REQUIRE( peaks.size() == 3 );
    REQUIRE( peaks[0].rect().midpoint() == Rect::PointT( 50, 50 ) );
    REQUIRE( peaks[1].rect().midpoint() == Rect::PointT( 40, 40 ) );
    REQUIRE( peaks[2].rect().midpoint() == Rect::PointT( 30, 30 ) );
    REQUIRE( peaks[0].rect().size() == Size( 3, 3 ) );
}
