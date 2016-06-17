
#pragma once

/// basic 3D point
template < typename T >
class Point
{
public:
    Point() = default;
    Point( const Point& ) = default;
    Point( Point&& ) = default;

    

    

private:
    T x_ = {};
    T y_ = {};
    T z_ = {};
};
