
// catch
#include "catch.hpp"

// std
#include <sstream>

// local
#include "test_utils.h"

// to be tested
#include "enum_helper.h"

using namespace Catch;
using namespace openpiv::core;

enum class E {
    one = 1,
    two = 2,
    three = 3
};
DECLARE_ENUM_HELPER( E, {
        { E::one, "one" },
        { E::two, "two" }
    } )

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

