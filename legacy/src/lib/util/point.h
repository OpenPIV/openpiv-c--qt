
#pragma once

// std
#include <cstdint>
#include <type_traits>

/// basic 3D point
template < typename T >
class Point
{
public:
    Point() = default;
    Point( const Point& ) = default;
    Point( Point&& ) = default;

    template < typename U,
               typename = std::enable_if<std::is_convertible<U, T>::value>::type >
    Point( const Point< U >& p )
        : x_( p.x_ ),
          y_( p.y_ ),
          z_( p.z_ )
    {}
    
    const T& x() const { return x_; }
    const T& y() const { return y_; }
    const T& z() const { return z_; }
    
private:
    T x_ = {};
    T y_ = {};
    T z_ = {};
};


/// specialization for integer
using IntPoint = Point< uint32_t >;

/// specialization for doubles
using DoublePoint = Point< double >;
