
// gtest
#include "gtest/gtest.h"

// std
#include <sstream>
#include <string>

// to be tested
#include "Point.h"

TEST(PointTest, IntTest)
{
    Point2<uint32_t> p;
    ASSERT_EQ(p[0], Point2<uint32_t>::type());
    ASSERT_EQ(p[1], Point2<uint32_t>::type());

    p = Point2<uint32_t>(1,2);
    ASSERT_EQ(p[0], 1);
    ASSERT_EQ(p[1], 2);
}

TEST(PointTest, ModifyTest)
{
    Point2<uint32_t> p{ 1, 2 };
    p[0] = 3;
    p[1] = 4;

    ASSERT_EQ(p[0], 3);
    ASSERT_EQ(p[1], 4);
}

TEST(PointTest, ConvertTest)
{
    Point2<int32_t> ip{ 1, 2 };
    Point2<double> dp{ ip };

    ASSERT_EQ(dp[0], 1);
    ASSERT_EQ(dp[1], 2);
}

TEST(PointTest, CopyConstructorTest)
{
    Point2<uint32_t> p1{ 1, 2 };
    Point2<uint32_t> p2{ p1 };

    ASSERT_EQ(p2[0], 1);
    ASSERT_EQ(p2[1], 2);
}

TEST(PointTest, MoveConstructorTest)
{
    Point2<uint32_t> p2{ Point2<uint32_t>{ 1, 2 } };

    ASSERT_EQ(p2[0], 1);
    ASSERT_EQ(p2[1], 2);
}

TEST(PointTest, AssignmentTest)
{
    Point2<uint32_t> p1{ 1, 2 };
    Point2<uint32_t> p2;
    p2 = p1;

    ASSERT_EQ(p2[0], 1);
    ASSERT_EQ(p2[1], 2);
}

TEST(PointTest, MoveAssignmentTest)
{
    Point2<uint32_t> p1{};
    p1 = Point2<uint32_t>{ 1, 2 };

    ASSERT_EQ(p1[0], 1);
    ASSERT_EQ(p1[1], 2);
}

TEST(PointTest, EqualityTest)
{
    Point2<uint32_t> p1{ 1, 2 };
    Point2<uint32_t> p2{ 3, 4 };
    Point2<uint32_t> p3{ 1, 2 };

    ASSERT_EQ( p1 != p2, true );
    ASSERT_EQ( p1 == p3, true );
}

TEST(PointTest, OStreamTest)
{
    std::stringstream ss;
    Point2<uint32_t> p1{ 1, 2 };
    ss << p1;

    ASSERT_EQ( ss.str(), std::string("(1,2)") );
}
