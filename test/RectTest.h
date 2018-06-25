
// gtest
#include "gtest/gtest.h"

// std
#include <sstream>
#include <string>

// to be tested
#include "Rect.h"

TEST(RectTest, DefaultTest)
{
    Rect r;
    ASSERT_EQ(r.bottomLeft(),  Point2<int32_t>());
    ASSERT_EQ(r.bottomRight(), Point2<int32_t>());
    ASSERT_EQ(r.topLeft(),     Point2<int32_t>());
    ASSERT_EQ(r.topRight(),    Point2<int32_t>());
    ASSERT_EQ(r.width(),  0);
    ASSERT_EQ(r.height(), 0);
    ASSERT_EQ(r.area(),   0);
}

TEST(RectTest, EqualityTest)
{
    ASSERT_EQ( Rect(Point2<int32_t>(5,5), Size(10,10)), Rect(Point2<int32_t>(5,5), Size(10,10)) );
    ASSERT_NE( Rect(Point2<int32_t>(1,5), Size(10,10)), Rect(Point2<int32_t>(5,5), Size(10,10)) );
    ASSERT_NE( Rect(Point2<int32_t>(5,5), Size(1,10)), Rect(Point2<int32_t>(5,5), Size(10,10)) );
}

TEST(RectTest, FromSizeTest)
{
    ASSERT_EQ( Rect::fromSize(Size(10,10)), Rect(Point2<int32_t>(0,0), Size(10,10)) );
}

TEST(RectTest, CopyTest)
{
    Rect r1(Point2<int32_t>(5,5), Size(10,10));
    Rect r2(r1);

    ASSERT_EQ(r1, r2);
}

TEST(RectTest, CornerTest)
{
    Rect r1(Point2<int32_t>(5,5), Size(10,10));
    ASSERT_EQ(r1.bottomLeft(),  Point2<int32_t>(5,5));
    ASSERT_EQ(r1.bottomRight(), Point2<int32_t>(15,5));
    ASSERT_EQ(r1.topLeft(),     Point2<int32_t>(5,15));
    ASSERT_EQ(r1.topRight(),    Point2<int32_t>(15,15));

    Rect r2(Point2<int32_t>(), Size(10,10));
    ASSERT_EQ(r2.bottomLeft(),  Point2<int32_t>(0,0));
    ASSERT_EQ(r2.bottomRight(), Point2<int32_t>(10,0));
    ASSERT_EQ(r2.topLeft(),     Point2<int32_t>(0,10));
    ASSERT_EQ(r2.topRight(),    Point2<int32_t>(10,10));
}

TEST(RectTest, WithinTest)
{
    Rect r(Point2<int32_t>(5,5), Size(10,10));
    ASSERT_TRUE( r.within( Rect( Point2<int32_t>(5,5), Size(10,10))) );
    ASSERT_TRUE( r.within( Rect( Point2<int32_t>(0,0), Size(20,20))) );
    ASSERT_FALSE( r.within( Rect( Point2<int32_t>(6,0), Size(8,8))) );
    ASSERT_FALSE( r.within( Rect( Point2<int32_t>(0,6), Size(8,8))) );
    ASSERT_FALSE( r.within( Rect( Point2<int32_t>(0,0), Size(10,20))) );
    ASSERT_FALSE( r.within( Rect( Point2<int32_t>(0,0), Size(20,10))) );
}

TEST(RectTest, ContainsTest)
{
    Rect r(Point2<int32_t>(5,5), Size(10,10));
    ASSERT_TRUE( r.contains( Rect( Point2<int32_t>(5,5), Size(10,10))) );
    ASSERT_TRUE( r.contains( Rect( Point2<int32_t>(6,6), Size(8,8))) );
    ASSERT_FALSE( r.contains( Rect( Point2<int32_t>(4,6), Size(8,8))) );
    ASSERT_FALSE( r.contains( Rect( Point2<int32_t>(6,4), Size(8,8))) );
    ASSERT_FALSE( r.contains( Rect( Point2<int32_t>(6,6), Size(8,10))) );
    ASSERT_FALSE( r.contains( Rect( Point2<int32_t>(6,6), Size(10,8))) );
}

TEST(RectTest, OStreamTest)
{
    std::stringstream ss;
    Rect r( Point2<int32_t>(1,1), Size(20, 20) );
    ss << r;

    ASSERT_EQ( ss.str(), "(1,1) -> [20,20]" );
}
