
// gtest
#include "gtest/gtest.h"

// to be tested
#include "PointTest.h"
#include "ImageTest.h"
#include "SizeTest.h"
#include "RectTest.h"

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
