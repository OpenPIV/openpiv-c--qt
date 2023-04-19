
#pragma once

// std
#include <array>
#include <cmath>
#include <cstdint>
#include <iostream>

// local
#include "core/util.h"
#include "core/exception_builder.h"

namespace openpiv::core {

/// basic n-dimensional point
template < size_t N, typename T >
class point
{
public:
    using value_t = T;
    static const size_t dimension = N;
    using data_t = typename std::array<T, N>;

    // ctor
    point() = default;
    point( const point& ) = default;
    point( point&& ) = default;

    /// allow initialization of the point components
    template < typename... Ts,
               typename = typename std::enable_if_t<N == sizeof...(Ts), void>,
               typename = typename std::enable_if_t<are_all_convertible_v<T, Ts...>> >
    constexpr point( Ts&&... v )
        : data_( data_t{ {static_cast<T>(v)...} } )
    {}

    /// conversion from another similar point
    template < typename U,
               typename = typename std::enable_if<std::is_convertible<U, T>::value>::type >
    constexpr point( const point< N, U >& p )
        : data_( convert_array_to< T >(p.data()) )
    {}

    /// conversion from a tuple
    constexpr point( data_t&& d )
        : data_( std::move(d) )
    {}

    // assignment
    point& operator=(const point& rhs) = default;
    point& operator=(point&& rhs) = default;

    // equality
    constexpr inline bool operator==(const point& rhs) const { return data_ == rhs.data_; }
    constexpr inline bool operator!=(const point& rhs) const { return !operator==(rhs); }

    constexpr inline bool operator<(const point& rhs) const
    {
        auto p = [](const auto& _p) constexpr { return _p[0]*_p[0] + _p[1]*_p[1]; };
        auto a = [](const auto& _p) constexpr { return atan2(_p[1], _p[0]); };
        if ( p(*this) == p(rhs) )
            return a(*this) < a(rhs);

        return p(*this) < p(rhs);
    }

    constexpr inline bool operator<=(const point& rhs) const
    {
        return operator==(rhs) || operator<(rhs);
    }

    constexpr inline bool operator>(const point& rhs) const
    {
        return !operator<=(rhs);
    }

    constexpr inline bool operator>=(const point& rhs) const
    {
        return operator==(rhs) || !operator<(rhs);
    }

    constexpr inline const T& operator[](size_t i) const
    {
        if (i>=N)
            exception_builder<std::out_of_range>() << "index out of range (" << N << "): " << i;

        return data_[i];
    }
    constexpr inline T& operator[](size_t i)
    {
        if (i>=N)
            exception_builder<std::out_of_range>() << "index out of range (" << N << "): " << i;

        return data_[i];
    }

    // return underlying data
    constexpr inline const data_t& data() const { return data_; }

private:
    data_t data_ {};
};

/// ostream operator
template < typename T, size_t N >
std::ostream& operator<<( std::ostream& os, const point<N, T>& p )
{
    os << "(";
    for( size_t i=0; i<N; ++i)
        os << (i==0?"":",") << p[i];
    os << ")";

    return os;
}


/// specialization for common dimensions
template < typename T >
using point2 = point< 2, T >;
template < typename T >
using point3 = point< 3, T >;

}
