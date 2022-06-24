
// catch
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>

// std
#include <sstream>
#include <string>
#include <tuple>

// local
#include "test_utils.h"

// to be tested
#include "core/image_view.h"
#include "core/image_utils.h"
#include "algos/stats.h"

using namespace std::string_literals;
using namespace Catch;
using namespace Catch::Matchers;
using namespace openpiv::core;
using namespace openpiv::algos;
namespace logger = openpiv::core::logger;

static auto init = [](){
    return logger::Logger::instance().add_sink(
        [](logger::Level, const std::string& m) -> bool
        {
            std::cout << m << "\n";
            return true;
        });
}();


TEST_CASE("image_view_test - basic_construction_test")
{
    g8_image im; g_8 v;
    std::tie( im, v ) = create_and_fill( {200, 200}, 0_g8 );

    auto iv = create_image_view( im, rect::from_size({100, 100}) );
    REQUIRE(iv.width() == 100);
    REQUIRE(iv.height() == 100);
    REQUIRE(iv.pixel_count() == 10000);
}

TEST_CASE("image_view_test - resize_test")
{
    g8_image im; g_8 v;
    std::tie( im, v ) = create_and_fill( {200, 200}, 128_g8 );
    auto iv = create_image_view( im, rect::from_size({100, 100}) );

    iv.resize(150, 150);
    REQUIRE(iv.width() == 150);
    REQUIRE(iv.height() == 150);
    REQUIRE(iv.pixel_count() == 150*150);

    bool result = true;
    for ( uint32_t i=0; i<iv.pixel_count(); ++i )
        result &= (iv[i] == v);

    REQUIRE( result == true );
}

TEST_CASE("image_view_test - resize_failure_test")
{
    g8_image im; g_8 v;
    std::tie( im, v ) = create_and_fill( {200, 200}, 128_g8 );
    auto iv = create_image_view( im, rect::from_size({100, 100}) );

    _REQUIRE_THROWS_MATCHES(
        iv.resize(300, 300),
        std::out_of_range,
        ContainsSubstring( "not contained within image"s, CaseSensitive::No ) );
}

TEST_CASE("image_view_test - copy_test")
{
    g8_image im; g_8 v;
    std::tie( im, v ) = create_and_fill( {200, 200}, 0_g8 );

    auto iv1 = create_image_view( im, rect::from_size({100, 100}) );
    auto iv2{ iv1 };
    REQUIRE(iv1.width() == iv2.width());
    REQUIRE(iv1.height() == iv2.height());
    REQUIRE(iv1.pixel_count() == iv2.pixel_count());

    // compare pointers not values as we want to check we're getting
    // same underlying data from im
    REQUIRE(iv1.line(0) == iv2.line(0));
}

TEST_CASE("image_view_test - move_test")
{
    g8_image im; g_8 v;
    std::tie( im, v ) = create_and_fill( {200, 200}, 0_g8 );

    auto iv1 = create_image_view( im, rect::from_size({100, 100}) );
    auto iv_tmp = create_image_view( im, rect::from_size({100, 100}) );
    auto iv2{ std::move(iv_tmp) };
    REQUIRE(iv1.width() == iv2.width());
    REQUIRE(iv1.height() == iv2.height());
    REQUIRE(iv1.pixel_count() == iv2.pixel_count());

    // compare pointers not values as we want to check we're getting
    // same underlying data from im
    REQUIRE(iv1.line(0) == iv2.line(0));
}

TEST_CASE("image_view_test - equality_test")
{
    g8_image im;
    std::tie( im, std::ignore ) = create_and_fill( {200, 200}, 0_g8 );

    auto iv1 = create_image_view( im, rect::from_size({100, 100}) );
    auto iv2 = create_image_view( im, rect::from_size({100, 100}) );
    REQUIRE(iv1 == iv2);
}

TEST_CASE("image_view_test - size_inequality_test")
{
    g8_image im;
    std::tie( im, std::ignore ) = create_and_fill( {200, 200}, 0_g8 );

    auto iv1 = create_image_view( im, rect::from_size({100, 101}) );
    auto iv2 = create_image_view( im, rect::from_size({100, 100}) );
    REQUIRE(iv1 != iv2);
}

TEST_CASE("image_view_test - origin_inequality_test")
{
    g8_image im;
    std::tie( im, std::ignore ) = create_and_fill( {200, 200}, 0_g8 );

    auto iv1 = create_image_view( im, rect( {1, 0}, {100, 100}) );
    auto iv2 = create_image_view( im, rect( {0, 0}, {100, 100}) );
    REQUIRE(iv1 != iv2);
}

TEST_CASE("image_view_test - image_inequality_test")
{
    g8_image im;
    std::tie( im, std::ignore ) = create_and_fill( {200, 200}, 0_g8 );
    g8_image im2;
    std::tie( im2, std::ignore ) = create_and_fill( {200, 200}, 0_g8 );

    auto iv1 = create_image_view( im,  rect( {0, 0}, {100, 100}) );
    auto iv2 = create_image_view( im2, rect( {0, 0}, {100, 100}) );
    REQUIRE(iv1 != iv2);
}

TEST_CASE("image_view_test - view_test")
{
    g8_image im; g_8 v;
    std::tie( im, v ) = create_and_fill( {200, 200}, 0_g8 );

    auto iv = create_image_view( im, rect::from_size({100, 100}) );
    REQUIRE( pixel_sum(im) == 0 );
    REQUIRE( pixel_sum(iv) == 0 );

    im[{}] = 255;
    REQUIRE( pixel_sum(im) == pixel_sum(iv) );
}

