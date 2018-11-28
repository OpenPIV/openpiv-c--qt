
// catch
#include "catch.hpp"

// std
#include <sstream>

// local
#include "TestUtils.h"

// to be tested
#include "EnumHelper.h"

using namespace Catch;

enum class E {
    one = 1,
    two = 2,
    three = 3
};
DECLARE_ENUM_HELPER( E, {
        { E::one, "one" },
        { E::two, "two" }
    } )

TEST_CASE("EnumHelperTest - default")
{
    std::stringstream ss;
    ss << E::one << " " << E::two;
    REQUIRE( ss.str() == "one two" );
}

TEST_CASE("EnumHelperTest - missing conversion")
{
    std::stringstream ss;
    ss << E::one << " " << E::two << " " << E::three;
    REQUIRE( ss.str() == "one two 3" );
}

