
#pragma once

// std
#include <cinttypes>
#include <cstring>
#include <limits>
#include <type_traits>


#pragma pack(push, 1)

// RGBA packed
template < typename T >
struct RGBA
{
    RGBA() = default;
    RGBA(const RGBA&) = default;
    RGBA(RGBA&&) = default;
    RGBA( T v ) : r(v), g(v), b(v), a(std::numeric_limits<T>::max()) {}
    RGBA( T r_, T g_, T b_, T a_ ) : r(r_), g(g_), b(b_), a(a_) {}

    RGBA& operator=(const RGBA&) = default;
    RGBA& operator=(RGBA&&) = default;
    inline bool operator==( const RGBA& rhs ) const
    {
        return std::memcmp(this, &rhs, sizeof(RGBA)) == 0;
    }
    inline bool operator!=( const RGBA& rhs ) const
    {
        return !operator==(rhs);
    }

    T r{};
    T g{};
    T b{};
    T a{};
};

using RGBA8  = RGBA<uint8_t>;
using RGBA16 = RGBA<uint16_t>;
using RGBA32 = RGBA<uint32_t>;

// utility to convert to greyscale from rgb
template < typename To, typename From >
inline
typename std::enable_if< std::is_same< From, To >::value && std::is_integral<From>::value && sizeof(From)<64, To >::type
rgbtogrey( const RGBA<From>& rgb )
{
    return (rgb.r*218ULL + rgb.g*732ULL + rgb.b*74ULL) >> 10;
}

template < typename To, typename From >
inline
typename std::enable_if< std::is_floating_point<To>::value, To >::type
rgbtogrey( const RGBA<From>& rgb )
{
    return 0.2126*rgb.r + 0.7152*rgb.g + 0.0722*rgb.b;
}


template < typename T >
struct YUVA
{
    YUVA() = default;
    YUVA(const YUVA&) = default;
    YUVA(YUVA&&) = default;
    YUVA( T v ) : y(v), u(0), v(0), a(std::numeric_limits<T>::max()) {}
    YUVA( T y_, T u_, T v_, T a_ ) : y(y_), u(u_), v(v_), a(a_) {}

    YUVA& operator=(const YUVA&) = default;
    YUVA& operator=(YUVA&&) = default;
    inline bool operator==( const YUVA& rhs ) const
    {
        return std::memcmp(this, &rhs, sizeof(YUVA)) == 0;
    }
    inline bool operator!=( const YUVA& rhs ) const
    {
        return !operator==(rhs);
    }

    T y{};
    T u{};
    T v{};
    T a{};
};

using YUVA8  = YUVA<uint8_t>;
using YUVA16 = YUVA<uint16_t>;
using YUVA32 = YUVA<uint32_t>;

// greyscale
template < typename T >
struct G
{
    G() = default;
    G(const G&) = default;
    G(G&&) = default;
    G( T v_ ) : v(v_) {}
    template < typename U,
               typename E = typename std::enable_if< std::is_convertible<U, T>::value >::type >
    G( U v_) : v(v_) {}

    template < typename U >
    G( const RGBA<U>& rgb )
        : v( rgbtogrey< T >( rgb ) )
    {}

    G& operator=(const G&) = default;
    G& operator=(G&&) = default;
    inline G& operator=(T v_) { v = v_; return *this; }
    template < typename U,
               typename E = typename std::enable_if< std::is_convertible<U, T>::value >::type >
    inline G& operator=(U v_) { v = v_; return *this; }

    inline operator T() const { return v; }
    inline operator RGBA<T>() const { return RGBA<T>(v); }

    T v{};
};

// greyscale
using G8  = G<uint8_t>;
using G16 = G<uint16_t>;
using G32 = G<uint32_t>;
using GF  = G<double>;

#pragma pack(pop)
