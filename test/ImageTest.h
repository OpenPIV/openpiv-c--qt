
// gtest
#include "gtest/gtest.h"

// std
#include <sstream>
#include <string>

// to be tested
#include "Image.h"
#include "ImageUtils.h"

TEST(ImageTest, IntTest)
{
    UInt8Image im( 200, 100 );
    ASSERT_EQ( im.width(), 200 );
    ASSERT_EQ( im.height(), 100 );
}

TEST(ImageTest, FillTest)
{
    UInt8Image im( 200, 100 );
    const uint8_t v = 128;
    fill( im, v );
    bool result = true;
    for ( uint32_t i=0; i<im.pixel_count(); ++i )
        result &= (im[i] == v);

    ASSERT_EQ( result, true );
}
