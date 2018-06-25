
// gtest
#include "gtest/gtest.h"

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

TEST(ImageExpressionTest, ImageExpressionInputTest)
{
    auto ie = ImageExpression<plus_op<T_::type>, T_, T_>{ T_{}, T_{} };
    ASSERT_TRUE( is_ie_inputtype<decltype(ie)>::value );
}

TEST(ImageExpressionTest, ImageInputTest)
{
    using type = G8Image;
    ASSERT_TRUE( is_ie_inputtype<type>::value );
}

TEST(ImageExpressionTest, ImageViewInputTest)
{
    using type = ImageView<G8>;
    ASSERT_TRUE( is_ie_inputtype<type>::value );
}

TEST(ImageExpressionTest, NegativeInputTest)
{
    using type = uint32_t;
    ASSERT_FALSE( is_ie_inputtype<type>::value );
}

TEST(ImageExpressionTest, AddConstTest)
{
    G8Image im; G8 v;
    std::tie( im, v ) = createAndFill( Size( 200, 100 ), 128);

    G8Image im2( 200, 100 );
    im2 = im + 1_g8;
    G8 min, max;
    std::tie(min, max) = findImageRange( im2 );
    ASSERT_EQ(min, 129);
    ASSERT_EQ(max, 129);
}

TEST(ImageExpressionTest, AddImageTest)
{
    G8Image im1; G8 v1;
    std::tie( im1, v1 ) = createAndFill( Size( 200, 100 ), 128);

    G8Image im2; G8 v2;
    std::tie( im2, v2 ) = createAndFill( Size( 200, 100 ), 127);

    G8Image im3( 200, 100 );
    im3 = im1 + im2;
    G8 min, max;
    std::tie(min, max) = findImageRange( im3 );
    ASSERT_EQ(min, 255);
    ASSERT_EQ(max, 255);
}

TEST(ImageExpressionTest, AddImageConstTest)
{
    G8Image im1; G8 v1;
    std::tie( im1, v1 ) = createAndFill( Size( 200, 100 ), 127);

    G8Image im2; G8 v2;
    std::tie( im2, v2 ) = createAndFill( Size( 200, 100 ), 127);

    G8Image im3( 200, 100 );
    im3 = im1 + im2 + 1_g8;

    G8 min, max;
    std::tie(min, max) = findImageRange( im3 );
    ASSERT_EQ(min, 255);
    ASSERT_EQ(max, 255);
}

TEST(ImageExpressionTest, ScaleTest)
{
    std::ifstream is("A_00001_a.tif", std::ios::binary);
    ASSERT_TRUE(is.is_open());

    std::shared_ptr<ImageLoader> loader{ ImageLoader::findLoader(is) };
    ASSERT_TRUE(!!loader);

    G16Image im;
    loader->load( is, im );

    // scale
    G16 min, max;
    std::tie(min, max) = findImageRange( im );
    G16 scale{ (max-min)==0 ? G16::max() : G16::max()/(max-min) };
    std::cout << "min: " << min << ", max: " << max << ", scale: " << scale << "\n";

    im = G16(scale) * im - G16(min);

    std::tie(min, max) = findImageRange( im );
    std::cout << "min: " << min << ", max: " << max << "\n";

    // write data
    std::shared_ptr<ImageLoader> writer{ ImageLoader::findLoader("image/x-portable-anymap") };
    ASSERT_TRUE(!!writer);
    ASSERT_EQ(writer->name(), "image/x-portable-anymap");

    std::fstream os( "A_00001_a.pgm", std::ios_base::trunc | std::ios_base::out );
    writer->save( os, im );
}
