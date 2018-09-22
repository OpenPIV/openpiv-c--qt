
// catch
#include "catch.hpp"

// std
#include <thread>

// local
#include "TestUtils.h"

// to be tested
#include "ImageAlgos.h"
#include "ImageLoader.h"
#include "ImageUtils.h"

TEST_CASE("ImageAlgosTest - FFTTest")
{
    GFImage im{ 256, 256 };
    fill( im, []( uint32_t w, uint32_t h ){ return (w/2)%2 ? 1.0 : 0.0; } );

    std::shared_ptr<ImageLoader> writer{ ImageLoader::findLoader("image/x-portable-anymap") };
    {
        std::fstream os( "fft-input.pgm", std::ios_base::trunc | std::ios_base::out );
        writer->save( os, im );
    }

    FFT fft( im.size() );
    GFImage output{ fft.transform( im ) };

    {
        std::fstream os( "fft-output.pgm", std::ios_base::trunc | std::ios_base::out );
        writer->save( os, output );
    }
}


TEST_CASE("ImageAlgosTest - FFT Different Size")
{
    GFImage im{ 256, 256 };
    fill( im, []( uint32_t w, uint32_t h ){ return (w/2)%2 ? 1.0 : 0.0; } );

    FFT fft( { 512, 512 } );
    _REQUIRE_THROWS_MATCHES( fft.transform( im ), std::runtime_error, Contains( "image size is different" ) );
}

TEST_CASE("ImageAlgosTest - FFT non-power-of-two size")
{
    _REQUIRE_THROWS_MATCHES( FFT( { 512, 400 } ), std::runtime_error, Contains( "power of 2" ) );
    _REQUIRE_THROWS_MATCHES( FFT( { 400, 512 } ), std::runtime_error, Contains( "power of 2" ) );
}

TEST_CASE("ImageAlgosTest - FFT wrong thread")
{
    GFImage im{ 256, 256 };
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

TEST_CASE("ImageAlgosTest - CrossCorrelationTest")
{
    // load images
    auto im = loadFromFile< GF >( "corr_a.tiff" );
    std::cout << "im: " << im << "\n";

    // extract a couple of small windows
    Size s{ 128, 128 };
    auto view_a{ createImageView( im, Rect{ {20, 20}, s } ) };
    auto view_b{ createImageView( im, Rect{ {30, 30}, s } ) };

    FFT fft( view_a.size() );

    // inspect individual FFTs
    {
        REQUIRE( saveToFile( "cross-correlate-a-input.pgm", view_a ) );

        auto [real, imag] = split_to_channels( fft.transform( view_a ) );
        REQUIRE( saveToFile( "cross-correlate-a-real.pgm", real ) );
        REQUIRE( saveToFile( "cross-correlate-a-imag.pgm", imag ) );
    }

    {
        REQUIRE( saveToFile( "cross-correlate-b-input.pgm", view_b ) );

        auto [real, imag] = split_to_channels( fft.transform( view_b ) );
        REQUIRE( saveToFile( "cross-correlate-b-real.pgm", real ) );
        REQUIRE( saveToFile( "cross-correlate-b-imag.pgm", imag ) );
    }

    // prepare & correlate
    GFImage output{ fft.cross_correlate( view_a, view_b ) };

    // write output
    REQUIRE( saveToFile( "cross-correlate-output.pgm", output ) );
}

TEST_CASE("ImageAlgosTest - FFTRealTest")
{
    // load images
    auto im_a = loadFromFile< GF >( "corr_a.tiff" );
    std::cout << "im_a: " << im_a << "\n";

    // extract a couple of small windows
    Size s{ 128, 128 };
    auto view_a{ createImageView( im_a, Rect{ {20, 20}, s } ) };

    REQUIRE( saveToFile( "fft_corr_a_input.pgm", view_a ) );

    // prepare & corrrelate
    FFT fft( view_a.size() );
    GFImage output{ fft.transform( view_a ) };

    // write output
    REQUIRE( saveToFile( "fft_corr_a_output.pgm", output) );
}

TEST_CASE("ImageAlgosTest - AutoCorrelationTest")
{
    // load images
    auto im = loadFromFile< GF >( "corr_a.tiff" );

    std::shared_ptr<ImageLoader> writer{ ImageLoader::findLoader("image/x-portable-anymap") };

    Size s{ 128, 128 };
    auto view_a{ createImageView( im, Rect{ {20, 20}, s } ) };
    auto view_b{ createImageView( im, Rect{ {30, 30}, s } ) };

    // combine
    GFImage data{ view_a.width(), view_a.height() };
    data = view_a + view_b;
    REQUIRE( saveToFile( "auto-correlate-input.pgm", data) );

    // prepare & corrrelate
    FFT fft( data.size() );
    GFImage output{ fft.auto_correlate( data ) };
    REQUIRE( saveToFile( "auto-correlate-output.pgm", output) );
}
