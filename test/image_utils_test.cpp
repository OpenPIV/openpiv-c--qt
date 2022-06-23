
// catch
#include <catch2/catch_test_macros.hpp>

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
#include "core/stream_utils.h"
#include "core/util.h"
#include "loaders/image_loader.h"
#include "algos/stats.h"

using namespace Catch;
using namespace openpiv::core;
using namespace openpiv::algos;
namespace logger = openpiv::core::logger;

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
    fill( im, [](auto, auto y){ return y; } );

    bool result = true;
    for ( uint32_t h=0; h<im.height(); ++h )
        for ( uint32_t w=0; w<im.width(); ++w )
            result &= (im[{w, h}] == h);

    REQUIRE( result );
}

TEST_CASE("image_utils_test - pnm_load_save_test")
{
    std::ifstream is("A_00001_a.tif", std::ios::binary);
    REQUIRE(is.is_open());

    std::shared_ptr<image_loader> loader{ image_loader_registry::find(is) };
    REQUIRE(!!loader);

    g16_image im;
    loader->load( is, im );

    // write data
    std::shared_ptr<image_loader> writer{ image_loader_registry::find("image/x-portable-anymap") };
    {
        std::fstream os( "A_00001_a.pgm", std::ios_base::trunc | std::ios_base::out | std::ios_base::binary );
        writer->save( os, im );
    }

    // re-read data, from PNM
    is = std::ifstream("A_00001_a.pgm", std::ios::binary);
    REQUIRE(is.is_open());

    loader = image_loader_registry::find(is);
    REQUIRE(!!loader);

    g16_image reloaded;
    loader->load( is, reloaded );

    {
        std::ofstream os( "reloaded.pgm", std::ios_base::trunc | std::ios_base::out | std::ios_base::binary );
        writer->save( os, reloaded );
    }

    // and check the two images are equal
    REQUIRE( im == reloaded );
}

TEST_CASE("image_utils_test - rgba_split_test")
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

TEST_CASE("image_utils_test - rgba_join_test")
{
    std::ifstream is("test-mono.tiff", std::ios::binary);
    REQUIRE(is.is_open());

    std::shared_ptr<image_loader> loader{ image_loader_registry::find(is) };
    REQUIRE(!!loader);

    g16_image im;
    loader->load( is, im );

    g_16 min, max;
    std::tie(min, max) = find_image_range( im );
    auto scale = (max == min) ? g_16::max() : g_16::max()/(max-min);
    apply( im, [min, scale](auto, auto v){return scale*(v-min);} );
    std::cout << "min: " << min << ", max: " << max << ", scale: " << scale << "\n";

    size output_size = im.size() - size(40, 40);
    auto r = create_image_view( im, rect{ { 0,  0}, output_size } );
    auto g = create_image_view( im, rect{ {10, 10}, output_size } );
    auto b = create_image_view( im, rect{ {20, 20}, output_size } );
    auto a = create_image_view( im, rect{ {30, 30}, output_size } );

    rgba16_image rgba = join_from_channels( r, g, b, a);

    std::shared_ptr<image_loader> writer{ image_loader_registry::find("image/x-portable-anymap") };
    std::fstream os( "test-rgbjoin.ppm", std::ios_base::trunc | std::ios_base::out | std::ios_base::binary );
    writer->save( os, rgba );
}

TEST_CASE("image_utils_test - simple_transpose_test")
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

TEST_CASE("image_utils_test - identity_transpose_test")
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

TEST_CASE("image_utils_test - complex_transpose_test")
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

