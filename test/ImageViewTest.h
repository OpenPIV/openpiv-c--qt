
// catch
#include "catch.hpp"

// std
#include <sstream>
#include <string>
#include <tuple>

// local
#include "TestUtils.h"

// to be tested
#include "ImageView.h"
#include "ImageUtils.h"

using namespace Catch;

TEST_CASE("ImageViewTest - BasicConstructionTest")
{
    G8Image im; uint8_t v;
    std::tie( im, v ) = createAndFill( Size( 200, 200 ), 0);

    auto iv = createImageView( im, Rect::fromSize(Size(100, 100)) );
    REQUIRE(iv.width() == 100);
    REQUIRE(iv.height() == 100);
    REQUIRE(iv.pixel_count() == 10000);
}

TEST_CASE("ImageViewTest - ResizeTest")
{
    G8Image im; uint8_t v;
    std::tie( im, v ) = createAndFill( Size( 200, 200 ), 128);
    auto iv = createImageView( im, Rect::fromSize(Size(100, 100)) );

    iv.resize(150, 150);
    REQUIRE(iv.width() == 150);
    REQUIRE(iv.height() == 150);
    REQUIRE(iv.pixel_count() == 150*150);

    bool result = true;
    for ( uint32_t i=0; i<iv.pixel_count(); ++i )
        result &= (iv[i] == v);

    REQUIRE( result == true );
}

TEST_CASE("ImageViewTest - ResizeFailureTest")
{
    G8Image im; uint8_t v;
    std::tie( im, v ) = createAndFill( Size( 200, 200 ), 128);
    auto iv = createImageView( im, Rect::fromSize(Size(100, 100)) );

    _REQUIRE_THROWS_MATCHES(
        iv.resize(300, 300),
        std::out_of_range,
        Contains( "not contained within image" ) );
}

TEST_CASE("ImageViewTest - CopyTest")
{
    G8Image im; uint8_t v;
    std::tie( im, v ) = createAndFill( Size( 200, 200 ), 0);

    auto iv1 = createImageView( im, Rect::fromSize(Size(100, 100)) );
    auto iv2{ iv1 };
    REQUIRE(iv1.width() == iv2.width());
    REQUIRE(iv1.height() == iv2.height());
    REQUIRE(iv1.pixel_count() == iv2.pixel_count());

    // compare pointers not values as we want to check we're getting
    // same underlying data from im
    REQUIRE(iv1.line(0) == iv2.line(0));
}

TEST_CASE("ImageViewTest - MoveTest")
{
    G8Image im; uint8_t v;
    std::tie( im, v ) = createAndFill( Size( 200, 200 ), 0);

    auto iv1 = createImageView( im, Rect::fromSize(Size(100, 100)) );
    auto iv_tmp = createImageView( im, Rect::fromSize(Size(100, 100)) );
    auto iv2{ std::move(iv_tmp) };
    REQUIRE(iv1.width() == iv2.width());
    REQUIRE(iv1.height() == iv2.height());
    REQUIRE(iv1.pixel_count() == iv2.pixel_count());

    // compare pointers not values as we want to check we're getting
    // same underlying data from im
    REQUIRE(iv1.line(0) == iv2.line(0));
}

TEST_CASE("ImageViewTest - EqualityTest")
{
    G8Image im;
    std::tie( im, std::ignore ) = createAndFill( Size( 200, 200 ), 0);

    auto iv1 = createImageView( im, Rect::fromSize(Size(100, 100)) );
    auto iv2 = createImageView( im, Rect::fromSize(Size(100, 100)) );
    REQUIRE(iv1 == iv2);
}

TEST_CASE("ImageViewTest - SizeInequalityTest")
{
    G8Image im;
    std::tie( im, std::ignore ) = createAndFill( Size( 200, 200 ), 0);

    auto iv1 = createImageView( im, Rect::fromSize(Size(100, 101)) );
    auto iv2 = createImageView( im, Rect::fromSize(Size(100, 100)) );
    REQUIRE(iv1 != iv2);
}

TEST_CASE("ImageViewTest - OriginInequalityTest")
{
    G8Image im;
    std::tie( im, std::ignore ) = createAndFill( Size( 200, 200 ), 0);

    auto iv1 = createImageView( im, Rect( {1, 0}, {100, 100}) );
    auto iv2 = createImageView( im, Rect( {0, 0}, {100, 100}) );
    REQUIRE(iv1 != iv2);
}

TEST_CASE("ImageViewTest - ImageInequalityTest")
{
    G8Image im;
    std::tie( im, std::ignore ) = createAndFill( Size( 200, 200 ), 0);
    G8Image im2;
    std::tie( im2, std::ignore ) = createAndFill( Size( 200, 200 ), 0);

    auto iv1 = createImageView( im,  Rect( {0, 0}, {100, 100}) );
    auto iv2 = createImageView( im2, Rect( {0, 0}, {100, 100}) );
    REQUIRE(iv1 != iv2);
}

TEST_CASE("ImageViewTest - ViewTest")
{
    G8Image im; uint8_t v;
    std::tie( im, v ) = createAndFill( Size( 200, 200 ), 0);

    auto iv = createImageView( im, Rect::fromSize(Size(100, 100)) );
    REQUIRE( pixel_sum(im) == 0 );
    REQUIRE( pixel_sum(iv) == 0 );

    im[Point2<uint32_t>()] = 255;
    REQUIRE( pixel_sum(im) == pixel_sum(iv) );
}

TEST_CASE("ImageViewTest - OffsetTest")
{
    G8Image im; uint8_t v;
    std::tie( im, v ) = createAndFill( Size( 200, 200 ), 0);
    im[ Point2<uint32_t>{20, 20} ] = 255;

    auto iv = createImageView( im, Rect( {20, 20}, {100, 100} ) );
    REQUIRE( iv[ Point2<uint32_t>(0, 0) ] == im[ Point2<uint32_t>(20, 20) ] );
}

TEST_CASE("ImageViewTest - LineTest")
{
    G8Image im; uint8_t v;
    std::tie( im, v ) = createAndFill( Size( 200, 200 ), 0);
    im[ Point2<uint32_t>{20, 20} ] = 255;

    auto iv = createImageView( im, Rect( {10, 10}, {100, 100} ) );
    REQUIRE( *(iv.line(10) + 10) == *(im.line(20) + 20) );
}

TEST_CASE("ImageViewTest - ImageFromImageViewTest")
{
    G8Image im; uint8_t v;
    std::tie( im, v ) = createAndFill( Size( 200, 200 ), 1);

    auto iv = createImageView( im, Rect::fromSize(Size(100, 50)) );
    GFImage im2{ iv };
    REQUIRE( im2.width() == iv.width() );
    REQUIRE( im2.height() == iv.height() );
    REQUIRE( pixel_sum(im2) == 5000 );
}

TEST_CASE("ImageViewTest - OutOfBoundsTest")
{
    G8Image im; uint8_t v;
    std::tie( im, v ) = createAndFill( Size( 200, 200 ), 0);

    _REQUIRE_THROWS_MATCHES(
        createImageView( im,
                         Rect::fromSize(Size(250, 250)) ),
        std::out_of_range,
        Contains( "not contained within image" ) );
}

