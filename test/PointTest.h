
// gtest
#include "gtest/gtest.h"

// std
#include <sstream>
#include <string>

// to be tested
#include "Point.h"

TEST(PointTest, IntTest)
{
    Int2DPoint p;
    ASSERT_EQ(p[0], Int2DPoint::type());
    ASSERT_EQ(p[1], Int2DPoint::type());

    p = Int2DPoint(1,2);
    ASSERT_EQ(p[0], 1);
    ASSERT_EQ(p[1], 2);
}

TEST(PointTest, ModifyTest)
{
    Int2DPoint p{ 1, 2 };
    p[0] = 3;
    p[1] = 4;

    ASSERT_EQ(p[0], 3);
    ASSERT_EQ(p[1], 4);
}

TEST(PointTest, ConvertTest1)
{
    Double2DPoint p{ 1.0, 2.0 };

    ASSERT_EQ(p[0], 1);
    ASSERT_EQ(p[1], 2);
}

TEST(PointTest, ConvertTest2)
{
    Int2DPoint ip{ 1, 2 };
    Double2DPoint dp{ ip };

    ASSERT_EQ(dp[0], 1);
    ASSERT_EQ(dp[1], 2);
}

TEST(PointTest, AssignmentTest1)
{
    Int2DPoint p1{ 1, 2 };
    Int2DPoint p2;
    p2 = p1;

    ASSERT_EQ(p2[0], 1);
    ASSERT_EQ(p2[1], 2);
}

TEST(PointTest, AssignmentTest2)
{
    Int2DPoint p1{};
    p1 = std::move( Int2DPoint{ 1, 2 } );

    ASSERT_EQ(p1[0], 1);
    ASSERT_EQ(p1[1], 2);
}

TEST(PointTest, EqualityTest)
{
    Int2DPoint p1{ 1, 2 };
    Int2DPoint p2{ 3, 4 };
    Int2DPoint p3{ 1, 2 };

    ASSERT_EQ( p1 != p2, true );
    ASSERT_EQ( p1 == p3, true );
}

TEST(PointTest, OStreamTest)
{
    std::stringstream ss;
    Int2DPoint p1{ 1, 2 };
    ss << p1;

    ASSERT_EQ( ss.str(), std::string("(1,2)") );
}
