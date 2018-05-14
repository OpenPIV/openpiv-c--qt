
// gtest
#include "gtest/gtest.h"

// std
#include <sstream>
#include <string>
#include <tuple>

// local
#include "TestUtils.h"

// to be tested
#include "ImageView.h"
#include "ImageUtils.h"

TEST(ImageViewTest, BasicConstructionTest)
{
    UInt8Image im; uint8_t v;
    std::tie( im, v ) = createAndFill( Size( 200, 200 ), 0);

    auto iv = createImageView( im, Rect::fromSize(Size(100, 100)) );
    ASSERT_EQ(iv.width(), 100);
    ASSERT_EQ(iv.height(), 100);
    ASSERT_EQ(iv.pixel_count(), 10000);
}

TEST(ImageViewTest, CopyTest)
{
    UInt8Image im; uint8_t v;
    std::tie( im, v ) = createAndFill( Size( 200, 200 ), 0);

    auto iv1 = createImageView( im, Rect::fromSize(Size(100, 100)) );
    auto iv2{ iv1 };
    ASSERT_EQ(iv1.width(), iv2.width());
    ASSERT_EQ(iv1.height(), iv2.height());
    ASSERT_EQ(iv1.pixel_count(), iv2.pixel_count());

    // compare pointers not values as we want to check we're getting
    // same underlying data from im
    ASSERT_EQ(iv1.data(), iv2.data());
    ASSERT_EQ(iv1.line(0), iv2.line(0));
}

TEST(ImageViewTest, MoveTest)
{
    UInt8Image im; uint8_t v;
    std::tie( im, v ) = createAndFill( Size( 200, 200 ), 0);

    auto iv1 = createImageView( im, Rect::fromSize(Size(100, 100)) );
    auto iv_tmp = createImageView( im, Rect::fromSize(Size(100, 100)) );
    auto iv2{ std::move(iv_tmp) };
    ASSERT_EQ(iv1.width(), iv2.width());
    ASSERT_EQ(iv1.height(), iv2.height());
    ASSERT_EQ(iv1.pixel_count(), iv2.pixel_count());

    // compare pointers not values as we want to check we're getting
    // same underlying data from im
    ASSERT_EQ(iv1.data(), iv2.data());
    ASSERT_EQ(iv1.line(0), iv2.line(0));
}

TEST(ImageViewTest, EqualityTest1)
{
    UInt8Image im;
    std::tie( im, std::ignore ) = createAndFill( Size( 200, 200 ), 0);

    auto iv1 = createImageView( im, Rect::fromSize(Size(100, 100)) );
    auto iv2 = createImageView( im, Rect::fromSize(Size(100, 100)) );
    ASSERT_TRUE(iv1 == iv2);
}

TEST(ImageViewTest, EqualityTest2)
{
    UInt8Image im;
    std::tie( im, std::ignore ) = createAndFill( Size( 200, 200 ), 0);

    auto iv1 = createImageView( im, Rect::fromSize(Size(100, 101)) );
    auto iv2 = createImageView( im, Rect::fromSize(Size(100, 100)) );
    ASSERT_TRUE(iv1 != iv2);
}

TEST(ImageViewTest, EqualityTest3)
{
    UInt8Image im;
    std::tie( im, std::ignore ) = createAndFill( Size( 200, 200 ), 0);

    auto iv1 = createImageView( im, Rect( {1, 0}, {100, 100}) );
    auto iv2 = createImageView( im, Rect( {0, 0}, {100, 100}) );
    ASSERT_TRUE(iv1 != iv2);
}

TEST(ImageViewTest, EqualityTest4)
{
    UInt8Image im;
    std::tie( im, std::ignore ) = createAndFill( Size( 200, 200 ), 0);
    UInt8Image im2;
    std::tie( im2, std::ignore ) = createAndFill( Size( 200, 200 ), 0);

    auto iv1 = createImageView( im,  Rect( {0, 0}, {100, 100}) );
    auto iv2 = createImageView( im2, Rect( {0, 0}, {100, 100}) );
    ASSERT_TRUE(iv1 != iv2);
}

TEST(ImageViewTest, ViewTest)
{
    UInt8Image im; uint8_t v;
    std::tie( im, v ) = createAndFill( Size( 200, 200 ), 0);

    auto iv = createImageView( im, Rect::fromSize(Size(100, 100)) );
    ASSERT_TRUE( pixel_sum(im) == 0 );
    ASSERT_TRUE( pixel_sum(iv) == 0 );

    im[UInt2DPoint()] = 255;
    ASSERT_EQ( pixel_sum(im), pixel_sum(iv) );
}

TEST(ImageViewTest, OutOfBoundsTest)
{
    UInt8Image im; uint8_t v;
    std::tie( im, v ) = createAndFill( Size( 200, 200 ), 0);

    ASSERT_DEATH( createImageView( im, Rect::fromSize(Size(250, 250)) ),
                  "image view .* not contained within image .*" );
}
