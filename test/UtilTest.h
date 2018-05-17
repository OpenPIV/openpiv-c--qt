
// gtest
#include "gtest/gtest.h"

// std
#include <cstdint>
#include <limits>
#include <sstream>
#include <string>

// to be tested
#include "Util.h"

TEST(UtilTest, ThrowerTest)
{
    auto l = [](){ Thrower<std::runtime_error>() << "die!"; };

    ASSERT_DEATH( l(), "die!" );
}

TEST(UtilTest, CheckedUnsignedConversionSameSizeWithinRange)
{
    int32_t i{ 32 };
    uint32_t u{ 32 };

    ASSERT_EQ(i, checked_unsigned_conversion<decltype(i)>(u));
}

TEST(UtilTest, CheckedUnsignedConversionSameSizeOutwithRange)
{
    uint32_t u{ std::numeric_limits<uint32_t>::max() };

    ASSERT_DEATH(checked_unsigned_conversion<int32_t>(u), "unable to convert.*");
}

TEST(UtilTest, CheckedUnsignedConversionLargerSize)
{
    int64_t i{ std::numeric_limits<uint32_t>::max() };
    uint32_t u{ std::numeric_limits<uint32_t>::max() };

    ASSERT_EQ(i, checked_unsigned_conversion<int64_t>(u));
}

TEST(UtilTest, AreAllConvertibleTrue)
{
    bool b{ are_all_convertible<double, int, unsigned int, float>::value };
    ASSERT_TRUE(b);
}

TEST(UtilTest, AreAllConvertibleFalse)
{
    bool b{ are_all_convertible<double, int, unsigned int, char>::value };
    ASSERT_TRUE(b);
}

TEST(UtilTest, AreAllEqualTrue)
{
    bool b{ are_all_equal<double, double, double>::value };
    ASSERT_TRUE(b);
}

TEST(UtilTest, AreAllEqualFalse)
{
    bool b{ are_all_equal<double, double, int>::value };
    ASSERT_FALSE(b);
}

TEST(UtilTest, ConvertArray)
{
    std::array<int, 4> i{1, 2, 3, 4};
    auto d{ convert_array_to< double >(i) };

    bool result = true;
    for ( decltype(i.size()) j=0; j<i.size(); ++j)
        result &= (i[j] == d[j]);

    ASSERT_EQ(i.size(), d.size());
    ASSERT_TRUE(result);
}
