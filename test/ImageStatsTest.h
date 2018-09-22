
// catch
#include "catch.hpp"

// local
#include "TestUtils.h"

// to be tested
#include "ImageStats.h"
#include "PixelTypes.h"

TEST_CASE("ImageStatsTest - FindImageRangeUniformTest")
{
    G8Image im; G8 v;
    std::tie( im, v ) = createAndFill( Size( 200, 200 ), 0_g8 );

    auto [min, max] = findImageRange( im );
    REQUIRE(min == 0);
    REQUIRE(max == 0);
}

TEST_CASE("ImageStatsTest - FindImageRangeMaxTest")
{
    G8Image im; G8 v;
    std::tie( im, v ) = createAndFill( Size( 200, 200 ), 0_g8 );

    im[Point2<uint32_t>{ 50, 50 }] = 255;
    auto [min, max] = findImageRange( im );
    REQUIRE(min == 0);
    REQUIRE(max == 255);
}

TEST_CASE("ImageStatsTest - FindImageRangeMinTest")
{
    G8Image im; G8 v;
    std::tie( im, v ) = createAndFill( Size( 200, 200 ), 255_g8 );

    im[Point2<uint32_t>{ 50, 50 }] = 0;
    auto [min, max] = findImageRange( im );
    REQUIRE(min == 0);
    REQUIRE(max == 255);
}

TEST_CASE("ImageStatsTest - FindImageRangeMinMaxTest")
{
    G8Image im; G8 v;
    std::tie( im, v ) = createAndFill( Size( 200, 200 ), 128_g8 );

    im[Point2<uint32_t>{ 50, 50 }] = 0;
    im[Point2<uint32_t>{ 150, 150 }] = 255;
    auto [min, max] = findImageRange( im );
    REQUIRE(min == 0);
    REQUIRE(max == 255);
}

