
// gtest
#include "gtest/gtest.h"

// local
#include "TestUtils.h"

// to be tested
#include "ImageStats.h"
#include "PixelTypes.h"

TEST(ImageStatsTest, FindImageRangeUniformTest)
{
    G8Image im; G8 v;
    std::tie( im, v ) = createAndFill( Size( 200, 200 ), 0);

    G8 min, max;
    std::tie( min, max ) = findImageRange( im );
    ASSERT_EQ(min, 0);
    ASSERT_EQ(max, 0);
}

TEST(ImageStatsTest, FindImageRangeMaxTest)
{
    G8Image im; G8 v;
    std::tie( im, v ) = createAndFill( Size( 200, 200 ), 0);

    G8 min, max;
    im[Point2<uint32_t>{ 50, 50 }] = 255;
    std::tie( min, max ) = findImageRange( im );
    ASSERT_EQ(min, 0);
    ASSERT_EQ(max, 255);
}

TEST(ImageStatsTest, FindImageRangeMinTest)
{
    G8Image im; G8 v;
    std::tie( im, v ) = createAndFill( Size( 200, 200 ), 255);

    G8 min, max;
    im[Point2<uint32_t>{ 50, 50 }] = 0;
    std::tie( min, max ) = findImageRange( im );
    ASSERT_EQ(min, 0);
    ASSERT_EQ(max, 255);
}

TEST(ImageStatsTest, FindImageRangeMinMaxTest)
{
    G8Image im; G8 v;
    std::tie( im, v ) = createAndFill( Size( 200, 200 ), 128);

    G8 min, max;
    im[Point2<uint32_t>{ 50, 50 }] = 0;
    im[Point2<uint32_t>{ 150, 150 }] = 255;
    std::tie( min, max ) = findImageRange( im );
    ASSERT_EQ(min, 0);
    ASSERT_EQ(max, 255);
}

