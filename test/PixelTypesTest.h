
// catch
#include "catch.hpp"

// std
#include <utility>

// to be tested
#include "PixelTypes.h"

TEST_CASE("PixelTypesTest - GSizeTest")
{
    REQUIRE( sizeof(G8) == sizeof(uint8_t) );
    REQUIRE( sizeof(G16) == sizeof(uint16_t) );
    REQUIRE( sizeof(G32) == sizeof(uint32_t) );
    REQUIRE( sizeof(GF) == sizeof(double) );
}

TEST_CASE("PixelTypesTest - RGBASizeTest")
{
    REQUIRE( sizeof(RGBA8) == 4*sizeof(uint8_t) );
    REQUIRE( sizeof(RGBA16) == 4*sizeof(uint16_t) );
    REQUIRE( sizeof(RGBA32) == 4*sizeof(uint32_t) );
}

TEST_CASE("PixelTypesTest - YUVASizeTest")
{
    REQUIRE( sizeof(YUVA8) == 4*sizeof(uint8_t) );
    REQUIRE( sizeof(YUVA16) == 4*sizeof(uint16_t) );
    REQUIRE( sizeof(YUVA32) == 4*sizeof(uint32_t) );
}

TEST_CASE("PixelTypesTest - ComplexSizeTest")
{
    REQUIRE( sizeof(CF) == 2*sizeof(double) );
}

TEST_CASE("PixelTypesTest - IsPixelTypeTest")
{
    REQUIRE( is_pixeltype<RGBA<uint16_t>>::value );
    REQUIRE( is_pixeltype<YUVA<uint16_t>>::value );
    REQUIRE( is_pixeltype<G<uint16_t>>::value );
    REQUIRE( is_pixeltype<Complex<uint16_t>>::value );
    REQUIRE( !is_pixeltype<uint16_t>::value );
}

TEST_CASE("PixelTypesTest - PixelConversionTest")
{
    REQUIRE( (pixeltype_is_convertible< RGBA<double>, G<double> >::value) );
    REQUIRE( (pixeltype_is_convertible< G<double>, RGBA<double> >::value) );
    REQUIRE( (pixeltype_is_convertible< G<double>, Complex<double> >::value) );
    REQUIRE( (pixeltype_is_convertible< Complex<double>, G<double> >::value) );
    REQUIRE( (pixeltype_is_convertible< G8, Complex<double> >::value) );
    REQUIRE( (pixeltype_is_convertible< G8, G16 >::value) );
    REQUIRE( (pixeltype_is_convertible< G8, GF >::value) );
}

TEST_CASE("PixelTypesTest - RGBATest")
{
    REQUIRE( RGBA8( 0, 0, 0, 0 ) == RGBA8() );
    REQUIRE( RGBA8( 128 ) == RGBA8( 128, 128, 128, 255) );
    REQUIRE( RGBA8( 128 ) != RGBA8( 128, 128, 128, 0) );
}

TEST_CASE("PixelTypesTest - RGBACopyTest")
{
    RGBA8 a( 128 );
    RGBA8 b( a );
    REQUIRE( a == b );
}

TEST_CASE("PixelTypesTest - RGBAMoveTest")
{
    RGBA8 a( 128 );
    RGBA8 b( std::move( a ) );
    REQUIRE( RGBA8( 128 ) == b );
}

TEST_CASE("PixelTypesTest - RGBAAssignTest")
{
    RGBA8 a( 128 );
    RGBA8 b;
    b = a;
    REQUIRE( a == b );
}

TEST_CASE("PixelTypesTest - RGBAMoveAssignTest")
{
    RGBA8 a( 128 );
    RGBA8 b;
    b = std::move( a );
    REQUIRE( RGBA8( 128 ) == b );
}

TEST_CASE("PixelTypesTest - ComplexConjugateTest")
{
    CF a{ 1,  1 };
    CF b{ 1, -1 };
    REQUIRE( a.conj() == b );
}

TEST_CASE("PixelTypesTest - ComplexToGreyscaleTest")
{
    {
        CF c{ 1,  0 };
        REQUIRE( GF{ c.abs() } == GF{ 1 } );
    }

    {
        CF c{ 0,  1 };
        REQUIRE( GF{ c.abs() } == GF{ 1 } );
    }
}
