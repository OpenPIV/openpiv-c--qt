
// catch
#include <catch2/catch_test_macros.hpp>

// std
#include <sstream>
#include <string>

// to be tested
#include "core/point.h"

using namespace openpiv::core;

TEST_CASE("point_test - int_test")
{
    point2<uint32_t> p;
    REQUIRE(p[0] == point2<uint32_t>::value_t());
    REQUIRE(p[1] == point2<uint32_t>::value_t());

    p = point2<uint32_t>(1,2);
    REQUIRE(p[0] == 1);
    REQUIRE(p[1] == 2);
}

TEST_CASE("point_test - modify_test")
{
    point2<uint32_t> p{ 1, 2 };
    p[0] = 3;
    p[1] = 4;

    REQUIRE(p[0] == 3);
    REQUIRE(p[1] == 4);
}

TEST_CASE("point_test - convert_test")
{
    point2<int32_t> ip{ 1, 2 };
    point2<double> dp{ ip };

    REQUIRE(dp[0] == 1);
    REQUIRE(dp[1] == 2);
}

TEST_CASE("point_test - copy_constructor_test")
{
    point2<uint32_t> p1{ 1, 2 };
    point2<uint32_t> p2{ p1 };

    REQUIRE(p2[0] == 1);
    REQUIRE(p2[1] == 2);
}

TEST_CASE("point_test - move_constructor_test")
{
    point2<uint32_t> p2{ point2<uint32_t>{ 1, 2 } };

    REQUIRE(p2[0] == 1);
    REQUIRE(p2[1] == 2);
}

TEST_CASE("point_test - assignment_test")
{
    point2<uint32_t> p1{ 1, 2 };
    point2<uint32_t> p2;
    p2 = p1;

    REQUIRE(p2[0] == 1);
    REQUIRE(p2[1] == 2);
}

TEST_CASE("point_test - move_assignment_test")
{
    point2<uint32_t> p1{};
    p1 = point2<uint32_t>{ 1, 2 };

    REQUIRE(p1[0] == 1);
    REQUIRE(p1[1] == 2);
}

TEST_CASE("point_test - equality_test")
{
    point2<uint32_t> p1{ 1, 2 };
    point2<uint32_t> p2{ 3, 4 };
    point2<uint32_t> p3{ 1, 2 };

    REQUIRE( p1 != p2 );
    REQUIRE( p1 == p3 );
}

TEST_CASE("point_test - ostream_test")
{
    std::stringstream ss;
    point2<uint32_t> p1{ 1, 2 };
    ss << p1;

    REQUIRE( ss.str() == std::string("(1,2)") );
}
