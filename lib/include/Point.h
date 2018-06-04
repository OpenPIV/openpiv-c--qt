
#pragma once

// std
#include <array>
#include <cstdint>
#include <iostream>

// local
#include "Util.h"

/// basic n-dimensional point
template < typename T, size_t N >
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
        : data_( DataType{ {v...} } )
    {}

    /// conversion from another similar point
    template < typename U,
               typename = typename std::enable_if<std::is_convertible<U, T>::value>::type >
    constexpr Point( const Point< U, N >& p )
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
std::ostream& operator<<( std::ostream& os, const Point<T, N>& p )
{
    os << "(";
    for( size_t i=0; i<N; ++i)
        os << (i==0?"":",") << p[i];
    os << ")";

    return os;
}


/// specialization for integer
using Int2DPoint = Point< int32_t, 2 >;
using Int3DPoint = Point< int32_t, 3 >;
using UInt2DPoint = Point< uint32_t, 2 >;
using UInt3DPoint = Point< uint32_t, 3 >;

/// specialization for doubles
using Double2DPoint = Point< double, 2 >;
using Double3DPoint = Point< double, 3 >;