TEST_CASE("image_utils_test - swap_quadrants_test")
{
    gf_image im{ 100, 100 };

    apply(
        im,
        [w=im.width(), h=im.height()]( auto i, auto )
        {
            g_f x = ( i % w ) < w/2 ? 1 : 2;
            g_f y = ( i / w ) < h/2 ? 1 : 4;

            return x*y;
        }
        );

    REQUIRE( save_to_file( "swap_quadrants_input.pgm", im ) );

    size s{ im.width()/2, im.height()/2 };
    auto q1 = create_image_view( im, { {0, 0}, s } );
    auto q2 = create_image_view( im, { {50, 0}, s } );
    auto q3 = create_image_view( im, { {0, 50}, s } );
    auto q4 = create_image_view( im, { {50, 50}, s } );

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

TEST_CASE("image_utils_test - peak_find_test")
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
    CHECK( peaks[0].rect().midpoint() == rect::point_t( 50, 50 ) );
    CHECK( peaks[1].rect().midpoint() == rect::point_t( 40, 40 ) );
    CHECK( peaks[2].rect().midpoint() == rect::point_t( 30, 30 ) );
    CHECK( peaks[0].rect().size() == size( 3, 3 ) );

    CHECK( peaks[0][ {1, 1} ] == 50.0 );
    CHECK( peaks[1][ {1, 1} ] == 40.0 );
    CHECK( peaks[2][ {1, 1} ] == 30.0 );
}

TEST_CASE("image_utils_test - peak_find_test_with_offset")
{
    auto sink_id = logger::Logger::instance().add_sink(
        [](logger::Level, const std::string& m) -> bool
            {
                std::cout << m << "\n";
                return true;
            });

    rect::point_t o{ 10, 10 };
    size s{ 100, 100 };
    const auto [ox, oy] = o.data();
    gf_image im{ rect( o, s ) };
    REQUIRE(im.rect() == rect( o, s ));

    // add some peaks
    im[ {20, 20} ] = 20.0;
    im[ {30, 30} ] = 30.0;
    im[ {40, 40} ] = 40.0;
    im[ {50, 50} ] = 50.0;

    // find the peaks - in order
    auto peaks{ find_peaks( im, 3, 1 ) };
    logger::sync_debug("image: {}", &im);
    logger::sync_debug("peaks: {}", peaks);
    REQUIRE( peaks.size() == 3 );
    CHECK( peaks[0].rect().midpoint() == rect::point_t( ox + 50, oy + 50 ) );
    CHECK( peaks[1].rect().midpoint() == rect::point_t( ox + 40, oy + 40 ) );
    CHECK( peaks[2].rect().midpoint() == rect::point_t( ox + 30, oy + 30 ) );
    CHECK( peaks[0].rect().size() == size( 3, 3 ) );

    CHECK( peaks[0][ {1, 1} ] == 50.0 );
    CHECK( peaks[1][ {1, 1} ] == 40.0 );
    CHECK( peaks[2][ {1, 1} ] == 30.0 );

    logger::Logger::instance().remove_sink(sink_id);
}

TEST_CASE("image_utils_test - peak_find_test - empty")
{
    gf_image im{ 100, 100 };

    // find the peaks - in order
    auto peaks{ find_peaks( im, 3, 1 ) };
    REQUIRE( peaks.size() == 0 );
}

TEST_CASE("image_utils_test - extract_test")
{
    gf_image im{ 100, 100 };

    // fill quadrants with values of 1, 2, 4, 8
    apply(
        im,
        [w=im.width(), h=im.height()]( auto i, auto )
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
    REQUIRE( q1.rect().bottomLeft() == rect::point_t{ 0, 0 } );
    REQUIRE( q2.rect().bottomLeft() == rect::point_t{ 50, 0 } );
    REQUIRE( q3.rect().bottomLeft() == rect::point_t{ 0, 50 } );
    REQUIRE( q4.rect().bottomLeft() == rect::point_t{ 50, 50 } );

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


TEST_CASE("image_utils_test - extract_image_view_test")
{
    gf_image im{ 100, 100 };

    // fill central 25% with 1 else 0
    apply(
        im,
        [w=im.width(), h=im.height()]( auto i, auto )
        {
            auto x = ( i % w );
            auto y = ( i / w );

            if ( x >= w/4 && x < 3*w/4 &&
                 y >= h/4 && y < 3*h/4 )
                return 1;

            return 0;
        }
        );

    size s{ im.width()/2, im.height()/2 };
    gf_image c1 = extract(im, { {25, 25}, s });
    gf_image c2 = extract(
        create_image_view(im, im.rect().dilate(0.5)),
        { {}, s }
        );

    // check we've extracted only the part of the
    // original we wanted
    REQUIRE( pixel_sum( c1 ) == 1 * c1.pixel_count() );
    REQUIRE( pixel_sum( c2 ) == 1 * c2.pixel_count() );
}
