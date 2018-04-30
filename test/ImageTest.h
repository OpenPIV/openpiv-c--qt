
// gtest
#include "gtest/gtest.h"

// std
#include <sstream>
#include <string>
#include <tuple>

// to be tested
#include "Image.h"
#include "ImageUtils.h"

std::tuple< UInt8Image, uint8_t > createAndFill( Size s, uint8_t v )
{
    UInt8Image result(s.width(), s.height());
    fill( result, v );

    return std::make_tuple( result, v );
}

TEST(ImageTest, IntTest)
{
    UInt8Image im( 200, 100 );
    ASSERT_EQ( im.width(), 200 );
    ASSERT_EQ( im.height(), 100 );
}

TEST(ImageTest, FillTest)
{
    UInt8Image im; uint8_t v;
    std::tie( im, v ) = createAndFill( Size( 200, 100 ), 128);
    bool result = true;
    for ( uint32_t i=0; i<im.pixel_count(); ++i )
        result &= (im[i] == v);

    ASSERT_EQ( result, true );
}

TEST(ImageTest, CopyTest)
{
    UInt8Image im; uint8_t v;
    std::tie( im, v ) = createAndFill( Size( 200, 100 ), 128);

    UInt8Image im2{ im };
    ASSERT_EQ(im.width(), im2.width());
    ASSERT_EQ(im.height(), im2.height());

    bool result = true;
    for ( uint32_t i=0; i<im.pixel_count(); ++i )
        result &= (im2[i] == v);

    ASSERT_EQ( result, true );
}

TEST(ImageTest, MoveTest)
{
    UInt8Image im; uint8_t v;
    std::tie( im, v ) = createAndFill( Size( 200, 100 ), 128);

    UInt8Image im2{ std::move(im) };

    bool result = true;
    for ( uint32_t i=0; i<im.pixel_count(); ++i )
        result &= (im2[i] == v);

    ASSERT_EQ( result, true );
}

TEST(ImageTest, LineOutOfBoundsTest)
{
    UInt8Image im; uint8_t v;
    std::tie( im, v ) = createAndFill( Size( 200, 100 ), 128);

    ASSERT_DEATH( im.line(101), "line out of range.*" );
}

TEST(ImageTest, LineTest)
{
    UInt8Image im; uint8_t v;
    std::tie( im, v ) = createAndFill( Size( 2, 2 ), 0);
    int64_t sum1 = pixel_sum(im) / im.pixel_count();
    ASSERT_EQ(sum1, 0);

    uint8_t* p = im.line(1);
    for ( size_t i=0; i<im.width(); ++i )
        *p++ = 128;

    int64_t sum2 = pixel_sum(im) / im.pixel_count();
    ASSERT_EQ(sum2, 64);
}

TEST(ImageTest, EqualityTest)
{
    UInt8Image im1;
    std::tie( im1, std::ignore ) = createAndFill( Size( 200, 100 ), 128);

    UInt8Image im2;
    std::tie( im2, std::ignore ) = createAndFill( Size( 200, 100 ), 128);

    ASSERT_EQ( im1, im2 );

    // modify a pixel
    im2[ UInt2DPoint( 100, 50 ) ] = 100;
    ASSERT_NE( im1, im2 );
}
