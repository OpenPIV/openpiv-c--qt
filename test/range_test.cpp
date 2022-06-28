
// catch
#include <catch2/catch_test_macros.hpp>

// to be tested
#include "core/range.h"

using namespace Catch;
using namespace openpiv::core;


TEST_CASE("util_test - range_from_to_test")
{
    std::vector<int> expected{ 1, 2, 3, 4, 5 };
    REQUIRE( expected == range_from( 1 ).to( 5 ) );
}

TEST_CASE("util_test - range_from_to_negative_test")
{
    std::vector<int> expected{ -3, -2, -1, 0, 1 };
    REQUIRE( expected == range_from( -3 ).to( 1 ) );
}

TEST_CASE("util_test - range_from_to_backwards_test")
{
    std::vector<int> expected{ 5, 4, 3, 2, 1 };
    REQUIRE( expected == range_from( 5 ).to( 1 ) );
}

TEST_CASE("util_test - range_from_length_test")
{
    std::vector<int> expected{ 1, 2, 3, 4, 5 };
    REQUIRE( expected == range_start_at( 1 ).length( 5 ) );
}

