
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
    ASSERT_EQ(r.bottomLeft(),  Int2DPoint());
    ASSERT_EQ(r.bottomRight(), Int2DPoint());
    ASSERT_EQ(r.topLeft(),     Int2DPoint());
    ASSERT_EQ(r.topRight(),    Int2DPoint());
    ASSERT_EQ(r.width(),  0);
    ASSERT_EQ(r.height(), 0);
    ASSERT_EQ(r.area(),   0);
}

TEST(RectTest, EqualityTest)
{
    ASSERT_EQ( Rect(Int2DPoint(5,5), Size(10,10)), Rect(Int2DPoint(5,5), Size(10,10)) );
    ASSERT_NE( Rect(Int2DPoint(1,5), Size(10,10)), Rect(Int2DPoint(5,5), Size(10,10)) );
    ASSERT_NE( Rect(Int2DPoint(5,5), Size(1,10)), Rect(Int2DPoint(5,5), Size(10,10)) );
}

TEST(RectTest, CopyTest)
{
    Rect r1(Int2DPoint(5,5), Size(10,10));
    Rect r2(r1);

    ASSERT_EQ(r1, r2);
}

TEST(RectTest, CornerTest)
{
    Rect r(Int2DPoint(5,5), Size(10,10));
    ASSERT_EQ(r.bottomLeft(),  Int2DPoint(5,5));
    ASSERT_EQ(r.bottomRight(), Int2DPoint(15,5));
    ASSERT_EQ(r.topLeft(),     Int2DPoint(5,15));
    ASSERT_EQ(r.topRight(),    Int2DPoint(15,15));
}

TEST(RectTest, OStreamTest)
{
    std::stringstream ss;
    Rect r( Int2DPoint(1,1), Size(20, 20) );
    ss << r;

    ASSERT_EQ( ss.str(), "(1,1) -> [20,20]" );
}
