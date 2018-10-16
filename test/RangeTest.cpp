
// catch
#include "catch.hpp"

// to be tested
#include "Range.h"

using namespace Catch;

TEST_CASE("UtilTest - RangeFromToTest")
{
    std::vector<int> expected{ 1, 2, 3, 4, 5 };
    REQUIRE( expected == range_from( 1 ).to( 5 ) );
}

TEST_CASE("UtilTest - RangeFromToNegativeTest")
{
    std::vector<int> expected{ -3, -2, -1, 0, 1 };
    REQUIRE( expected == range_from( -3 ).to( 1 ) );
}

TEST_CASE("UtilTest - RangeFromToBackwardsTest")
{
    std::vector<int> expected{ 5, 4, 3, 2, 1 };
    REQUIRE( expected == range_from( 5 ).to( 1 ) );
}

TEST_CASE("UtilTest - RangeFromLengthTest")
{
    std::vector<int> expected{ 1, 2, 3, 4, 5 };
    REQUIRE( expected == range_start_at( 1 ).length( 5 ) );
}

