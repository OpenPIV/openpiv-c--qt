
// catch
#include <catch2/catch_test_macros.hpp>

// local
#include "test_utils.h"

// to be tested
#include "core/pixel_types.h"
#include "algos/stats.h"

using namespace openpiv::core;
using namespace openpiv::algos;

TEST_CASE("image_stats_test - find_image_range_uniform_test")
{
    g8_image im; g_8 v;
    std::tie( im, v ) = create_and_fill( {200, 200}, 0_g8 );

    auto [min, max] = find_image_range( im );
    REQUIRE(min == 0);
    REQUIRE(max == 0);
}

TEST_CASE("image_stats_test - find_image_range_max_test")
{
    g8_image im; g_8 v;
    std::tie( im, v ) = create_and_fill( {200, 200}, 0_g8 );

    im[{ 50, 50 }] = 255;
    auto [min, max] = find_image_range( im );
    REQUIRE(min == 0);
    REQUIRE(max == 255);
}

TEST_CASE("image_stats_test - find_image_range_min_test")
{
    g8_image im; g_8 v;
    std::tie( im, v ) = create_and_fill( {200, 200}, 255_g8 );

    im[{ 50, 50 }] = 0;
    auto [min, max] = find_image_range( im );
    REQUIRE(min == 0);
    REQUIRE(max == 255);
}

TEST_CASE("image_stats_test - find_image_range_min_max_test")
{
    g8_image im; g_8 v;
    std::tie( im, v ) = create_and_fill( {200, 200}, 128_g8 );

    im[{ 50, 50 }] = 0;
    im[{ 150, 150 }] = 255;
    auto [min, max] = find_image_range( im );
    REQUIRE(min == 0);
    REQUIRE(max == 255);
}

