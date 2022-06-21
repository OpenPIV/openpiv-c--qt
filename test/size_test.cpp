
// catch
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>

// std
#include <stdexcept>
#include <sstream>
#include <string>

// to be tested
#include "core/size.h"

// local
#include "test_utils.h"

using namespace std::string_literals;
using namespace Catch;
using namespace Catch::Matchers;
using namespace openpiv::core;

TEST_CASE("size_test - default_test")
{
    size s;
    REQUIRE(s.width() == 0);
    REQUIRE(s.height() == 0);
    REQUIRE(s.area() == 0);
}

TEST_CASE("size_test - basic_test")
{
    size s(10, 20);

    REQUIRE(s.width() == 10);
    REQUIRE(s.height() == 20);
    REQUIRE(s.area() == 200);
}

TEST_CASE("size_test - copy_test")
{
    size s1(10, 20);
    size s2(s1);

    REQUIRE(s2.width() == 10);
    REQUIRE(s2.height() == 20);
}

TEST_CASE("size_test - assign_test")
{
    size s1(10, 20);
    size s2(40, 50);

    s2=s1;

    REQUIRE(s2.width() == 10);
    REQUIRE(s2.height() == 20);
}

TEST_CASE("size_test - move_test")
{
    size s1(10, 20);
    size s2(std::move(s1));

    REQUIRE(s2.width() == 10);
    REQUIRE(s2.height() == 20);
}

TEST_CASE("size_test - equality_test")
{
    size s1(10, 20);

    REQUIRE(s1 == size(10, 20));
    REQUIRE(s1 != size(20, 10));
}

TEST_CASE("size_test - add_test")
{
    size s1(10, 20);
    size s2(20, 10);

    REQUIRE(s1 + s2 == size(30, 30));
    s1 += s2;
    REQUIRE(s1 == size(30, 30));
}

TEST_CASE("size_test - subtract_test")
{
    size s1(10, 20);
    size s2(10, 20);

    REQUIRE(s1 - s2 == size(0, 0));
    s1 -= s2;
    REQUIRE(s1 == size(0, 0));

    _REQUIRE_THROWS_MATCHES( s1 -= s2,
                             std::out_of_range,
                             ContainsSubstring( "sizes cannot be negative"s, CaseSensitive::No ) );
}

TEST_CASE("size_test - maximal_test")
{
    size s(10, 20);

    REQUIRE(maximal_size(s) == size(20, 20));
}

TEST_CASE("size_test - minimal_test")
{
    size s(10, 20);

    REQUIRE(minimal_size(s) == size(10, 10));
}

TEST_CASE("size_test - transpose_test")
{
    size s(10, 20);

    REQUIRE(transpose(s) == size(20, 10));
}

TEST_CASE("size_test - components_test")
{
    size s(10, 20);
    const auto [w, h] = s.components();

    REQUIRE(w == 10);
    REQUIRE(h == 20);
}

TEST_CASE("size_test - ostream_test")
{
    std::stringstream ss;
    size s1(10, 20);
    ss << s1;

    REQUIRE( ss.str() == "[10,20]" );
}
