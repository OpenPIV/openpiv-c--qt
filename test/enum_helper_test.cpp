
// catch
#include <catch2/catch.hpp>

// std
#include <sstream>

// local
#include "test_utils.h"

// to be tested
#include "core/enum_helper.h"

using namespace Catch;
using namespace openpiv::core;

enum class E {
    one = 1,
    two = 2,
    three = 3,
    four = 4
};
DECLARE_ENUM_HELPER( E, {
        { E::one, "one" },
        { E::two, "two" },
        { E::four, "four" }
    } )

TEST_CASE("enum_helper_test - default to_string")
{
    std::string s;
    s += to_string(E::one) + " " + to_string(E::two);
    REQUIRE( s == "one two" );
}

TEST_CASE("enum_helper_test - default")
{
    std::stringstream ss;
    ss << E::one << " " << E::two;
    REQUIRE( ss.str() == "one two" );
}

TEST_CASE("enum_helper_test - missing conversion")
{
    std::stringstream ss;
    ss << E::one << " " << E::two << " " << E::three;
    REQUIRE( ss.str() == "one two 3" );
}

TEST_CASE("enum_helper_test - gap handled")
{
    std::stringstream ss;
    ss << E::one << " " << E::two << " " << E::three << " " << E::four;
    REQUIRE( ss.str() == "one two 3 four" );
}

