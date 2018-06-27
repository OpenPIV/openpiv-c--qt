
// gtest
#include "gtest/gtest.h"

// to be tested
#include "PixelTypes.h"

TEST(PixelTypesTest, GSizeTest)
{
    ASSERT_EQ( sizeof(G8), sizeof(uint8_t) );
    ASSERT_EQ( sizeof(G16), sizeof(uint16_t) );
    ASSERT_EQ( sizeof(G32), sizeof(uint32_t) );
    ASSERT_EQ( sizeof(GF), sizeof(double) );
}

TEST(PixelTypesTest, RGBASizeTest)
{
    ASSERT_EQ( sizeof(RGBA8), 4*sizeof(uint8_t) );
    ASSERT_EQ( sizeof(RGBA16), 4*sizeof(uint16_t) );
    ASSERT_EQ( sizeof(RGBA32), 4*sizeof(uint32_t) );
}

TEST(PixelTypesTest, YUVASizeTest)
{
    ASSERT_EQ( sizeof(YUVA8), 4*sizeof(uint8_t) );
    ASSERT_EQ( sizeof(YUVA16), 4*sizeof(uint16_t) );
    ASSERT_EQ( sizeof(YUVA32), 4*sizeof(uint32_t) );
}

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


