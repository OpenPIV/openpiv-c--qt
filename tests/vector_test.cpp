
// catch
#include <catch2/catch_test_macros.hpp>

// std
#include <sstream>
#include <string>

// to be tested
#include "core/vector.h"

using namespace openpiv::core;

TEST_CASE("vector_test - int_test")
{
    vector2<uint32_t> p;
    REQUIRE(p[0] == vector2<uint32_t>::value_t());
    REQUIRE(p[1] == vector2<uint32_t>::value_t());

    p = vector2<uint32_t>(1,2);
    REQUIRE(p[0] == 1);
    REQUIRE(p[1] == 2);
}

TEST_CASE("vector_test - modify_test")
{
    vector2<uint32_t> p{ 1, 2 };
    p[0] = 3;
    p[1] = 4;

    REQUIRE(p[0] == 3);
    REQUIRE(p[1] == 4);
}

TEST_CASE("vector_test - convert_test")
{
    vector2<int32_t> ip{ 1, 2 };
    vector2<double> dp{ ip };

    REQUIRE(dp[0] == 1);
    REQUIRE(dp[1] == 2);
}

TEST_CASE("vector_test - copy_constructor_test")
{
    vector2<uint32_t> p1{ 1, 2 };
    vector2<uint32_t> p2{ p1 };

    REQUIRE(p2[0] == 1);
    REQUIRE(p2[1] == 2);
}

TEST_CASE("vector_test - move_constructor_test")
{
    vector2<uint32_t> p2{ vector2<uint32_t>{ 1, 2 } };

    REQUIRE(p2[0] == 1);
    REQUIRE(p2[1] == 2);
}

TEST_CASE("vector_test - assignment_test")
{
    vector2<uint32_t> p1{ 1, 2 };
    vector2<uint32_t> p2;
    p2 = p1;

    REQUIRE(p2[0] == 1);
    REQUIRE(p2[1] == 2);
}

TEST_CASE("vector_test - move_assignment_test")
{
    vector2<uint32_t> p1{};
    p1 = vector2<uint32_t>{ 1, 2 };

    REQUIRE(p1[0] == 1);
    REQUIRE(p1[1] == 2);
}

TEST_CASE("vector_test - equality_test")
{
    vector2<uint32_t> p1{ 1, 2 };
    vector2<uint32_t> p2{ 3, 4 };
    vector2<uint32_t> p3{ 1, 2 };

    REQUIRE( p1 != p2 );
    REQUIRE( p1 == p3 );
}

TEST_CASE("vector_test - ostream_test")
{
    std::stringstream ss;
    vector2<uint32_t> p1{ 1, 2 };
    ss << p1;

    REQUIRE( ss.str() == std::string("[1,2]") );
}

TEST_CASE("vector_test - point - point")
{
    using point_t = point2<double>;
    auto v = point_t{ 20.0, 10.5 } - point_t{ 10.2, 5.1 };
    CHECK( v == vector2<double>{ 9.8, 5.4 } );
}

TEST_CASE("vector_test - point + vector")
{
    using point_t = point2<double>;
    using vector_t = vector2<double>;
    auto p = point_t{ 20.0, 10.5 } + vector_t{ 10.2, -3.1 };
    CHECK( p == point_t{ 30.2, 7.4 } );
}

TEST_CASE("vector_test - point - vector")
{
    using point_t = point2<double>;
    using vector_t = vector2<double>;
    auto p = point_t{ 20.0, 10.5 } - vector_t{ 10.2, -3.1 };
    CHECK( p == point_t{ 9.8, 13.6 } );
}

TEST_CASE("vector_test - vector + vector")
{
    using vector_t = vector2<double>;
    auto v = vector_t{ 20.0, 10.5 } + vector_t{ 10.2, -3.1 };
    CHECK( v == vector_t{ 30.2, 7.4 } );
}

TEST_CASE("vector_test - vector - vector")
{
    using vector_t = vector2<double>;
    auto v = vector_t{ 20.0, 10.5 } - vector_t{ 10.2, -3.1 };
    CHECK( v == vector_t{ 9.8, 13.6 } );
}

TEST_CASE("vector_test - vector * const")
{
    using vector_t = vector2<double>;
    auto v = vector_t{ 20.0, 10.0 } * 2.5;
    CHECK( v == vector_t{ 50.0, 25.0 } );
}

TEST_CASE("vector_test - vector / const")
{
    using vector_t = vector2<double>;
    auto v = vector_t{ 22.0, 11.0 } / 1.1;
    CHECK( v == vector_t{ 20.0, 10.0 } );
}

TEST_CASE("vector_test - split difference and move points")
{
    using vector_t = vector2<double>;
    auto v = vector_t{ 4.0, 2.0 } / 2;

    using point_t = point2<double>;
    point_t p = { 50, 50 };
    auto p1 = p - v;
    auto p2 = p + v;
    CHECK( p1 == point_t{ 48, 49 } );
    CHECK( p2 == point_t{ 52, 51 } );
}
