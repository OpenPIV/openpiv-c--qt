
// catch
#include "catch.hpp"

// std
#include <sstream>
#include <string>
#include <tuple>

// local
#include "TestUtils.h"

// to be tested
#include "ImageExpression.h"
#include "Image.h"
#include "ImageUtils.h"
#include "ImageStats.h"

class T_
{
public:
    using type = uint32_t;
    constexpr type operator[](size_t) const { return type{1}; }
};


TEST_CASE("ImageExpressionTest - ImageExpressionInputTest")
{
    auto ie = ImageExpression<plus_op<T_::type>, T_, T_>{ T_{}, T_{} };
    REQUIRE( is_ie_inputtype<decltype(ie)>::value );
}

TEST_CASE("ImageExpressionTest - ImageInputTest")
{
    using type = G8Image;
    REQUIRE( is_ie_inputtype<type>::value );
}

TEST_CASE("ImageExpressionTest - ImageViewInputTest")
{
    using type = ImageView<G8>;
    REQUIRE( is_ie_inputtype<type>::value );
}

TEST_CASE("ImageExpressionTest - NegativeInputTest")
{
    using type = uint32_t;
    REQUIRE( !is_ie_inputtype<type>::value );
}

TEST_CASE("ImageExpressionTest - AddConstTest")
{
    G8Image im; G8 v;
    std::tie( im, v ) = createAndFill( Size( 200, 100 ), 128_g8 );

    G8Image im2( 200, 100 );
    im2 = im + 1_g8;
    auto [min, max] = findImageRange( im2 );
    REQUIRE(min == 129);
    REQUIRE(max == 129);
}

TEST_CASE("ImageExpressionTest - ConjugateTest")
{
    CFImage im;
    std::tie( im, std::ignore ) = createAndFill( Size( 200, 200 ), CF{ 0, 1 } );

    CFImage im2( 200, 200 );
    im2 = conj(im);

    for ( decltype(im.pixel_count()) i=0; i<im.pixel_count(); ++i )
        REQUIRE( im[i] == im2[i].conj() );
}

TEST_CASE("ImageExpressionTest - AddImageTest")
{
    G8Image im1; G8 v1;
    std::tie( im1, v1 ) = createAndFill( Size( 200, 100 ), 128_g8 );

    G8Image im2; G8 v2;
    std::tie( im2, v2 ) = createAndFill( Size( 200, 100 ), 127_g8 );

    G8Image im3( 200, 100 );
    im3 = im1 + im2;

    auto [min, max] = findImageRange( im3 );
    REQUIRE(min == 255);
    REQUIRE(max == 255);
}

TEST_CASE("ImageExpressionTest - AddImageConstTest")
{
    G8Image im1; G8 v1;
    std::tie( im1, v1 ) = createAndFill( Size( 200, 100 ), 127_g8 );

    G8Image im2; G8 v2;
    std::tie( im2, v2 ) = createAndFill( Size( 200, 100 ), 127_g8 );

    G8Image im3( 200, 100 );
    im3 = im1 + im2 + 1_g8;

    auto [min, max] = findImageRange( im3 );
    REQUIRE(min == 255);
    REQUIRE(max == 255);
}

TEST_CASE("ImageExpressionTest - ScaleTest")
{
    std::ifstream is("A_00001_a.tif", std::ios::binary);
    REQUIRE(is.is_open());

    std::shared_ptr<ImageLoader> loader{ ImageLoader::findLoader(is) };
    REQUIRE(!!loader);

    G16Image im;
    loader->load( is, im );

    // scale
    auto [min, max] = findImageRange( im );
    auto scale{ (max == min) ? G16::max() : G16::max()/(max-min) };
    std::cout << "min: " << min << ", max: " << max << ", scale: " << scale << "\n";

    im = G16{ scale } * im - G16{ min };

    std::tie(min, max) = findImageRange( im );
    std::cout << "min: " << min << ", max: " << max << "\n";

    // write data
    std::shared_ptr<ImageLoader> writer{ ImageLoader::findLoader("image/x-portable-anymap") };
    REQUIRE(!!writer);
    REQUIRE(writer->name() == std::string("image/x-portable-anymap"));

    std::fstream os( "A_00001_a.pgm", std::ios_base::trunc | std::ios_base::out );
    writer->save( os, im );
}
