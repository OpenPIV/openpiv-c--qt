
// catch
#include <catch2/catch_test_macros.hpp>

// std
#include <utility>

// to be tested
#include "core/pixel_types.h"

using namespace openpiv::core;

TEST_CASE("pixel_types_test - g_size_test")
{
    REQUIRE( sizeof(g_8) == sizeof(uint8_t) );
    REQUIRE( sizeof(g_16) == sizeof(uint16_t) );
    REQUIRE( sizeof(g_32) == sizeof(uint32_t) );
    REQUIRE( sizeof(g_f) == sizeof(double) );
}

TEST_CASE("pixel_types_test - rgba_size_test")
{
    REQUIRE( sizeof(rgba_8) == 4*sizeof(uint8_t) );
    REQUIRE( sizeof(rgba_16) == 4*sizeof(uint16_t) );
    REQUIRE( sizeof(rgba_32) == 4*sizeof(uint32_t) );
}

TEST_CASE("pixel_types_test - yuva_size_test")
{
    REQUIRE( sizeof(yuva_8) == 4*sizeof(uint8_t) );
    REQUIRE( sizeof(yuva_16) == 4*sizeof(uint16_t) );
    REQUIRE( sizeof(yuva_32) == 4*sizeof(uint32_t) );
}

TEST_CASE("pixel_types_test - complex_size_test")
{
    REQUIRE( sizeof(c_f) == 2*sizeof(double) );
}

TEST_CASE("pixel_types_test - is_pixel_type_test")
{
    REQUIRE( is_pixeltype_v< rgba<uint16_t> > );
    REQUIRE( is_pixeltype_v< yuva<uint16_t> > );
    REQUIRE( is_pixeltype_v< g<uint16_t> > );
    REQUIRE( is_pixeltype_v< complex<uint16_t> > );
    REQUIRE( !is_pixeltype_v< uint16_t > );
}

TEST_CASE("pixel_types_test - is_real_mono_pixel_type_test")
{
    REQUIRE( !is_real_mono_pixeltype_v< rgba<uint16_t> > );
    REQUIRE( !is_real_mono_pixeltype_v< yuva<uint16_t> > );
    REQUIRE( !is_real_mono_pixeltype_v< complex<uint16_t> > );
    REQUIRE( !is_real_mono_pixeltype_v< uint16_t > );
    REQUIRE( is_real_mono_pixeltype_v< g<uint32_t> > );
    REQUIRE( is_real_mono_pixeltype_v< g<uint16_t> > );
    REQUIRE( is_real_mono_pixeltype_v< g<uint8_t> > );
}

TEST_CASE("pixel_types_test - pixel_conversion_test")
{
    SECTION( "self" )
    {
        REQUIRE( (pixeltype_is_convertible_v< g_f, g_f >) );
        REQUIRE( (pixeltype_is_convertible_v< g_8, g_8 >) );
        REQUIRE( (pixeltype_is_convertible_v< g_16, g_16 >) );
        REQUIRE( (pixeltype_is_convertible_v< g_32, g_32 >) );
        REQUIRE( (pixeltype_is_convertible_v< c_f, c_f >) );
        REQUIRE( (pixeltype_is_convertible_v< c_8, c_8 >) );
        REQUIRE( (pixeltype_is_convertible_v< c_16, c_16 >) );
        REQUIRE( (pixeltype_is_convertible_v< c_32, c_32 >) );
        REQUIRE( (pixeltype_is_convertible_v< rgba<double>, rgba<double> >) );
        REQUIRE( (pixeltype_is_convertible_v< rgba_8, rgba_8 >) );
        REQUIRE( (pixeltype_is_convertible_v< rgba_16, rgba_16 >) );
        REQUIRE( (pixeltype_is_convertible_v< rgba_32, rgba_32 >) );
    }

    SECTION( "convertion" )
    {
        REQUIRE( (pixeltype_is_convertible_v< rgba<double>, g<double> >) );
        REQUIRE( (pixeltype_is_convertible_v< g<double>, rgba<double> >) );
        REQUIRE( (pixeltype_is_convertible_v< g<double>, complex<double> >) );
        REQUIRE( (pixeltype_is_convertible_v< complex<double>, g<double> >) );
        REQUIRE( (pixeltype_is_convertible_v< g_8, c_f >) );
        REQUIRE( (pixeltype_is_convertible_v< g_8, g_16 >) );
        REQUIRE( (pixeltype_is_convertible_v< g_8, g_32 >) );
        REQUIRE( (pixeltype_is_convertible_v< g_8, g_f >) );
        REQUIRE( (pixeltype_is_convertible_v< g_f, c_f >) );
    }
}

TEST_CASE("pixel_types_test - rgba_Test")
{
    REQUIRE( rgba_8( 0, 0, 0, 0 ) == rgba_8() );
    REQUIRE( rgba_8( 128 ) == rgba_8( 128, 128, 128, 255) );
    REQUIRE( rgba_8( 128 ) != rgba_8( 128, 128, 128, 0) );
}

TEST_CASE("pixel_types_test - rgba_copy_test")
{
    rgba_8 a( 128 );
    rgba_8 b( a );
    REQUIRE( a == b );
}

TEST_CASE("pixel_types_test - rgba_move_test")
{
    rgba_8 a( 128 );
    rgba_8 b( std::move( a ) );
    REQUIRE( rgba_8( 128 ) == b );
}

TEST_CASE("pixel_types_test - rgba_assign_test")
{
    rgba_8 a( 128 );
    rgba_8 b;
    b = a;
    REQUIRE( a == b );
}

TEST_CASE("pixel_types_test - rgba_move_assign_test")
{
    rgba_8 a( 128 );
    rgba_8 b;
    b = std::move( a );
    REQUIRE( rgba_8( 128 ) == b );
}

TEST_CASE("pixel_types_test - complex_conjugate_test")
{
    c_f a{ 1,  1 };
    c_f b{ 1, -1 };
    REQUIRE( a.conj() == b );
}

TEST_CASE("pixel_types_test - complex_to_greyscale_test")
{
    {
        c_f c{ 1,  0 };
        REQUIRE( g_f{ c.abs() } == g_f{ 1 } );
    }

    {
        c_f c{ 0,  1 };
        REQUIRE( g_f{ c.abs() } == g_f{ 1 } );
    }
}
