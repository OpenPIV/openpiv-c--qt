
// catch
#include "catch.hpp"

// std
#include <thread>

// local
#include "test_utils.h"

// to be tested
#include "image_algos.h"
#include "image_loader.h"
#include "image_utils.h"

using namespace Catch;

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
                std::cout << p << ": " << output[ p ] << "\n";
                REQUIRE( (output[ p ] != c_f{}) );
            }
            else if ( x == 224 && y == 224 )
            {
                auto p = point2<uint32_t>{ x, y };
                std::cout << p << ": " << output[ p ] << "\n";
                REQUIRE( (output[ p ] != c_f{}) );
            }
            else
            {
                auto p = point2<uint32_t>{ x, y };
                std::cout << p << ": " << output[ p ] << "\n";
                REQUIRE( (output[ p ].abs_sqr()) == Approx(0).margin(1e-9) );
            }

    // inverse transform
    gf_image g{ real( fft.transform( output, direction::REVERSE ) ) };

    REQUIRE( save_to_file( "fft-reverse.pgm", g) );
}


TEST_CASE("image_algos_test - FFT Different Size")
{
    gf_image im{ 256, 256 };
    fill( im, []( uint32_t w, uint32_t h ){ return (w/2)%2 ? 1.0 : 0.0; } );

    FFT fft( { 512, 512 } );
    _REQUIRE_THROWS_MATCHES( fft.transform( im ), std::runtime_error, Contains( "image size is different" ) );
}

TEST_CASE("image_algos_test - FFT non-power-of-two size")
{
    _REQUIRE_THROWS_MATCHES( FFT( { 512, 400 } ), std::runtime_error, Contains( "power of 2" ) );
    _REQUIRE_THROWS_MATCHES( FFT( { 400, 512 } ), std::runtime_error, Contains( "power of 2" ) );
}

TEST_CASE("image_algos_test - FFT wrong thread")
{
    gf_image im{ 256, 256 };
    fill( im, []( uint32_t w, uint32_t h ){ return (w/2)%2 ? 1.0 : 0.0; } );

    FFT fft( im.size() );
    std::thread t{ [&im, &fft](){
            _REQUIRE_THROWS_MATCHES(
                fft.transform( im ),
                std::runtime_error,
                Contains( "wrong thread" ) );
        } };
    t.join();
}

TEST_CASE("image_algos_test - cross_correlation_test")
{
    // load images
    auto im = load_from_file< g_f >( "corr_a.tiff" );

    // extract a couple of small windows
    size s{ 128, 128 };
    auto view_a{ create_image_view( im, rect{ {20, 20}, s } ) };
    auto view_b{ create_image_view( im, rect{ {20, 25}, s } ) };

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
    auto view_a{ create_image_view( im, rect{ {20, 20}, s } ) };
    auto view_b{ create_image_view( im, rect{ {20, 25}, s } ) };

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
    std::cout << "im_a: " << im_a << "\n";

    // extract a couple of small windows
    size s{ 128, 128 };
    auto view_a{ create_image_view( im_a, rect{ {20, 20}, s } ) };
    auto view_b{ create_image_view( im_a, rect{ {20, 25}, s } ) };

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

