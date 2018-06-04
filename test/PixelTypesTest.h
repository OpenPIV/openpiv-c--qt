
// gtest
#include "gtest/gtest.h"

// to be tested
#include "PixelTypes.h"

TEST(PixelTypesTest, RGBATest)
{
    ASSERT_EQ( RGBA8( 0, 0, 0, 0 ), RGBA8() );
    ASSERT_EQ( RGBA8( 128 ), RGBA8( 128, 128, 128, 255) );
    ASSERT_NE( RGBA8( 128 ), RGBA8( 128, 128, 128, 0) );
}

TEST(PixelTypesTest, RGBACopyTest)
{
    RGBA8 a( 128 );
    RGBA8 b( a );
    ASSERT_EQ( a, b );
}

TEST(PixelTypesTest, RGBAMoveTest)
{
    RGBA8 a( 128 );
    RGBA8 b( std::move( a ) );
    ASSERT_EQ( RGBA8( 128 ), b );
}

TEST(PixelTypesTest, RGBAAssignTest)
{
    RGBA8 a( 128 );
    RGBA8 b;
    b = a;
    ASSERT_EQ( a, b );
}

TEST(PixelTypesTest, RGBAMoveAssignTest)
{
    RGBA8 a( 128 );
    RGBA8 b;
    b = std::move( a );
    ASSERT_EQ( RGBA8( 128 ), b );
}


