
// catch
#include <catch2/catch_test_macros.hpp>

// std
#include <sstream>
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
    REQUIRE( is_pixeltype_v< complex<int16_t> > );
    REQUIRE( !is_pixeltype_v< uint16_t > );
}

TEST_CASE("pixel_types_test - is_real_mono_pixel_type_test")
{
    REQUIRE( !is_real_mono_pixeltype_v< rgba<uint16_t> > );
    REQUIRE( !is_real_mono_pixeltype_v< yuva<uint16_t> > );
    REQUIRE( !is_real_mono_pixeltype_v< complex<int16_t> > );
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

    {
        c_f c{ 1,  1 };
        REQUIRE( g_f{ c.abs_sqr() } == g_f{ 2 } );
    }
}

TEST_CASE("pixel_types_test - rgba")
{
    SECTION("default constructor")
    {
        auto p = rgba_8();
        REQUIRE(std::is_same_v<rgba_8::value_t, uint8_t>);
        CHECK(p.r == rgba_8::value_t{});
        CHECK(p.g == rgba_8::value_t{});
        CHECK(p.b == rgba_8::value_t{});
        CHECK(p.a == rgba_8::value_t{});
    }

    SECTION("constructor from value")
    {
        auto p = rgba_8(128);
        CHECK(p.r == 128);
        CHECK(p.g == 128);
        CHECK(p.b == 128);
        CHECK(p.a == std::numeric_limits<rgba_8::value_t>::max());
    }

    SECTION("assignment from value")
    {
        rgba_8 p;
        p = 128;
        CHECK(p.r == 128);
        CHECK(p.g == 128);
        CHECK(p.b == 128);
        CHECK(p.a == std::numeric_limits<rgba_8::value_t>::max());
    }

    SECTION("constructor from values")
    {
        auto p = rgba_8(127, 128, 129, 130);
        CHECK(p.r == 127);
        CHECK(p.g == 128);
        CHECK(p.b == 129);
        CHECK(p.a == 130);
    }

    SECTION("comparison operators")
    {
        auto p1 = rgba_8(127, 128, 129, 130);
        auto p2 = rgba_8(127, 128, 129, 131);
        CHECK(p1 != p2);
        p2.a = 130;
        CHECK(p1 == p2);
    }

    SECTION("defaulted constructors")
    {
        auto p = rgba_8(1, 2, 3, 4);
        auto p_copy{ p };
        auto p_move{ std::move(p) };

        CHECK(p_copy.r == 1);
        CHECK(p_copy.g == 2);
        CHECK(p_copy.b == 3);
        CHECK(p_copy.a == 4);

        CHECK(p_copy == p_move);
    }

    SECTION("defaulted assignments")
    {
        auto p = rgba_8(1, 2, 3, 4);
        rgba_8 p_copy;
        p_copy = p;
        rgba_8 p_move;
        p_move = std::move(p);

        CHECK(p_copy.r == 1);
        CHECK(p_copy.g == 2);
        CHECK(p_copy.b == 3);
        CHECK(p_copy.a == 4);

        CHECK(p_copy == p_move);
    }

    SECTION("ostream")
    {
        auto p = rgba_8(1, 2, 3, 4);
        std::ostringstream ss;
        ss << p;
        CHECK(ss.str() == "rgba(1, 2, 3, 4)");
    }
}

TEST_CASE("pixel_types_test - yuva")
{
    SECTION("default constructor")
    {
        auto p = yuva_8();
        REQUIRE(std::is_same_v<yuva_8::value_t, uint8_t>);
        CHECK(p.y == yuva_8::value_t{});
        CHECK(p.u == yuva_8::value_t{});
        CHECK(p.v == yuva_8::value_t{});
        CHECK(p.a == yuva_8::value_t{});
    }

    SECTION("constructor from value")
    {
        auto p = yuva_8(128);
        CHECK(p.y == 128);
        CHECK(p.u == yuva_8::value_t{});
        CHECK(p.v == yuva_8::value_t{});
        CHECK(p.a == std::numeric_limits<yuva_8::value_t>::max());
    }

    SECTION("assignment from value")
    {
        yuva_8 p;
        p = 128;
        CHECK(p.y == 128);
        CHECK(p.u == yuva_8::value_t{});
        CHECK(p.v == yuva_8::value_t{});
        CHECK(p.a == std::numeric_limits<yuva_8::value_t>::max());
    }

    SECTION("constructor from values")
    {
        auto p = yuva_8(127, 128, 129, 130);
        CHECK(p.y == 127);
        CHECK(p.u == 128);
        CHECK(p.v == 129);
        CHECK(p.a == 130);
    }

    SECTION("comparison operators")
    {
        auto p1 = yuva_8(127, 128, 129, 130);
        auto p2 = yuva_8(127, 128, 129, 131);
        CHECK(p1 != p2);
        p2.a = 130;
        CHECK(p1 == p2);
    }

    SECTION("defaulted constructors")
    {
        auto p = yuva_8(1, 2, 3, 4);
        auto p_copy{ p };
        auto p_move{ std::move(p) };

        CHECK(p_copy.y == 1);
        CHECK(p_copy.u == 2);
        CHECK(p_copy.v == 3);
        CHECK(p_copy.a == 4);

        CHECK(p_copy == p_move);
    }

    SECTION("defaulted assignments")
    {
        auto p = yuva_8(1, 2, 3, 4);
        yuva_8 p_copy;
        p_copy = p;
        yuva_8 p_move;
        p_move = std::move(p);

        CHECK(p_copy.y == 1);
        CHECK(p_copy.u == 2);
        CHECK(p_copy.v == 3);
        CHECK(p_copy.a == 4);

        CHECK(p_copy == p_move);
    }

    SECTION("ostream")
    {
        auto p = yuva_8(1, 2, 3, 4);
        std::ostringstream ss;
        ss << p;
        CHECK(ss.str() == "yuva(1, 2, 3, 4)");
    }
}

TEST_CASE("pixel_types_test - g")
{
    SECTION("default constructor")
    {
        auto p = g_8();
        REQUIRE(std::is_same_v<g_8::value_t, uint8_t>);
        CHECK(p.v == g_8::value_t{});
    }

    SECTION("constructor from value")
    {
        auto p = g_8(128);
        CHECK(p.v == 128);
    }

    SECTION("converting constructor")
    {
        auto p = g_f( uint8_t{ 127 } );
        CHECK(p.v == 127);
    }

    SECTION("assignment from value")
    {
        g_8 p;
        p = 128;
        CHECK(p.v == 128);
    }

    SECTION("comparison operators")
    {
        auto p1 = g_8(128);
        auto p2 = g_8(127);
        CHECK(p1 != p2);
        p2.v = 128;
        CHECK(p1 == p2);
    }

    SECTION("defaulted constructors")
    {
        auto p = g_8(127);
        auto p_copy{ p };
        auto p_move{ std::move(p) };

        CHECK(p_copy.v == 127);
        CHECK(p_copy == p_move);
    }

    SECTION("defaulted assignments")
    {
        auto p = g_8(127);
        g_8 p_copy;
        p_copy = p;
        g_8 p_move;
        p_move = std::move(p);

        CHECK(p_copy.v == 127);
        CHECK(p_copy == p_move);
    }

    SECTION("ostream")
    {
        auto p = g_16(32565);
        std::ostringstream ss;
        ss << p;
        CHECK(ss.str() == "g(32565)");
    }
}
