
// catch
#include "catch.hpp"

// std
#include <cstdint>
#include <limits>
#include <sstream>
#include <string>

// local
#include "TestUtils.h"

// to be tested
#include "Util.h"

using namespace Catch;

TEST_CASE("UtilTest - IsPow2Test")
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

TEST_CASE("UtilTest - ThrowerTest")
{
    auto l = [](){ Thrower<std::runtime_error>() << "die!"; };

    _REQUIRE_THROWS_MATCHES( l(), std::runtime_error, Contains( "die!" ) );
}

TEST_CASE("UtilTest - CheckedUnsignedConversionSameSizeWithinRange")
{
    int32_t i{ 32 };
    uint32_t u{ 32 };

    REQUIRE(i == checked_unsigned_conversion<decltype(i)>(u));
}

TEST_CASE("UtilTest - CheckedUnsignedConversionSameSizeOutwithRange")
{
    uint32_t u{ std::numeric_limits<uint32_t>::max() };

    _REQUIRE_THROWS_MATCHES(
        checked_unsigned_conversion<int32_t>(u),
        std::range_error,
        Contains( "unable to convert" ) );
}

TEST_CASE("UtilTest - CheckedUnsignedConversionLargerSize")
{
    int64_t i{ std::numeric_limits<uint32_t>::max() };
    uint32_t u{ std::numeric_limits<uint32_t>::max() };

    REQUIRE(i == checked_unsigned_conversion<int64_t>(u));
}

TEST_CASE("UtilTest - AreAllConvertibleTrue")
{
    bool b{ are_all_convertible<double, int, unsigned int, float>::value };
    REQUIRE(b);
}

TEST_CASE("UtilTest - AreAllConvertibleFalse")
{
    bool b{ are_all_convertible<double, int, unsigned int, char>::value };
    REQUIRE(b);
}

TEST_CASE("UtilTest - AreAllEqualTrue")
{
    bool b{ are_all_equal<double, double, double>::value };
    REQUIRE(b);
}

TEST_CASE("UtilTest - AreAllEqualFalse")
{
    bool b{ are_all_equal<double, double, int>::value };
    REQUIRE(!b);
}

TEST_CASE("UtilTest - ConvertArray")
{
    std::array<int, 4> i{ {1, 2, 3, 4} };
    auto d{ convert_array_to< double >(i) };

    bool result = true;
    for ( decltype(i.size()) j=0; j<i.size(); ++j)
        result &= (i[j] == d[j]);

    REQUIRE(i.size() == d.size());
    REQUIRE(result);
}
