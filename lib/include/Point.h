
#pragma once

// std
#include <array>
#include <cstdint>
#include <iostream>

// local
#include "Util.h"

/// basic n-dimensional point
template < size_t N, typename T >
class Point
{
public:
    using type = T;
    static const size_t dimension = N;
    using DataType = typename std::array<T, N>;

    // ctor
    Point() = default;
    Point( const Point& ) = default;
    Point( Point&& ) = default;

    /// allow initialization of the point components
    template < typename... Ts,
               typename = typename std::enable_if<N == sizeof...(Ts), void>::type,
               typename = typename std::enable_if<are_all_convertible<T, Ts...>::value>::type >
    constexpr Point( Ts&&... v )
        : data_( DataType{ {static_cast<T>(v)...} } )
    {}

    /// conversion from another similar point
    template < typename U,
               typename = typename std::enable_if<std::is_convertible<U, T>::value>::type >
    constexpr Point( const Point< N, U >& p )
        : data_( convert_array_to< T >(p.data()) )
    {}

    // assignment
    Point& operator=(const Point& rhs) = default;
    Point& operator=(Point&& rhs) = default;

    // equality
    constexpr inline bool operator==(const Point& rhs) const { return data_ == rhs.data_; }
    constexpr inline bool operator!=(const Point& rhs) const { return !operator==(rhs); }

    constexpr inline const T& operator[](size_t i) const { return data_[i]; }
    constexpr inline T& operator[](size_t i) { return data_[i]; }

    // return underlying data
    constexpr inline const DataType& data() const { return data_; }

private:
    DataType data_ {};
};

/// ostream operator
template < typename T, size_t N >
std::ostream& operator<<( std::ostream& os, const Point<N, T>& p )
{
    os << "(";
    for( size_t i=0; i<N; ++i)
        os << (i==0?"":",") << p[i];
    os << ")";

    return os;
}


/// specialization for common dimensions
template < typename T >
using Point2 = Point< 2, T >;
template < typename T >
using Point3 = Point< 3, T >;
