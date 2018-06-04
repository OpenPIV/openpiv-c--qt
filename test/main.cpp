
// gtest
#include "gtest/gtest.h"

// to be tested
#include "PointTest.h"
#include "SizeTest.h"
#include "RectTest.h"
#include "ImageTest.h"
#include "ImageViewTest.h"
#include "UtilTest.h"
#include "ImageLoaderTest.h"
#include "PixelTypesTest.h"

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
