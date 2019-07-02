
// catch
#include <catch2/catch.hpp>

// std
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <tuple>

// local
#include "test_utils.h"

// to be tested
#include "core/image.h"
#include "core/image_utils.h"
#include "core/image_view.h"
#include "core/range.h"
#include "core/util.h"
#include "loaders/image_loader.h"
#include "algos/stats.h"

using namespace Catch;
using namespace openpiv::core;
using namespace openpiv::algos;

TEST_CASE("image_utils_test - constant_fill_test")
{
    g8_image im; g_8 v;
    std::tie( im, v ) = create_and_fill( size( 200, 100 ), 128_g8 );
    bool result = true;
    for ( uint32_t i=0; i<im.pixel_count(); ++i )
        result &= (im[i] == v);

    REQUIRE( result );
}

TEST_CASE("image_utils_test - generator_fill_test")
{
    g8_image im{128, 128};
    fill( im, [](auto x, auto y){ return y; } );

    bool result = true;
    for ( uint32_t h=0; h<im.height(); ++h )
        for ( uint32_t w=0; w<im.width(); ++w )
            result &= (im[{w, h}] == h);

    REQUIRE( result );
}

TEST_CASE("image_test - pnm_load_save_test")
{
    std::ifstream is("A_00001_a.tif", std::ios::binary);
    REQUIRE(is.is_open());

    std::shared_ptr<image_loader> loader{ image_loader::find_loader(is) };
    REQUIRE(!!loader);

    g16_image im;
    loader->load( is, im );

    // write data
    std::shared_ptr<image_loader> writer{ image_loader::find_loader("image/x-portable-anymap") };
    {
        std::fstream os( "A_00001_a.pgm", std::ios_base::trunc | std::ios_base::out );
        writer->save( os, im );
    }

    // re-read data, from PNM
    is = std::ifstream("A_00001_a.pgm", std::ios::binary);
    REQUIRE(is.is_open());

    loader = image_loader::find_loader(is);
    REQUIRE(!!loader);

    g16_image reloaded;
    loader->load( is, reloaded );

    {
        std::ofstream os( "reloaded.pgm", std::ios_base::trunc | std::ios_base::out );
        writer->save( os, reloaded );
    }

    // and check the two images are equal
    REQUIRE( im == reloaded );
}

