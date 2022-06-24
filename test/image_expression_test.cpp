
// catch
#include <catch2/catch_test_macros.hpp>

// std
#include <sstream>
#include <string>
#include <tuple>

// local
#include "test_utils.h"

// to be tested
#include "core/image_expression.h"
#include "core/image.h"
#include "core/image_utils.h"
#include "algos/stats.h"

using namespace openpiv::core;
using namespace openpiv::algos;

class T_
{
public:
    using type = uint32_t;
    constexpr type operator[](size_t) const { return type{1}; }
};


TEST_CASE("image_expression_test - image_expression_input_test")
{
    auto ie = image_expression<plus_op<T_::type>, T_, T_>{ T_{}, T_{} };
    REQUIRE( is_ie_inputtype_v<decltype(ie)> );
    REQUIRE( is_imageexpression_v<decltype(ie)> );
}

TEST_CASE("image_expression_test - image_input_test")
{
    using type = g8_image;
    REQUIRE( is_ie_inputtype_v<type> );
    REQUIRE( !is_imageexpression_v<type> );
}

TEST_CASE("image_expression_test - image_view_input_test")
{
    using type = image_view<g_8>;
    REQUIRE( is_ie_inputtype_v<type> );
    REQUIRE( !is_imageexpression_v<type> );
}

TEST_CASE("image_expression_test - negative_input_test")
{
    using type = uint32_t;
    REQUIRE( !is_ie_inputtype_v<type> );
    REQUIRE( !is_imageexpression_v<type> );
}

TEST_CASE("image_expression_test - add_const_test")
{
    g8_image im; g_8 v;
    std::tie( im, v ) = create_and_fill( size( 200, 100 ), 128_g8 );

    g8_image im2( 200, 100 );
    im2 = im + 1_g8;
    auto [min, max] = find_image_range( im2 );
    REQUIRE(min == 129);
    REQUIRE(max == 129);
}

TEST_CASE("image_expression_test - conjugate_test")
{
    cf_image im;
    std::tie( im, std::ignore ) = create_and_fill( size( 200, 200 ), c_f{ 0, 1 } );

    cf_image im2( 200, 200 );
    im2 = conj(im);

    for ( decltype(im.pixel_count()) i=0; i<im.pixel_count(); ++i )
        REQUIRE( im[i] == im2[i].conj() );
}

TEST_CASE("image_expression_test - conjugate_multiply_test")
{
    cf_image im;
    std::tie( im, std::ignore ) = create_and_fill( size( 200, 200 ), c_f{ 2, 1 } );

    cf_image result( 200, 200 );
    result = im * conj( im );

    for ( decltype(im.pixel_count()) i=0; i<im.pixel_count(); ++i )
        REQUIRE( (result[i] == c_f{ 5, 0 }) );
}

TEST_CASE("image_expression_test - write_to_self_test")
{
    cf_image im;
    std::tie( im, std::ignore ) = create_and_fill( size( 200, 200 ), c_f{ 2, 1 } );

    im = im * conj( im );

    for ( decltype(im.pixel_count()) i=0; i<im.pixel_count(); ++i )
        REQUIRE( (im[i] == c_f{ 5, 0 }) );
}

TEST_CASE("image_expression_test - add_image_test")
{
    g8_image im1; g_8 v1;
    std::tie( im1, v1 ) = create_and_fill( size( 200, 100 ), 128_g8 );

    g8_image im2; g_8 v2;
    std::tie( im2, v2 ) = create_and_fill( size( 200, 100 ), 127_g8 );

    g8_image im3( 200, 100 );
    im3 = im1 + im2;

    auto [min, max] = find_image_range( im3 );
    REQUIRE(min == 255);
    REQUIRE(max == 255);
}

TEST_CASE("image_expression_test - add_image_const_test")
{
    g8_image im1; g_8 v1;
    std::tie( im1, v1 ) = create_and_fill( size( 200, 100 ), 127_g8 );

    g8_image im2; g_8 v2;
    std::tie( im2, v2 ) = create_and_fill( size( 200, 100 ), 127_g8 );

    g8_image im3( 200, 100 );
    im3 = im1 + im2 + 1_g8;

    auto [min, max] = find_image_range( im3 );
    REQUIRE(min == 255);
    REQUIRE(max == 255);
}

TEST_CASE("image_expression_test - scale_test")
{
    std::ifstream is("A_00001_a.tif", std::ios::binary);
    REQUIRE(is.is_open());

    std::shared_ptr<image_loader> loader{ image_loader_registry::find(is) };
    REQUIRE(!!loader);

    g16_image im;
    loader->load( is, im );

    // scale
    auto [min, max] = find_image_range( im );
    auto scale{ (max == min) ? g_16::max() : g_16::max()/(max-min) };
    std::cout << "min: " << min << ", max: " << max << ", scale: " << scale << "\n";

    im = g_16{ scale } * im - g_16{ min };

    std::tie(min, max) = find_image_range( im );
    std::cout << "min: " << min << ", max: " << max << "\n";

    // write data
    std::shared_ptr<image_loader> writer{ image_loader_registry::find("image/x-portable-anymap") };
    REQUIRE(!!writer);
    REQUIRE(writer->name() == std::string("image/x-portable-anymap"));

    std::fstream os( "A_00001_a.pgm", std::ios_base::trunc | std::ios_base::out | std::ios_base::binary );
    writer->save( os, im );
}
