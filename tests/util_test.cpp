
// catch
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>

// std
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <sstream>
#include <string>

// local
#include "test_utils.h"

// to be tested
#include "core/util.h"

using namespace std::string_literals;
using namespace Catch;
using namespace Catch::Matchers;
using namespace openpiv::core;

TEST_CASE("util_test - typed_memcpy - stride 1")
{
#pragma pack(push, 1)
    struct s {
        uint8_t a = 1;
        uint32_t b = 2;
        double c = 3.141;
    };
#pragma pack(pop)

    static constexpr size_t count = 20;
    std::array<s, count> in{ {} };
    std::array<uint8_t, sizeof(s) * count> out{ 0 };

    typed_memcpy(&out[0], &in[0], count);
    int result = memcmp(&in[0], &out[0], out.size());
    REQUIRE(result == 0);
}

TEST_CASE("util_test - typed_memcpy - stride 2")
{
#pragma pack(push, 1)
    struct s {
        uint8_t a = 1;
        uint32_t b = 2;
        double c = 3.141;
    };
#pragma pack(pop)

    static constexpr size_t count = 20;
    std::array<s, 2*count> in{ {} };
    std::array<uint8_t, sizeof(s) * count> out{ 0 };

    typed_memcpy(&out[0], &in[0], count, 2);
    int result = memcmp(&in[0], &out[0], out.size());
    REQUIRE(result == 0);
}

TEST_CASE("util_test - is_pow2_test")
{
    std::vector< uint16_t > valid;
    uint16_t p = 1;
    while ( p < std::numeric_limits<uint16_t>::max()/2 )
    {
        valid.push_back( p );
        p <<= 1;
    }

    uint16_t v = 0;
    size_t p_index = 0;
    while ( p_index < valid.size() && v < std::numeric_limits<uint16_t>::max() )
    {
        bool check = is_pow2(v);
        REQUIRE( check == (v == valid[p_index]) );
        if ( check )
            ++p_index;

        ++v;
    }
}

TEST_CASE("util_test - exception_builderTest")
{
    auto l = [](){ exception_builder<std::runtime_error>() << "die!"; };

    _REQUIRE_THROWS_MATCHES( l(),
                             std::runtime_error,
                             ContainsSubstring( "die!"s, CaseSensitive::No ) );
}

TEST_CASE("util_test - checked_unsigned_conversion_same_size_within_range")
{
    int32_t i{ 32 };
    uint32_t u{ 32 };

    REQUIRE(i == checked_unsigned_conversion<decltype(i)>(u));
}

TEST_CASE("util_test - checked_unsigned_conversion_same_size_outwith_range")
{
    uint32_t u{ std::numeric_limits<uint32_t>::max() };

    _REQUIRE_THROWS_MATCHES(
        checked_unsigned_conversion<int32_t>(u),
        std::range_error,
        ContainsSubstring( "unable to convert"s, CaseSensitive::No ) );
}

TEST_CASE("util_test - checked_unsigned_conversion_larger_size")
{
    int64_t i{ std::numeric_limits<uint32_t>::max() };
    uint32_t u{ std::numeric_limits<uint32_t>::max() };

    REQUIRE(i == checked_unsigned_conversion<int64_t>(u));
}

TEST_CASE("util_test - are_all_convertible_true")
{
    bool b{ are_all_convertible_v<double, int, unsigned int, float> };
    REQUIRE(b);
}

TEST_CASE("util_test - are_all_convertible_false")
{
    bool b{ are_all_convertible_v<double, int, unsigned int, char> };
    REQUIRE(b);
}

TEST_CASE("util_test - are_all_equal_true")
{
    bool b{ are_all_equal_v<double, double, double> };
    REQUIRE(b);
}

TEST_CASE("util_test - are_all_equal_false")
{
    bool b{ are_all_equal_v<double, double, int> };
    REQUIRE(!b);
}

TEST_CASE("util_test - convert_array")
{
    std::array<int, 4> i{ {1, 2, 3, 4} };
    auto d{ convert_array_to< double >(i) };

    bool result = true;
    for ( decltype(i.size()) j=0; j<i.size(); ++j)
        result &= (i[j] == d[j]);

    REQUIRE(i.size() == d.size());
    REQUIRE(result);
}
