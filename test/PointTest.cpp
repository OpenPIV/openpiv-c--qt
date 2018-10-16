
// catch
#include "catch.hpp"

// std
#include <sstream>
#include <string>

// to be tested
#include "Point.h"

TEST_CASE("PointTest - IntTest")
{
    Point2<uint32_t> p;
    REQUIRE(p[0] == Point2<uint32_t>::type());
    REQUIRE(p[1] == Point2<uint32_t>::type());

    p = Point2<uint32_t>(1,2);
    REQUIRE(p[0] == 1);
    REQUIRE(p[1] == 2);
}

TEST_CASE("PointTest - ModifyTest")
{
    Point2<uint32_t> p{ 1, 2 };
    p[0] = 3;
    p[1] = 4;

    REQUIRE(p[0] == 3);
    REQUIRE(p[1] == 4);
}

TEST_CASE("PointTest - ConvertTest")
{
    Point2<int32_t> ip{ 1, 2 };
    Point2<double> dp{ ip };

    REQUIRE(dp[0] == 1);
    REQUIRE(dp[1] == 2);
}

TEST_CASE("PointTest - CopyConstructorTest")
{
    Point2<uint32_t> p1{ 1, 2 };
    Point2<uint32_t> p2{ p1 };

    REQUIRE(p2[0] == 1);
    REQUIRE(p2[1] == 2);
}

TEST_CASE("PointTest - MoveConstructorTest")
{
    Point2<uint32_t> p2{ Point2<uint32_t>{ 1, 2 } };

    REQUIRE(p2[0] == 1);
    REQUIRE(p2[1] == 2);
}

TEST_CASE("PointTest - AssignmentTest")
{
    Point2<uint32_t> p1{ 1, 2 };
    Point2<uint32_t> p2;
    p2 = p1;

    REQUIRE(p2[0] == 1);
    REQUIRE(p2[1] == 2);
}

TEST_CASE("PointTest - MoveAssignmentTest")
{
    Point2<uint32_t> p1{};
    p1 = Point2<uint32_t>{ 1, 2 };

    REQUIRE(p1[0] == 1);
    REQUIRE(p1[1] == 2);
}

TEST_CASE("PointTest - EqualityTest")
{
    Point2<uint32_t> p1{ 1, 2 };
    Point2<uint32_t> p2{ 3, 4 };
    Point2<uint32_t> p3{ 1, 2 };

    REQUIRE( p1 != p2 );
    REQUIRE( p1 == p3 );
}

TEST_CASE("PointTest - OStreamTest")
{
    std::stringstream ss;
    Point2<uint32_t> p1{ 1, 2 };
    ss << p1;

    REQUIRE( ss.str() == std::string("(1,2)") );
}
