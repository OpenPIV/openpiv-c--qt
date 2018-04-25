
// gtest
#include "gtest/gtest.h"

// std
#include <sstream>
#include <string>

// to be tested
#include "Size.h"

TEST(SizeTest, DefaultTest)
{
    Size s;
    ASSERT_EQ(s.width(), 0);
    ASSERT_EQ(s.height(), 0);
    ASSERT_EQ(s.area(), 0);
}

TEST(SizeTest, BasicTest)
{
    Size s(10, 20);

    ASSERT_EQ(s.width(), 10);
    ASSERT_EQ(s.height(), 20);
    ASSERT_EQ(s.area(), 200);
}

TEST(SizeTest, CopyTest)
{
    Size s1(10, 20);
    Size s2(s1);

    ASSERT_EQ(s2.width(), 10);
    ASSERT_EQ(s2.height(), 20);
}

TEST(SizeTest, AssignTest)
{
    Size s1(10, 20);
    Size s2(40, 50);

    s2=s1;

    ASSERT_EQ(s2.width(), 10);
    ASSERT_EQ(s2.height(), 20);
}

TEST(SizeTest, MoveTest)
{
    Size s1(10, 20);
    Size s2(std::move(s1));

    ASSERT_EQ(s2.width(), 10);
    ASSERT_EQ(s2.height(), 20);
}

TEST(SizeTest, EqualityTest)
{
    Size s1(10, 20);

    ASSERT_EQ(s1, Size(10, 20));
    ASSERT_NE(s1, Size(20, 10));
}

TEST(SizeTest, OStreamTest)
{
    std::stringstream ss;
    Size s1(10, 20);
    ss << s1;

    ASSERT_EQ( ss.str(), "[10,20]" );
}
