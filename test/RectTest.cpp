
// catch
#include "catch.hpp"

// std
#include <sstream>
#include <string>

// to be tested
#include "Rect.h"

TEST_CASE("RectTest - DefaultTest")
{
    Rect r;
    REQUIRE(r.bottomLeft()  ==  Point2<int32_t>());
    REQUIRE(r.bottomRight() == Point2<int32_t>());
    REQUIRE(r.topLeft()     == Point2<int32_t>());
    REQUIRE(r.topRight()    == Point2<int32_t>());
    REQUIRE(r.width()  == 0);
    REQUIRE(r.height() == 0);
    REQUIRE(r.area()   == 0);
}

TEST_CASE("RectTest - EqualityTest")
{
    REQUIRE( Rect(Point2<int32_t>(5,5), Size(10,10)) == Rect(Point2<int32_t>(5,5), Size(10,10)) );
    REQUIRE( Rect(Point2<int32_t>(1,5), Size(10,10)) != Rect(Point2<int32_t>(5,5), Size(10,10)) );
    REQUIRE( Rect(Point2<int32_t>(5,5), Size(1,10)) != Rect(Point2<int32_t>(5,5), Size(10,10)) );
}

TEST_CASE("RectTest - FromSizeTest")
{
    REQUIRE( Rect::from_size(Size(10,10)) == Rect(Point2<int32_t>(0,0), Size(10,10)) );
}

TEST_CASE("RectTest - CopyTest")
{
    Rect r1(Point2<int32_t>(5,5), Size(10,10));
    Rect r2(r1);

    REQUIRE(r1 == r2);
}

TEST_CASE("RectTest - CornerTest")
{
    Rect r1(Point2<int32_t>(5,5), Size(10,10));
    REQUIRE(r1.bottomLeft()  == Point2<int32_t>(5,5));
    REQUIRE(r1.bottomRight() == Point2<int32_t>(15,5));
    REQUIRE(r1.topLeft()     == Point2<int32_t>(5,15));
    REQUIRE(r1.topRight()    == Point2<int32_t>(15,15));

    Rect r2(Point2<int32_t>(), Size(10,10));
    REQUIRE(r2.bottomLeft()  == Point2<int32_t>(0,0));
    REQUIRE(r2.bottomRight() == Point2<int32_t>(10,0));
    REQUIRE(r2.topLeft()     == Point2<int32_t>(0,10));
    REQUIRE(r2.topRight()    == Point2<int32_t>(10,10));
}

TEST_CASE("RectTest - WithinTest")
{
    Rect r(Point2<int32_t>(5,5), Size(10,10));
    REQUIRE( r.within( Rect( Point2<int32_t>(5,5), Size(10,10))) );
    REQUIRE( r.within( Rect( Point2<int32_t>(0,0), Size(20,20))) );
    REQUIRE_FALSE( r.within( Rect( Point2<int32_t>(6,0), Size(8,8))) );
    REQUIRE_FALSE( r.within( Rect( Point2<int32_t>(0,6), Size(8,8))) );
    REQUIRE_FALSE( r.within( Rect( Point2<int32_t>(0,0), Size(10,20))) );
    REQUIRE_FALSE( r.within( Rect( Point2<int32_t>(0,0), Size(20,10))) );
}

TEST_CASE("RectTest - ContainsTest")
{
    Rect r(Point2<int32_t>(5,5), Size(10,10));
    REQUIRE( r.contains( Rect( Point2<int32_t>(5,5), Size(10,10))) );
    REQUIRE( r.contains( Rect( Point2<int32_t>(6,6), Size(8,8))) );
    REQUIRE_FALSE( r.contains( Rect( Point2<int32_t>(4,6), Size(8,8))) );
    REQUIRE_FALSE( r.contains( Rect( Point2<int32_t>(6,4), Size(8,8))) );
    REQUIRE_FALSE( r.contains( Rect( Point2<int32_t>(6,6), Size(8,10))) );
    REQUIRE_FALSE( r.contains( Rect( Point2<int32_t>(6,6), Size(10,8))) );
}

TEST_CASE("RectTest - OStreamTest")
{
    std::stringstream ss;
    Rect r( Point2<int32_t>(1,1), Size(20, 20) );
    ss << r;

    REQUIRE( ss.str() == "(1,1) -> [20,20]" );
}
