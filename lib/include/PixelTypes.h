
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
    RGBA& operator=( T v )
    {
        std::swap( *this, RGBA(v) );
        return *this;
    }

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
    YUVA& operator=( T v )
    {
        std::swap( *this, YUVA(v) );
        return *this;
    }

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

    template < typename U,
               typename E = typename std::enable_if< std::is_convertible<U, T>::value >::type >
    constexpr Complex( U r, U i = U{} ) : real(r), imag(i){}
    constexpr Complex( const std::complex<T>& c ) : real(c.real()), imag(c.imag()) {}

    Complex& operator=( const Complex& c ) = default;
    Complex& operator=( Complex&& c ) = default;
    template < typename U,
               typename E = typename std::enable_if< std::is_convertible<U, T>::value >::type >
    Complex& operator=( const Complex<U>& c ) { real = c.real; imag = c.imag; return *this; }

    bool operator==( const Complex& rhs ) const
    {
        return real == rhs.real && imag == rhs.imag;
    }
    bool operator!=( const Complex& lhs ) const
    {
        return !operator==(lhs);
    }
    // mathematical comparison operators; complex cannot be
    // compared so use abs value for sorting
    bool operator>( const Complex& rhs ) const { return abs_sqr() > rhs.abs_sqr(); }
    bool operator<( const Complex& rhs ) const { return abs_sqr() < rhs.abs_sqr(); }
    bool operator>=( const Complex& rhs ) const { return abs_sqr() >= rhs.abs_sqr(); }
    bool operator<=( const Complex& rhs ) const { return abs_sqr() <= rhs.abs_sqr(); }

    inline operator std::complex<T>() const { return std::complex<T>(real, imag); }

    inline constexpr Complex operator-( const Complex& rhs ) const
    {
        return { real - rhs.real, imag - rhs.imag };
    }

    inline constexpr Complex operator+( const Complex& rhs ) const
    {
        return { real + rhs.real, imag + rhs.imag };
    }

    inline constexpr Complex operator*( const Complex& rhs ) const
    {
        const T a{ real }, b{ imag }, c{ rhs.real }, d{ rhs.imag };
        return { a*c - b*d, b*c + a*d };
    }

    inline constexpr Complex operator/( const Complex& rhs ) const
    {
        const T a{ real }, b{ imag }, c{ rhs.real }, d{ rhs.imag };
        const T denom{ c*c + d*d };
        return { (a*c + b*d)/denom, (b*c - a*d)/denom };
    }

    inline constexpr Complex& operator-=( const Complex& rhs )
    {
        *this = operator-(rhs);
        return *this;
    }

    inline constexpr Complex& operator+=( const Complex& rhs )
    {
        *this = operator+(rhs);
        return *this;
    }

    inline constexpr Complex& operator*=( const Complex& rhs )
    {
        *this = operator*(rhs);
        return *this;
    }

    inline constexpr Complex& operator/=( const Complex& rhs )
    {
        *this = operator/(rhs);
        return *this;
    }

    inline constexpr Complex conj() const { return { real, -imag }; }
    inline constexpr T abs() const { return std::sqrt( abs_sqr() ); }
    inline constexpr T abs_sqr() const { return (*this * conj()).real; }

    T real{};
    T imag{};
};

template <typename T>
inline constexpr Complex<T> operator-( const T& lhs, const Complex<T>& rhs )
{
    return Complex<T>{ lhs } - rhs;
}

template <typename T>
inline constexpr Complex<T> operator+( const T& lhs, const Complex<T>& rhs )
{
    return Complex<T>{ lhs } + rhs;
}

template <typename T>
inline constexpr Complex<T> operator*( const T& lhs, const Complex<T>& rhs )
{
    return Complex<T>{ lhs } * rhs;
}

template <typename T>
inline constexpr Complex<T> operator/( const T& lhs, const Complex<T>& rhs )
{
    return Complex<T>{ lhs } / rhs;
}

template <typename T>
inline constexpr Complex<T> exp( const Complex<T>& c )
{
    const T e{ exp( c.real ) };
    return Complex<T>{ e * std::cos( c.imag ), e * std::sin( c.imag ) };
}

using C8  = Complex<uint8_t>;
using C16 = Complex<uint16_t>;
using C32 = Complex<uint32_t>;
using CF  = Complex<double>;

template < typename T >
std::ostream& operator<<(std::ostream& os, const Complex<T>& v )
{
    static const char* sign[] = { " ", " +" };
    os << v.real << sign[ v.imag< 0 ? 0 : 1 ] << v.imag << "j";
    return os;
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
    template < typename U,
               typename = typename std::enable_if< std::is_convertible< U, T >::value >::type >
    G( U v_ ) : v(v_) {}

    G& operator=(const G&) = default;
    G& operator=(G&&) = default;
    template < typename U,
               typename = typename std::enable_if< std::is_convertible< U, T >::value >::type >
    inline G& operator=(U v_) { v = v_; return *this; }

    inline operator T() const { return v; }

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


// conversion functions between pixel types

template<typename From, typename To,
         typename = typename std::enable_if<
             is_pixeltype<From>::value && is_pixeltype<To>::value
             >::type >
class pixeltype_is_convertible_helper
{
    template<typename From1, typename To1,
             typename = decltype( convert( std::declval<From1>(), std::declval<To1&>() ) ) >
    static std::true_type test(int);

    template<typename, typename>
    static std::false_type test(...);

public:
    typedef decltype( test<From, To>(0) ) type;
};


template<typename From, typename To>
struct pixeltype_is_convertible
    : public pixeltype_is_convertible_helper<From, To>::type
{};

// identity case; should be optimized away
template < template <typename> class PT, typename T >
inline constexpr
typename std::enable_if<is_pixeltype<PT<T>>::value>::type
convert( const PT<T>& from, PT<T>& to )
{
    to = from;
}

// same pixel type but different contained types
template < template <typename> class PT, typename From, typename To >
inline constexpr
typename std::enable_if<std::is_convertible<From, To>::value>::type
convert( const PT<From>& from, PT<To>& to )
{
    to = from;
}

/// convert from rgb to greyscale
template < typename From, typename To >
inline constexpr
typename
std::enable_if<
    std::is_convertible<From, To>::value &&
    std::is_integral<From>::value &&
    sizeof(From)<64 >::type
convert( const RGBA<From>& rgb, G<To>& g )
{
    g = (rgb.r*218ULL + rgb.g*732ULL + rgb.b*74ULL) >> 10;
}

template < typename From, typename To >
inline constexpr
typename
std::enable_if<
    std::is_convertible<From, To>::value &&
    std::is_floating_point<From>::value >::type
convert( const RGBA<From>& rgb, G<To>& g )
{
    g = 0.2126*rgb.r + 0.7152*rgb.g + 0.0722*rgb.b;
}

template < typename From, typename To >
inline constexpr
typename
std::enable_if<
    std::is_convertible<From, To>::value >::type
convert( const G<From>& g, RGBA<To>& rgba )
{
    rgba = RGBA<To>{ g };
}

/// convert from complex to greyscale
template < typename From, typename To >
inline constexpr
typename
std::enable_if<
    std::is_convertible<From, To>::value >::type
convert( const Complex<From>& c, G<To>& g )
{
    g = c.abs();
}

template < typename From, typename To >
inline constexpr
typename
std::enable_if<
    std::is_convertible<From, To>::value >::type
convert( const G<From>& g, Complex<To>& c )
{
    c = Complex<To>{ g };
}



#pragma pack(pop)
