
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
    G8Image im; uint8_t v;
    std::tie( im, v ) = createAndFill( Size( 200, 200 ), 0);

    auto iv = createImageView( im, Rect::fromSize(Size(100, 100)) );
    ASSERT_EQ(iv.width(), 100);
    ASSERT_EQ(iv.height(), 100);
    ASSERT_EQ(iv.pixel_count(), 10000);
}

TEST(ImageViewTest, ResizeTest)
{
    G8Image im; uint8_t v;
    std::tie( im, v ) = createAndFill( Size( 200, 200 ), 128);
    auto iv = createImageView( im, Rect::fromSize(Size(100, 100)) );

    iv.resize(150, 150);
    ASSERT_EQ(iv.width(), 150);
    ASSERT_EQ(iv.height(), 150);
    ASSERT_EQ(iv.pixel_count(), 150*150);

    bool result = true;
    for ( uint32_t i=0; i<iv.pixel_count(); ++i )
        result &= (iv[i] == v);

    ASSERT_EQ( result, true );
}

TEST(ImageViewTest, ResizeFailureTest)
{
    G8Image im; uint8_t v;
    std::tie( im, v ) = createAndFill( Size( 200, 200 ), 128);
    auto iv = createImageView( im, Rect::fromSize(Size(100, 100)) );

    _ASSERT_DEATH( iv.resize(300, 300), std::out_of_range, "not contained within image" );
}

TEST(ImageViewTest, CopyTest)
{
    G8Image im; uint8_t v;
    std::tie( im, v ) = createAndFill( Size( 200, 200 ), 0);

    auto iv1 = createImageView( im, Rect::fromSize(Size(100, 100)) );
    auto iv2{ iv1 };
    ASSERT_EQ(iv1.width(), iv2.width());
    ASSERT_EQ(iv1.height(), iv2.height());
    ASSERT_EQ(iv1.pixel_count(), iv2.pixel_count());

    // compare pointers not values as we want to check we're getting
    // same underlying data from im
    ASSERT_EQ(iv1.line(0), iv2.line(0));
}

TEST(ImageViewTest, MoveTest)
{
    G8Image im; uint8_t v;
    std::tie( im, v ) = createAndFill( Size( 200, 200 ), 0);

    auto iv1 = createImageView( im, Rect::fromSize(Size(100, 100)) );
    auto iv_tmp = createImageView( im, Rect::fromSize(Size(100, 100)) );
    auto iv2{ std::move(iv_tmp) };
    ASSERT_EQ(iv1.width(), iv2.width());
    ASSERT_EQ(iv1.height(), iv2.height());
    ASSERT_EQ(iv1.pixel_count(), iv2.pixel_count());

    // compare pointers not values as we want to check we're getting
    // same underlying data from im
    ASSERT_EQ(iv1.line(0), iv2.line(0));
}

TEST(ImageViewTest, EqualityTest)
{
    G8Image im;
    std::tie( im, std::ignore ) = createAndFill( Size( 200, 200 ), 0);

    auto iv1 = createImageView( im, Rect::fromSize(Size(100, 100)) );
    auto iv2 = createImageView( im, Rect::fromSize(Size(100, 100)) );
    ASSERT_TRUE(iv1 == iv2);
}

TEST(ImageViewTest, SizeInequalityTest)
{
    G8Image im;
    std::tie( im, std::ignore ) = createAndFill( Size( 200, 200 ), 0);

    auto iv1 = createImageView( im, Rect::fromSize(Size(100, 101)) );
    auto iv2 = createImageView( im, Rect::fromSize(Size(100, 100)) );
    ASSERT_TRUE(iv1 != iv2);
}

TEST(ImageViewTest, OriginInequalityTest)
{
    G8Image im;
    std::tie( im, std::ignore ) = createAndFill( Size( 200, 200 ), 0);

    auto iv1 = createImageView( im, Rect( {1, 0}, {100, 100}) );
    auto iv2 = createImageView( im, Rect( {0, 0}, {100, 100}) );
    ASSERT_TRUE(iv1 != iv2);
}

TEST(ImageViewTest, ImageInequalityTest)
{
    G8Image im;
    std::tie( im, std::ignore ) = createAndFill( Size( 200, 200 ), 0);
    G8Image im2;
    std::tie( im2, std::ignore ) = createAndFill( Size( 200, 200 ), 0);

    auto iv1 = createImageView( im,  Rect( {0, 0}, {100, 100}) );
    auto iv2 = createImageView( im2, Rect( {0, 0}, {100, 100}) );
    ASSERT_TRUE(iv1 != iv2);
}

TEST(ImageViewTest, ViewTest)
{
    G8Image im; uint8_t v;
    std::tie( im, v ) = createAndFill( Size( 200, 200 ), 0);

    auto iv = createImageView( im, Rect::fromSize(Size(100, 100)) );
    ASSERT_TRUE( pixel_sum(im) == 0 );
    ASSERT_TRUE( pixel_sum(iv) == 0 );

    im[Point2<uint32_t>()] = 255;
    ASSERT_EQ( pixel_sum(im), pixel_sum(iv) );
}

TEST(ImageViewTest, OffsetTest)
{
    G8Image im; uint8_t v;
    std::tie( im, v ) = createAndFill( Size( 200, 200 ), 0);
    im[ Point2<uint32_t>{20, 20} ] = 255;

    auto iv = createImageView( im, Rect( {20, 20}, {100, 100} ) );
    ASSERT_EQ( iv[ Point2<uint32_t>(0, 0) ], im[ Point2<uint32_t>(20, 20) ] );
}

TEST(ImageViewTest, LineTest)
{
    G8Image im; uint8_t v;
    std::tie( im, v ) = createAndFill( Size( 200, 200 ), 0);
    im[ Point2<uint32_t>{20, 20} ] = 255;

    auto iv = createImageView( im, Rect( {10, 10}, {100, 100} ) );
    ASSERT_EQ( *(iv.line(10) + 10), *(im.line(20) + 20) );
}

TEST(ImageViewTest, ImageFromImageViewTest)
{
    G8Image im; uint8_t v;
    std::tie( im, v ) = createAndFill( Size( 200, 200 ), 1);

    auto iv = createImageView( im, Rect::fromSize(Size(100, 50)) );
    DoubleImage im2{ iv };
    ASSERT_EQ( im2.width(), iv.width() );
    ASSERT_EQ( im2.height(), iv.height() );
    ASSERT_EQ( pixel_sum(im2), 5000 );
}

TEST(ImageViewTest, OutOfBoundsTest)
{
    G8Image im; uint8_t v;
    std::tie( im, v ) = createAndFill( Size( 200, 200 ), 0);

    _ASSERT_DEATH(
        createImageView( im,
                         Rect::fromSize(Size(250, 250)) ),
        std::out_of_range, "not contained within image" );
}

