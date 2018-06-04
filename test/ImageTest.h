
// gtest
#include "gtest/gtest.h"

// std
#include <sstream>
#include <string>
#include <tuple>

// local
#include "TestUtils.h"

// to be tested
#include "Image.h"
#include "ImageUtils.h"

TEST(ImageTest, IntTest)
{
    G8Image im( 200, 100 );
    ASSERT_EQ( im.width(), 200 );
    ASSERT_EQ( im.height(), 100 );
}

TEST(ImageTest, FillTest)
{
    G8Image im; G8 v;
    std::tie( im, v ) = createAndFill( Size( 200, 100 ), 128);
    bool result = true;
    for ( uint32_t i=0; i<im.pixel_count(); ++i )
        result &= (im[i] == v);

    ASSERT_EQ( result, true );
}

TEST(ImageTest, CopyTest)
{
    G8Image im; G8 v;
    std::tie( im, v ) = createAndFill( Size( 200, 100 ), 128);

    G8Image im2{ im };
    ASSERT_EQ(im.width(), im2.width());
    ASSERT_EQ(im.height(), im2.height());

    bool result = true;
    for ( uint32_t i=0; i<im.pixel_count(); ++i )
        result &= (im2[i] == v);

    ASSERT_EQ( result, true );
}

TEST(ImageTest, MoveTest)
{
    G8Image im; G8 v;
    std::tie( im, v ) = createAndFill( Size( 200, 100 ), 128);

    G8Image im2{ std::move(im) };

    bool result = true;
    for ( uint32_t i=0; i<im.pixel_count(); ++i )
        result &= (im2[i] == v);

    ASSERT_EQ( result, true );
}

TEST(ImageTest, ConvertTest)
{
    G8Image im; G8 v;
    std::tie( im, v ) = createAndFill( Size( 200, 200 ), 128);

    DoubleImage im2{ im };

    bool result = true;
    for ( uint32_t i=0; i<im.pixel_count(); ++i )
        result &= (im2[i] == v);

    ASSERT_EQ( result, true );
}

TEST(ImageTest, LineOutOfBoundsTest)
{
    G8Image im; G8 v;
    std::tie( im, v ) = createAndFill( Size( 200, 100 ), 128);

    _ASSERT_DEATH( im.line(101), std::range_error, "line out of range" );
}

TEST(ImageTest, LineTest)
{
    G8Image im; G8 v;
    std::tie( im, v ) = createAndFill( Size( 2, 2 ), 0);
    int64_t sum1 = pixel_sum(im) / im.pixel_count();
    ASSERT_EQ(sum1, 0);

    G8* p = im.line(1);
    for ( size_t i=0; i<im.width(); ++i )
        *p++ = 128;

    int64_t sum2 = pixel_sum(im) / im.pixel_count();
    ASSERT_EQ(sum2, 64);
}

TEST(ImageTest, EqualityTest)
{
    G8Image im1;
    std::tie( im1, std::ignore ) = createAndFill( Size( 200, 100 ), 128);

    G8Image im2;
    std::tie( im2, std::ignore ) = createAndFill( Size( 200, 100 ), 128);

    ASSERT_EQ( im1, im2 );

    // modify a pixel
    im2[ UInt2DPoint( 100u, 50u ) ] = 100;
    ASSERT_NE( im1, im2 );
}