TEST_CASE("image_test - rgba_split_test")
{
    rgba16_image im{ 200, 200, rgba_16{100, 200, 300, 400} };
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

TEST_CASE("image_test - rgba_join_test")
{
    std::ifstream is("test-mono.tiff", std::ios::binary);
    REQUIRE(is.is_open());

    std::shared_ptr<image_loader> loader{ image_loader::find_loader(is) };
    REQUIRE(!!loader);

    g16_image im;
    loader->load( is, im );

    g_16 min, max;
    std::tie(min, max) = find_image_range( im );
    auto scale = (max == min) ? g_16::max() : g_16::max()/(max-min);
    apply( im, [min, scale](auto i, auto v){return scale*(v-min);} );
    std::cout << "min: " << min << ", max: " << max << ", scale: " << scale << "\n";

    size output_size = im.size() - size(40, 40);
    image_view< g_16 > r{ im, rect{ { 0,  0}, output_size } };
    image_view< g_16 > g{ im, rect{ {10, 10}, output_size } };
    image_view< g_16 > b{ im, rect{ {20, 20}, output_size } };
    image_view< g_16 > a{ im, rect{ {30, 30}, output_size } };

    rgba16_image rgba = join_from_channels( r, g, b, a);

    std::shared_ptr<image_loader> writer{ image_loader::find_loader("image/x-portable-anymap") };
    std::fstream os( "test-rgbjoin.ppm", std::ios_base::trunc | std::ios_base::out );
    writer->save( os, rgba );
}

TEST_CASE("image_test - simple_transpose_test")
{
    g16_image im{ 100, 200 };
    std::iota( std::begin( im ), std::end( im ), 0 );

    REQUIRE( save_to_file( "transpose_input.pgm", im ) );

    g16_image transposed{ transpose( im ) };
    REQUIRE( save_to_file( "transpose_output.pgm", transposed ) );

    REQUIRE( transposed.width()  == im.height() );
    REQUIRE( transposed.height() == im.width() );

    for ( auto h : range_start_at(0).length(transposed.height()) )
        REQUIRE( *transposed.line(h) == h );
}

TEST_CASE("image_test - identity_transpose_test")
{
    g16_image im{ 100, 200 };
    std::iota( std::begin( im ), std::end( im ), 0 );

    g16_image transposed{ transpose( im ) };
    g16_image identity{ transpose( transposed ) };

    auto i = std::cbegin(im);
    auto e = std::cend(im);
    auto id = std::cbegin(identity);
    while ( i!=e )
    {
        REQUIRE( *i == *id );
        ++i; ++id;
    }
}

TEST_CASE("image_test - complex_transpose_test")
{
    cf_image im{ 100, 200 };
    std::generate( std::begin( im ), std::end( im ),
                   [i=0]() mutable {
                       return c_f{ i, i };
                   } );


    cf_image transposed{ transpose( im ) };

    auto i = std::cbegin(im);
    auto e = std::cend(im);
    auto id = std::cbegin(transposed);
    while ( i!=e )
    {
        REQUIRE( *i == *id );
        ++i; ++id;
    }
}

TEST_CASE("image_test - swap_quadrants_test")
{
    gf_image im{ 100, 100 };

    apply(
        im,
        [w=im.width(), h=im.height()]( auto i, auto v )
        {
            g_f x = ( i % w ) < w/2 ? 1 : 2;
            g_f y = ( i / w ) < h/2 ? 1 : 4;

            return x*y;
        }
        );

    REQUIRE( save_to_file( "swap_quadrants_input.pgm", im ) );

    size s{ im.width()/2, im.height()/2 };
    gf_image_view q1{ im, { {0, 0}, s } };
    gf_image_view q2{ im, { {50, 0}, s } };
    gf_image_view q3{ im, { {0, 50}, s } };
    gf_image_view q4{ im, { {50, 50}, s } };

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

TEST_CASE("image_test - peak_find_test")
{
    gf_image im{ 100, 100 };

    // add some peaks
    im[ {20, 20} ] = 20.0;
    im[ {30, 30} ] = 30.0;
    im[ {40, 40} ] = 40.0;
    im[ {50, 50} ] = 50.0;

    // find the peaks - in order
    auto peaks{ find_peaks( im, 3, 1 ) };
    REQUIRE( peaks.size() == 3 );
    REQUIRE( peaks[0].rect().midpoint() == rect::point_t( 50, 50 ) );
    REQUIRE( peaks[1].rect().midpoint() == rect::point_t( 40, 40 ) );
    REQUIRE( peaks[2].rect().midpoint() == rect::point_t( 30, 30 ) );
    REQUIRE( peaks[0].rect().size() == size( 3, 3 ) );
}

TEST_CASE("image_test - extract_test")
{
    gf_image im{ 100, 100 };

    // fill quadrants with values of 1, 2, 4, 8
    apply(
        im,
        [w=im.width(), h=im.height()]( auto i, auto v )
        {
            g_f x = ( i % w ) < w/2 ? 1 : 2;
            g_f y = ( i / w ) < h/2 ? 1 : 4;

            return x*y;
        }
        );

    size s{ im.width()/2, im.height()/2 };
    gf_image q1 = extract(im, { {0, 0}, s });
    gf_image q2 = extract(im, { {50, 0}, s });
    gf_image q3 = extract(im, { {0, 50}, s });
    gf_image q4 = extract(im, { {50, 50}, s });

    // check we've extracted only the part of the
    // original we wanted
    REQUIRE( pixel_sum( q1 ) == 1 * q1.pixel_count() );
    REQUIRE( pixel_sum( q2 ) == 2 * q2.pixel_count() );
    REQUIRE( pixel_sum( q3 ) == 4 * q3.pixel_count() );
    REQUIRE( pixel_sum( q4 ) == 8 * q4.pixel_count() );

    swap_quadrants( im );

    // check extracted images are truly independent
    // of original image
    REQUIRE( pixel_sum( q1 ) == 1 * q1.pixel_count() );
    REQUIRE( pixel_sum( q2 ) == 2 * q2.pixel_count() );
    REQUIRE( pixel_sum( q3 ) == 4 * q3.pixel_count() );
    REQUIRE( pixel_sum( q4 ) == 8 * q4.pixel_count() );
}
