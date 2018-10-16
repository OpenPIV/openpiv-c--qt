
// catch
#include "catch.hpp"

// std
#include <stdexcept>
#include <sstream>
#include <string>

// to be tested
#include "Size.h"

// local
#include "TestUtils.h"

using namespace Catch;

TEST_CASE("SizeTest - DefaultTest")
{
    Size s;
    REQUIRE(s.width() == 0);
    REQUIRE(s.height() == 0);
    REQUIRE(s.area() == 0);
}

TEST_CASE("SizeTest - BasicTest")
{
    Size s(10, 20);

    REQUIRE(s.width() == 10);
    REQUIRE(s.height() == 20);
    REQUIRE(s.area() == 200);
}

TEST_CASE("SizeTest - CopyTest")
{
    Size s1(10, 20);
    Size s2(s1);

    REQUIRE(s2.width() == 10);
    REQUIRE(s2.height() == 20);
}

TEST_CASE("SizeTest - AssignTest")
{
    Size s1(10, 20);
    Size s2(40, 50);

    s2=s1;

    REQUIRE(s2.width() == 10);
    REQUIRE(s2.height() == 20);
}

TEST_CASE("SizeTest - MoveTest")
{
    Size s1(10, 20);
    Size s2(std::move(s1));

    REQUIRE(s2.width() == 10);
    REQUIRE(s2.height() == 20);
}

TEST_CASE("SizeTest - EqualityTest")
{
    Size s1(10, 20);

    REQUIRE(s1 == Size(10, 20));
    REQUIRE(s1 != Size(20, 10));
}

TEST_CASE("SizeTest - AddTest")
{
    Size s1(10, 20);
    Size s2(20, 10);

    REQUIRE(s1 + s2 == Size(30, 30));
    s1 += s2;
    REQUIRE(s1 == Size(30, 30));
}

TEST_CASE("SizeTest - SubtractTest")
{
    Size s1(10, 20);
    Size s2(10, 20);

    REQUIRE(s1 - s2 == Size(0, 0));
    s1 -= s2;
    REQUIRE(s1 == Size(0, 0));

    _REQUIRE_THROWS_MATCHES( s1 -= s2, std::out_of_range, Contains( "sizes cannot be negative" ) );
}

TEST_CASE("SizeTest - MaximalTest")
{
    Size s(10, 20);

    REQUIRE(maximal_size(s) == Size(20, 20));
}

TEST_CASE("SizeTest - MinimalTest")
{
    Size s(10, 20);

    REQUIRE(minimal_size(s) == Size(10, 10));
}

TEST_CASE("SizeTest - TransposeTest")
{
    Size s(10, 20);

    REQUIRE(transpose(s) == Size(20, 10));
}

TEST_CASE("SizeTest - ComponentsTest")
{
    Size s(10, 20);
    const auto [w, h] = s.components();

    REQUIRE(w == 10);
    REQUIRE(h == 20);
}

TEST_CASE("SizeTest - OStreamTest")
{
    std::stringstream ss;
    Size s1(10, 20);
    ss << s1;

    REQUIRE( ss.str() == "[10,20]" );
}
