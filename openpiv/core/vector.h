
#pragma once

// std
#include <array>
#include <cstdint>
#include <functional>
#include <iostream>
#include <utility>

// local
#include "core/point.h"
#include "core/util.h"

namespace openpiv::core {

/// basic n-dimensional vector
template < size_t N, typename T >
class vector
{
public:
    using value_t = T;
    static const size_t dimension = N;
    using data_t = typename std::array<T, N>;

    // ctor
    vector() = default;
    vector( const vector& ) = default;
    vector( vector&& ) = default;

    /// allow initialization of the vector components
    template < typename... Ts,
               typename = typename std::enable_if_t<N == sizeof...(Ts)>,
               typename = typename std::enable_if_t<are_all_convertible_v<T, Ts...>> >
    constexpr vector( Ts&&... v )
        : data_( data_t{ {static_cast<T>(v)...} } )
    {}

    /// conversion from another similar vector
    template < typename U,
               typename = typename std::enable_if_t<std::is_convertible_v<U, T>> >
    constexpr vector( const vector< N, U >& v )
        : data_( convert_array_to< T >(v.data()) )
    {}

    /// conversion from a tuple
    constexpr vector( data_t&& d )
        : data_( std::move(d) )
    {}

    // assignment
    vector& operator=(const vector& rhs) = default;
    vector& operator=(vector&& rhs) = default;

    // equality
    constexpr inline bool operator==(const vector& rhs) const { return data_ == rhs.data_; }
    constexpr inline bool operator!=(const vector& rhs) const { return !operator==(rhs); }

    constexpr inline const T& operator[](size_t i) const { return data_[i]; }
    constexpr inline T& operator[](size_t i) { return data_[i]; }

    // return underlying data
    constexpr inline const data_t& data() const { return data_; }

private:
    data_t data_ {};
};

/// ostream operator
template < size_t N, typename T >
std::ostream& operator<<( std::ostream& os, const vector<N, T>& p )
{
    os << "[";
    for( size_t i=0; i<N; ++i)
        os << (i==0?"":",") << p[i];
    os << "]";

    return os;
}

template <typename Op,
          typename T,
          std::size_t... Is>
constexpr T apply( const T& lhs, const T& rhs, std::index_sequence<Is...> )
{
    Op op;
    return T{ op(lhs[Is], rhs[Is])... };
}

template < size_t N, typename T >
vector<N,T> operator-(const point<N,T>& p1, const point<N,T>& p2)
{
    return apply<std::minus<T>>( p1.data(), p2.data(), std::make_index_sequence<N>() );
}

template < size_t N, typename T >
point<N,T> operator+(const point<N,T>& p, const vector<N,T>& v)
{
    return apply<std::plus<T>>( p.data(), v.data(), std::make_index_sequence<N>() );
}

template < size_t N, typename T >
point<N,T> operator-(const point<N,T>& p, const vector<N,T>& v)
{
    return apply<std::minus<T>>( p.data(), v.data(), std::make_index_sequence<N>() );
}

template < size_t N, typename T >
vector<N,T> operator-(const vector<N,T>& v1, const vector<N,T>& v2)
{
    return apply<std::minus<T>>( v1.data(), v2.data(), std::make_index_sequence<N>() );
}

template < size_t N, typename T >
vector<N,T> operator+(const vector<N,T>& v1, const vector<N,T>& v2)
{
    return apply<std::plus<T>>( v1.data(), v2.data(), std::make_index_sequence<N>() );
}


/// specialization for common dimensions
template < typename T >
using vector2 = vector< 2, T >;
template < typename T >
using vector3 = vector< 3, T >;

}