TEST_CASE("image_view_test - offset_test")
{
    g8_image im; g_8 v;
    std::tie( im, v ) = create_and_fill( {200, 200}, 0_g8 );
    im[ {10, 20} ] = 255;

    auto iv = create_image_view( im, rect( {10, 20}, {100, 100} ) );
    REQUIRE( (iv[ {0, 0} ] == im[ {10, 20} ]) );
    REQUIRE( im.rect().bottomLeft() == rect::point_t{} );
    REQUIRE( iv.rect().bottomLeft() == rect::point_t{10, 20} );
}

TEST_CASE("image_view_test - line_test")
{
    g8_image im; g_8 v;
    std::tie( im, v ) = create_and_fill( {200, 200}, 127_g8 );

    // set a single pixel
    im[ {20, 20} ] = 255;

    auto iv = create_image_view( im, rect( {10, 10}, {100, 100} ) );

    // check line pointers all match
    for (decltype(iv.height()) h=0; h<iv.height(); ++h)
    {
        REQUIRE( iv.line(h) == im.line(h + 10) + 10 );
        REQUIRE( iv.line(h) + iv.width() - 1 == im.line(h + 10) + 10 + iv.width() - 1 );
        REQUIRE( *(iv.line(h) + iv.width() - 1) == *(im.line(h + 10) + 10 + iv.width() - 1) );
    }

    REQUIRE( *(iv.line(10) + 10) == *(im.line(20) + 20) );
}

TEST_CASE("image_view_test - image_from_image_view_test")
{
    g8_image im; g_8 v;
    std::tie( im, v ) = create_and_fill( {200, 200}, 1_g8 );

    auto iv = create_image_view( im, {{10, 20}, {100, 50}} );
    gf_image im2{ iv };
    REQUIRE( im2.width() == iv.width() );
    REQUIRE( im2.height() == iv.height() );
    REQUIRE( pixel_sum(im2) == 5000 );
    REQUIRE( iv.rect().bottomLeft() == rect::point_t{10, 20} );
    REQUIRE( im2.rect().bottomLeft() == rect::point_t{10, 20} );
}

TEST_CASE("image_view_test - image_view_from_image_view_test")
{
    g8_image im; g_8 v;
    std::tie( im, v ) = create_and_fill( {200, 200}, 1_g8 );

    auto iv1 = create_image_view( im, { {10, 20}, {100, 50} } );
    auto iv2 = create_image_view( iv1, rect::from_size(iv1.size()) );

    REQUIRE( iv1.rect().bottomLeft() == rect::point_t{10, 20} );
    REQUIRE( iv2.rect().bottomLeft() == rect::point_t{10, 20} );
    REQUIRE( iv2.width() == iv1.width() );
    REQUIRE( iv2.height() == iv1.height() );
    REQUIRE( pixel_sum(iv1) == 5000 );
    REQUIRE( pixel_sum(iv2) == 5000 );

    // make an inner image_view, write and check correct data is modified
    // was (0, 0) -> (100, 50), should now be (10, 10) -> (80, 30)
    auto iv3 = create_image_view( iv1, { {10, 10}, {80, 30} } );
    fill(iv3, 3_g8);
    REQUIRE( pixel_sum(iv3) == 80*30*3 );
    REQUIRE( iv3.rect().bottomLeft() == rect::point_t{20, 30} );

    // check with a new image view
    auto iv4 = create_image_view( im, { {20, 30}, {80, 30} } );
    REQUIRE( pixel_sum(iv4) == 80*30*3 );
}

TEST_CASE("image_view_test - out_of_bounds_test")
{
    g8_image im; g_8 v;
    std::tie( im, v ) = create_and_fill( {200, 200}, 0_g8 );

    _REQUIRE_THROWS_MATCHES(
        create_image_view(
            im,
            rect::from_size({250, 250}) ),
        std::out_of_range,
        ContainsSubstring( "not contained within image"s, CaseSensitive::No ) );
}

TEST_CASE("image_view_test - convertion_test")
{
    g8_image im; g_8 v;
    std::tie( im, v ) = create_and_fill( {200, 200}, 127_g8 );

    auto view = create_image_view( im, { {50, 50}, {100, 100} } );
    cf_image c( view.size() );
    c = view;

    REQUIRE( c.width() == view.width() );
    REQUIRE( c.height() == view.height() );
    REQUIRE( c.pixel_count() == view.pixel_count() );
    bool result = true;
    for ( uint32_t i=0; i<c.pixel_count(); ++i )
        result &= (c[i] == c_f{ v });

    REQUIRE( result == true );
}

TEST_CASE("image_view_test - fill_test")
{
    g16_image im; g_16 v;
    std::tie( im, v ) = create_and_fill( {200, 200}, 127_g16 );

    auto view = create_image_view( im, { {50, 50}, {100, 100} } );
    fill( view, 255_g16 );

    REQUIRE( pixel_sum( view ) == 100*100*255 );
    REQUIRE( pixel_sum( im ) == 100*100*128 + 200*200*127 );

    g_16 min, max;
    std::tie(min, max) = find_image_range( im );
    auto scale{ (max == min) ? g_16::max() : g_16::max()/(max-min) };

    apply( im, [&min, &scale](auto, auto v){return scale*(v-min);} );
    REQUIRE( save_to_file( "view_fill_test.pgm", im ) );
}

