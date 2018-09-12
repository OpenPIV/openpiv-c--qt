
#pragma once

// std
#include <cinttypes>
#include <cmath>
#include <complex>
#include <cstring>
#include <iostream>
#include <limits>
#include <type_traits>


#pragma pack(push, 1)

// RGBA packed
template < typename T >
struct RGBA
{
    using Type = T;
    using value_type = T;

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

template < typename T >
std::ostream& operator<<(std::ostream& os, const RGBA<T>& rgba )
{
    os << "RGBA(" << rgba.r << ", " << rgba.g << ", " << rgba.b << ", " << rgba.a << ")";
    return os;
}

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

/// YUVA packed
template < typename T >
struct YUVA
{
    using Type = T;
    using value_type = T;

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

template < typename T >
std::ostream& operator<<(std::ostream& os, const YUVA<T>& v )
{
    os << "YUVA(" << v.y << ", " << v.u << ", " << v.v << ", " << v.a << ")";
    return os;
}

// complex pixel type
template < typename T >
class Complex
{
public:
    using Type = T;
    using value_type = T;

    Complex() = default;
    Complex( const Complex& ) = default;
    Complex( Complex&& ) = default;
    Complex( T r, T i ) : real(r), imag(i){}
    Complex( const std::complex<T>& c ) : real(c.real()), imag(c.imag()) {}
    template <typename U,
              typename E = typename std::enable_if<
                  std::is_arithmetic<U>::value &&
                  std::is_convertible<U, T>::value >::type >
    Complex( const U& v )
        : real(v)
    {}

    Complex& operator=( const Complex& c ) = default;
    Complex& operator=( Complex&& c ) = default;
    template < typename U,
               typename E = typename std::enable_if< std::is_convertible<U, T>::value >::type >
    Complex& operator=( const Complex<U>& c ) { real = c.real; imag = c.imag; return *this; }

    inline operator T() const { return std::hypot(real, imag); }
    inline operator std::complex<T>() const { return std::complex<T>(real, imag); }

    inline Complex conj() const { return { real, -imag }; }

    T real{};
    T imag{};
};

using C8  = Complex<uint8_t>;
using C16 = Complex<uint16_t>;
using C32 = Complex<uint32_t>;
using CF  = Complex<double>;

template < typename T >
std::ostream& operator<<(std::ostream& os, const Complex<T>& v )
{
    char sign{ v.imag<0 ? '-' : '+' };
    os << v.real << sign << v.imag << "j";
    return os;
}

template < typename To, typename From >
inline To complextogrey( const Complex<From>& c )
{
    return std::hypot(c.real, c.imag);
}


// greyscale
template < typename T >
struct G
{
    using Type = T;
    using value_type = T;
    constexpr static T max() { return std::numeric_limits<T>::max(); }
    constexpr static T min() { return std::numeric_limits<T>::min(); }

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

    template < typename U >
    G( const Complex<U>& c )
        : v( complextogrey< T >( c ) )
    {}

    G& operator=(const G&) = default;
    G& operator=(G&&) = default;
    inline G& operator=(T v_) { v = v_; return *this; }
    template < typename U,
               typename E = typename std::enable_if< std::is_convertible<U, T>::value >::type >
    inline G& operator=(U v_) { v = v_; return *this; }

    inline operator T() const { return v; }
    inline operator RGBA<T>() const { return RGBA<T>(v); }
    inline operator Complex<T>() const { return Complex<T>(v, 0); }

    T v{};
};

// greyscale
using G8  = G<uint8_t>;
using G16 = G<uint16_t>;
using G32 = G<uint32_t>;
using GF  = G<double>;

inline G8  operator ""_g8 ( unsigned long long v ) { return G8( v ); }
inline G16 operator ""_g16( unsigned long long v ) { return G16( v ); }
inline G32 operator ""_g32( unsigned long long v ) { return G32( v ); }
inline GF  operator ""_gf ( long double v )        { return GF( v ); }

template < typename T >
std::ostream& operator<<(std::ostream& os, const G<T>& v )
{
    os << "G" << v.v;
    return os;
}

template <typename T>
struct is_pixeltype : std::false_type {};

template <typename T>
struct is_pixeltype<G<T>> : std::true_type {};

template <typename T>
struct is_pixeltype<RGBA<T>> : std::true_type {};

template <typename T>
struct is_pixeltype<YUVA<T>> : std::true_type {};

template <typename T>
struct is_pixeltype<Complex<T>> : std::true_type {};


#pragma pack(pop)
