
// catch
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>

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
#include "algos/stats.h"
#include "loaders/image_loader.h"


using namespace std::string_literals;
using namespace Catch;
using namespace Catch::Matchers;
using namespace openpiv::core;
using namespace openpiv::algos;

TEST_CASE("image_test - int_test")
{
    g8_image im( 200, 100 );
    CHECK( im.width()  == 200 );
    CHECK( im.height() == 100 );
    CHECK( im.pixel_count() == 100*200 );
    auto d = std::distance( im.begin(), im.end() );
    CHECK( d == (decltype(d))im.pixel_count() );
}

TEST_CASE("image_test - resize_test")
{
    g8_image im;
    REQUIRE( im.width()  == 0 );
    REQUIRE( im.height() == 0 );

    im.resize( 100, 100 );
    REQUIRE( im.width()  == 100 );
    REQUIRE( im.height() == 100 );
    REQUIRE( im.pixel_count() == 100*100 );

    g_8 v{ 128 };
    fill( im, v );
    bool result = true;
    for ( uint32_t i=0; i<im.pixel_count(); ++i )
        result &= (im[i] == v);

    REQUIRE( result );
}

TEST_CASE("image_test - copy_test")
{
    g8_image im; g_8 v;
    std::tie( im, v ) = create_and_fill( {200, 100}, 128_g8 );

    g8_image im2{ im };
    REQUIRE(im.width()  == im2.width());
    REQUIRE(im.height() == im2.height());

    bool result = true;
    for ( uint32_t i=0; i<im2.pixel_count(); ++i )
        result &= (im2[i] == v);

    REQUIRE( result );
}

TEST_CASE("image_test - move_test")
{
    g8_image im; g_8 v;
    std::tie( im, v ) = create_and_fill( {200, 100}, 128_g8 );

    g8_image im2{ std::move(im) };

    bool result = true;
    for ( uint32_t i=0; i<im.pixel_count(); ++i )
        result &= (im2[i] == v);

    REQUIRE( result );
}

TEST_CASE("image_test - convert_test")
{
    g8_image im; g_8 v;
    std::tie( im, v ) = create_and_fill( {200, 200}, 128_g8 );
    gf_image im2;

    SECTION( "copy" )
    {
        im2 = gf_image{ im };
    }

    SECTION( "assign" )
    {
        im2 = im;
    }

    bool result = true;
    for ( uint32_t i=0; i<im.pixel_count(); ++i )
        result &= (im2[i] == v);

    REQUIRE( result );
}

TEST_CASE("image_test - line_out_of_bounds_test")
{
    g8_image im; g_8 v;
    std::tie( im, v ) = create_and_fill( {200, 100}, 128_g8 );

    _REQUIRE_THROWS_MATCHES( im.line(101),
                             std::range_error,
                             ContainsSubstring( "line out of range"s, CaseSensitive::No ) );
}

TEST_CASE("image_test - line_test")
{
    g8_image im; g_8 v;
    std::tie( im, v ) = create_and_fill( {2, 2}, 0_g8 );
    auto sum1 = pixel_sum(im) / im.pixel_count();
    REQUIRE(sum1 == 0);

    g_8* p = im.line(1);
    for ( size_t i=0; i<im.width(); ++i )
        *p++ = 128;

    auto sum2 = pixel_sum(im) / im.pixel_count();
    REQUIRE(sum2 == 64);
}

TEST_CASE("image_test - equality_test")
{
    g8_image im1;
    std::tie( im1, std::ignore ) = create_and_fill( {200, 100}, 128_g8 );

    g8_image im2;
    std::tie( im2, std::ignore ) = create_and_fill( {200, 100}, 128_g8 );

    REQUIRE( im1 == im2 );

    // modify a pixel
    im2[ {100, 50} ] = 100;
    REQUIRE( im1 != im2 );
}

TEST_CASE("image_test - apply_test")
{
    g8_image im; g_8 v;
    std::tie( im, v ) = create_and_fill( {200, 200}, 128_g8 );
    im[{100, 100}] = 129;

    g_8 min, max;
    std::tie(min, max) = find_image_range( im );
    auto scale{ (max == min) ? g_8::max() : g_8::max()/(max-min) };

    apply( im, [&min, &scale](auto, auto v){return scale*(v-min);} );

    std::tie(min, max) = find_image_range( im );
    REQUIRE(min == 0);
    REQUIRE(max == 255);
}


TEST_CASE("image_test - scale_test")
{
    std::ifstream is("A_00001_a.tif", std::ios::binary);
    REQUIRE(is.is_open());

    std::shared_ptr<image_loader> loader{ image_loader_registry::find(is) };
    REQUIRE(!!loader);

    g16_image im;
    loader->load( is, im );

    // scale
    g_16 min, max;
    std::tie(min, max) = find_image_range( im );
    auto scale{ (max == min) ? g_16::max() : g_16::max()/(max-min) };
    std::cout << "min: " << min << ", max: " << max << ", scale: " << scale << "\n";

    apply( im, [min, scale](auto, auto v){return scale*(v-min);} );

    std::tie(min, max) = find_image_range( im );
    std::cout << "min: " << min << ", max: " << max << "\n";

    // write data
    std::shared_ptr<image_loader> writer{ image_loader_registry::find("image/x-portable-anymap") };
    REQUIRE(!!writer);
    REQUIRE(writer->name() == "image/x-portable-anymap");

    std::fstream os( "A_00001_a.pgm", std::ios_base::trunc | std::ios_base::out | std::ios_base::binary );
    writer->save( os, im );
}

TEST_CASE("image_test - complex_conversion_test")
{
    auto [ g_im, v ] = create_and_fill< g_8 >( { 200, 200 }, 127 );
    cf_image c_im{ g_im };
    {
        auto [ min, max ] = find_image_range( c_im );
        REQUIRE( min == max );
        REQUIRE( (min == c_f{ 127, 0 }) );
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

TEST_CASE("image_test - iterator_test")
{
    g16_image im{ 100, 100 };
    std::iota( std::begin( im ), std::end( im ), 0 );

    for ( auto h : range_start_at(0).length(im.height()) )
        REQUIRE( *im.line(h) == h*100 );
}

