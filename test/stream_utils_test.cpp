
// catch
#include <catch2/catch_test_macros.hpp>

// std
#include <sstream>
#include <string>

// to be tested
#include "core/stream_utils.h"

using namespace openpiv::core;

TEST_CASE("stream_utils_test - join")
{
    std::vector<int> vi{ 1, 2, 3, 4, 5 };
    REQUIRE(join(vi) == "[1, 2, 3, 4, 5]");

    std::list<int> li{ 2, 3, 4, 5, 6 };
    REQUIRE(join(li) == "[2, 3, 4, 5, 6]");
}


TEST_CASE("stream_utils_test - ostream")
{
    std::stringstream ss;

    SECTION("vector")
    {
        ss << std::vector<int>{ 1, 2, 3, 4, 5 };
        REQUIRE(ss.str() == "[1, 2, 3, 4, 5]");
    }

    SECTION("vector")
    {
        ss << std::list<int>{ 2, 3, 4, 5, 6 };
        REQUIRE(ss.str() == "[2, 3, 4, 5, 6]");
    }
}
