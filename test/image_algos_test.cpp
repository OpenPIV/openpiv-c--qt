
// catch
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>

// std
#include <thread>

// local
#include "test_utils.h"

// to be tested
#include "algos/fft.h"
#include "loaders/image_loader.h"
#include "core/image_utils.h"

using namespace std::string_literals;
using namespace Catch;
using namespace Catch::Matchers;
using namespace openpiv::core;
using namespace openpiv::algos;

TEST_CASE("image_algos_test - FFTTest")
{
    // generate sinusoidal pattern
    gf_image im{ 256, 256 };
    fill( im, []( uint32_t w, uint32_t h ){ return 128 * std::sin( 2*M_PI*w/8 + 2*M_PI*h/8 ); } );

    // save
    REQUIRE( save_to_file( "fft-input.pgm", im ) );

    // transform
    FFT fft( im.size() );
    cf_image output{ fft.transform( im, direction::FORWARD ) };

    REQUIRE( save_to_file( "fft-output.pgm", gf_image( output ) ) );

    // two peaks; relies on quadrants being unswapped i.e. zero
    // frequency is at edges
    for ( uint32_t y=0; y<output.height(); ++y )
        for ( uint32_t x=0; x<output.width(); ++x )
            if ( x == 32 && y == 32 )
            {
                auto p = point2<uint32_t>{ x, y };
                REQUIRE( (output[ p ] != c_f{}) );
            }
            else if ( x == 224 && y == 224 )
            {
                auto p = point2<uint32_t>{ x, y };
                REQUIRE( (output[ p ] != c_f{}) );
            }
            else
            {
                auto p = point2<uint32_t>{ x, y };
                REQUIRE_THAT( (output[ p ].abs_sqr()), WithinAbs(0, 1e-9) );
            }

    // inverse transform
    gf_image g{ real( fft.transform( output, direction::REVERSE ) ) };

    REQUIRE( save_to_file( "fft-reverse.pgm", g) );
}

TEST_CASE("image_algos_test - FFT two real images")
{
    // generate sinusoidal patterns
    gf_image a{ 256, 256 };
    fill( a, []( uint32_t w, uint32_t h ){ return 128 * std::sin( 2*M_PI*w/8 + 2*M_PI*h/8 ); } );

    gf_image b{ 256, 256 };
    fill( b,
          [width=b.width()]( uint32_t w, uint32_t h ) {
              return 128 * std::sin( 2*M_PI*(width-w)/8 + 2*M_PI*h/8 );
          } );

    // save
    REQUIRE( save_to_file( "fft-real-input-a.pgm", a ) );
    REQUIRE( save_to_file( "fft-real-input-b.pgm", b ) );

    // transform
    FFT fft( a.size() );
    auto [a_fft, b_fft] = fft.transform_real( a, b, direction::FORWARD );

    REQUIRE( save_to_file( "fft-real-output-a.pgm", gf_image( a_fft ) ) );
    REQUIRE( save_to_file( "fft-real-output-b.pgm", gf_image( b_fft ) ) );

    // two peaks; relies on quadrants being unswapped i.e. zero
    // frequency is at edges
    auto checker =
        [](const cf_image& im, point2<uint32_t> a, point2<uint32_t> b)
        {
            for ( uint32_t y=0; y<im.height(); ++y )
                for ( uint32_t x=0; x<im.width(); ++x )
                {
                    auto p = point2<uint32_t>{ x, y };
                    if ( p == a || p == b )
                    {
                        REQUIRE( (im[ p ] != c_f{}) );
                    }
                    else
                    {
                        REQUIRE_THAT( (im[ p ].abs_sqr()), WithinAbs(0, 1e-9) );
                    }
                }
        };

    checker(a_fft, {32, 32}, {224, 224});
    checker(b_fft, {32, 224}, {224, 32});
}

TEST_CASE("image_algos_test - FFT Different Size")
{
    gf_image im{ 256, 256 };
    fill( im, []( uint32_t w, uint32_t ){ return (w/2)%2 ? 1.0 : 0.0; } );

    FFT fft( { 512, 512 } );
    _REQUIRE_THROWS_MATCHES( fft.transform( im ),
                             std::runtime_error,
                             ContainsSubstring( "image size is different"s, CaseSensitive::No ) );
}

TEST_CASE("image_algos_test - FFT non-power-of-two size")
{
    _REQUIRE_THROWS_MATCHES( FFT( { 512, 400 } ),
                             std::runtime_error,
                             ContainsSubstring( "power of 2"s, CaseSensitive::No ) );
    _REQUIRE_THROWS_MATCHES( FFT( { 400, 512 } ),
                             std::runtime_error,
                             ContainsSubstring( "power of 2"s, CaseSensitive::No ) );
}

TEST_CASE("image_algos_test - cross_correlation_test")
{
    // load images
    auto im = load_from_file< g_f >( "corr_a.tiff" );

    // extract a couple of small windows
    size s{ 128, 128 };
    auto view_a = create_image_view( im, rect{ {20, 20}, s } );
    auto view_b = create_image_view( im, rect{ {20, 25}, s } );

    REQUIRE( save_to_file( "cross-correlate-a-input.pgm", view_a ) );
    REQUIRE( save_to_file( "cross-correlate-b-input.pgm", view_b ) );

    // prepare & correlate
    FFT fft( view_a.size() );
    gf_image output{ fft.cross_correlate( view_a, view_b ) };

    // write output
    REQUIRE( save_to_file( "cross-correlate-output.pgm", output ) );
}

TEST_CASE("image_algos_test - auto_correlation_test")
{
    // load images
    auto im = load_from_file< g_f >( "corr_a.tiff" );

    // extract a couple of small windows
    size s{ 128, 128 };
    auto view_a = create_image_view( im, rect{ {20, 20}, s } );
    auto view_b = create_image_view( im, rect{ {20, 25}, s } );

    // combine
    gf_image data{ view_a.width(), view_a.height() };
    data = view_a + view_b;
    REQUIRE( save_to_file( "auto-correlate-input.pgm", data) );

    // prepare & corrrelate
    FFT fft( data.size() );
    gf_image output{ fft.auto_correlate( data ) };
    REQUIRE( save_to_file( "auto-correlate-output.pgm", output) );
}

TEST_CASE("image_algos_test - fft_real_test")
{
    // load images
    auto im_a = load_from_file< g_f >( "corr_a.tiff" );

    // extract a couple of small windows
    size s{ 128, 128 };
    auto view_a = create_image_view( im_a, rect{ {20, 20}, s } );
    auto view_b = create_image_view( im_a, rect{ {20, 25}, s } );

    // combine
    gf_image data{ s };
    data = view_a + view_b;

    REQUIRE( save_to_file( "fft_corr_a_input.pgm", data ) );

    // transform
    FFT fft( data.size() );
    cf_image output;
    output = fft.transform( data, direction::FORWARD );

    // remove DC
    output[ {0, 0} ] = c_f{};

    // transformed
    swap_quadrants( output );
    REQUIRE( save_to_file( "fft_corr_a_fourier.pgm", gf_image{ output }) );

    // complex conjugate
    for ( uint32_t h=0; h<output.height(); ++h )
        for ( uint32_t w=0; w<output.width(); ++w )
            output[ {w, h} ] = output[ {w, h} ] * output[ {w, h} ].conj();

    REQUIRE( save_to_file( "fft_corr_a_conj.pgm", gf_image{ output }) );

    // inverse
    output = fft.transform( gf_image{ output }, direction::REVERSE );
    swap_quadrants( output );

    // write output
    REQUIRE( save_to_file( "fft_corr_a_output.pgm", gf_image{ output }) );
}

