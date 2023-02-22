
// catch
#include <catch2/catch_test_macros.hpp>

// std
#include <sstream>
#include <string>

// to be tested
#include "core/rect.h"

using namespace openpiv::core;

TEST_CASE("rect_test - default_test")
{
    rect r;
    REQUIRE(r.bottomLeft()  == rect::point_t{});
    REQUIRE(r.bottomRight() == rect::point_t{});
    REQUIRE(r.topLeft()     == rect::point_t{});
    REQUIRE(r.topRight()    == rect::point_t{});
    REQUIRE(r.width()  == 0);
    REQUIRE(r.height() == 0);
    REQUIRE(r.area()   == 0);
}

TEST_CASE("rect_test - equality_test")
{
    REQUIRE( rect({5, 5}, {10, 10}) == rect({5, 5}, {10, 10}) );
    REQUIRE( rect({1, 5}, {10, 10}) != rect({5, 5}, {10, 10}) );
    REQUIRE( rect({5, 5}, {1, 10})  != rect({5, 5}, {10, 10}) );
}

TEST_CASE("rect_test - from_size_test")
{
    REQUIRE( rect::from_size({10,10}) == rect({}, {10, 10}) );
}

TEST_CASE("rect_test - copy_test")
{
    rect r1({5, 5}, {10, 10});
    rect r2(r1);

    REQUIRE(r1 == r2);
}

TEST_CASE("rect_test - corner_test")
{
    rect r1({5,5}, {10,10});
    REQUIRE(r1.bottomLeft()  == rect::point_t(5, 5));
    REQUIRE(r1.bottomRight() == rect::point_t(15,5));
    REQUIRE(r1.topLeft()     == rect::point_t(5,15));
    REQUIRE(r1.topRight()    == rect::point_t(15,15));

    rect r2({}, {10,10});
    REQUIRE(r2.bottomLeft()  == rect::point_t(0,0));
    REQUIRE(r2.bottomRight() == rect::point_t(10,0));
    REQUIRE(r2.topLeft()     == rect::point_t(0,10));
    REQUIRE(r2.topRight()    == rect::point_t(10,10));
}

TEST_CASE("rect_test - within_test")
{
    rect r({5,5}, {10,10});
    REQUIRE( r.within( rect( {5, 5}, {10, 10})) );
    REQUIRE( r.within( rect( {0, 0}, {20, 20})) );
    REQUIRE_FALSE( r.within( rect( {6, 0}, {8, 8})) );
    REQUIRE_FALSE( r.within( rect( {0, 6}, {8, 8})) );
    REQUIRE_FALSE( r.within( rect( {0, 0}, {10, 20})) );
    REQUIRE_FALSE( r.within( rect( {0, 0}, {20, 10})) );
}

TEST_CASE("rect_test - contains_test")
{
    rect r({5, 5}, {10, 10});
    REQUIRE( r.contains( rect( {5, 5}, {10, 10})) );
    REQUIRE( r.contains( rect( {6, 6}, {8, 8})) );
    REQUIRE_FALSE( r.contains( rect( {4, 6}, {8, 8})) );
    REQUIRE_FALSE( r.contains( rect( {6, 4}, {8, 8})) );
    REQUIRE_FALSE( r.contains( rect( {6, 6}, {8, 10})) );
    REQUIRE_FALSE( r.contains( rect( {6, 6}, {10, 8})) );
}

TEST_CASE("rect_test - midpoint_test")
{
    rect r({5, 5}, {10, 10});
    REQUIRE( r.midpoint() == rect::point_t(10, 10) );
}

TEST_CASE("rect_test - edge_test")
{
    rect r({5, 6}, {10, 11});
    CHECK( r.left() == 5 );
    CHECK( r.right() == 15 );
    CHECK( r.bottom() == 6 );
    CHECK( r.top() == 17 );
}

TEST_CASE("rect_test - dilate_test")
{
    rect r( {0, 0}, {10, 20} );
    CHECK( r.dilate(2) == rect( {-2, -2}, {14, 24} ) );
    CHECK( r.dilate(-2) == rect( {2, 2}, {6, 16} ) );
    CHECK( r.dilate(1.0) == rect( {0, 0}, {10, 20} ) );
    CHECK( r.dilate(1.2) == rect( {-1, -2}, {12, 24} ) );
    CHECK( rect( {}, {32, 32} ).dilate(0.5) == rect( {8, 8}, {16, 16} ) );
}

TEST_CASE("rect_test - ostream_test")
{
    std::stringstream ss;
    rect r( {1, 1}, {20, 20} );
    ss << r;

    REQUIRE( ss.str() == "(1,1) -> [20,20]" );
}
