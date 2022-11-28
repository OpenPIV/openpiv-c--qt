
#pragma once

// local
#include "core/util.h"

// std
#include <cinttypes>
#include <cmath>
#include <complex>
#include <cstring>
#include <iostream>
#include <limits>
#include <string_view>
#include <type_traits>

namespace openpiv::core {

#pragma pack(push, 1)

// rgba packed
template < typename T >
struct rgba
{
    using value_t = T;

    rgba() = default;
    rgba(const rgba&) = default;
    rgba(rgba&&) = default;
    rgba( T v ) : r(v), g(v), b(v), a(std::numeric_limits<T>::max()) {}
    rgba( T r_, T g_, T b_, T a_ ) : r(r_), g(g_), b(b_), a(a_) {}

    rgba& operator=(const rgba&) = default;
    rgba& operator=(rgba&&) = default;
    rgba& operator=( T v )
    {
        rgba tmp{ v };
        std::swap( *this, tmp );
        return *this;
    }

    inline bool operator==( const rgba& rhs ) const
    {
        return std::memcmp(this, &rhs, sizeof(rgba)) == 0;
    }
    inline bool operator!=( const rgba& rhs ) const
    {
        return !operator==(rhs);
    }

    T r{};
    T g{};
    T b{};
    T a{};
};

using rgba_8  = rgba<uint8_t>;
using rgba_16 = rgba<uint16_t>;
using rgba_32 = rgba<uint32_t>;

template < typename T >
std::ostream& operator<<(std::ostream& os, const rgba<T>& rgba )
{
    os << "rgba(" << +rgba.r << ", " << +rgba.g << ", " << +rgba.b << ", " << +rgba.a << ")";
    return os;
}

/// yuva packed
template < typename T >
struct yuva
{
    using value_t = T;

    yuva() = default;
    yuva(const yuva&) = default;
    yuva(yuva&&) = default;
    yuva( T v ) : y(v), u(0), v(0), a(std::numeric_limits<T>::max()) {}
    yuva( T y_, T u_, T v_, T a_ ) : y(y_), u(u_), v(v_), a(a_) {}

    yuva& operator=(const yuva&) = default;
    yuva& operator=(yuva&&) = default;
    yuva& operator=( T v )
    {
        yuva tmp{ v };
        std::swap( *this, tmp );
        return *this;
    }

    inline bool operator==( const yuva& rhs ) const
    {
        return std::memcmp(this, &rhs, sizeof(yuva)) == 0;
    }
    inline bool operator!=( const yuva& rhs ) const
    {
        return !operator==(rhs);
    }

    T y{};
    T u{};
    T v{};
    T a{};
};

using yuva_8  = yuva<uint8_t>;
using yuva_16 = yuva<uint16_t>;
using yuva_32 = yuva<uint32_t>;

template < typename T >
std::ostream& operator<<(std::ostream& os, const yuva<T>& v )
{
    os << "yuva(" << +v.y << ", " << +v.u << ", " << +v.v << ", " << +v.a << ")";
    return os;
}

// complex pixel type
template < typename T >
class complex
{
public:
    using value_t = T;

    complex() = default;
    complex( const complex& ) = default;
    complex( complex&& ) = default;

    constexpr complex( value_t r ) : real(r) {}
    constexpr complex( value_t r, value_t i ) : real(r), imag(i){}
    template < typename U,
               typename E = typename std::enable_if_t< std::is_convertible_v<U, T> > >
    constexpr complex( U r ) : real(r) {}
    template < typename U,
               typename E = typename std::enable_if_t< std::is_convertible_v<U, T> > >
    constexpr complex( U r, U i ) : real(r), imag(i) {}
    constexpr complex( const std::complex<T>& c ) : real(c.real()), imag(c.imag()) {}

    complex& operator=( const complex& c ) = default;
    complex& operator=( complex&& c ) = default;
    template < typename U,
               typename E = typename std::enable_if_t< std::is_convertible_v<U, T> > >
    complex& operator=( U r ) { real = r; imag = T{}; return *this; }
    template < typename U,
               typename E = typename std::enable_if_t< std::is_convertible_v<U, T> > >
    complex& operator=( const complex<U>
                        & c ) { real = c.real; imag = c.imag; return *this; }

    bool operator==( const complex& rhs ) const
    {
        return real == rhs.real && imag == rhs.imag;
    }
    bool operator!=( const complex& lhs ) const
    {
        return !operator==(lhs);
    }
    // mathematical comparison operators; complex cannot be
    // compared so use abs value for sorting
    bool operator>( const complex& rhs ) const { return abs_sqr() > rhs.abs_sqr(); }
    bool operator<( const complex& rhs ) const { return abs_sqr() < rhs.abs_sqr(); }
    bool operator>=( const complex& rhs ) const { return abs_sqr() >= rhs.abs_sqr(); }
    bool operator<=( const complex& rhs ) const { return abs_sqr() <= rhs.abs_sqr(); }

    inline operator std::complex<T>() const { return std::complex<T>(real, imag); }

    inline constexpr complex operator-( const complex& rhs ) const
    {
        return { real - rhs.real, imag - rhs.imag };
    }

    inline constexpr complex operator+( const complex& rhs ) const
    {
        return { real + rhs.real, imag + rhs.imag };
    }

    inline constexpr complex operator*( const complex& rhs ) const
    {
        const T &a{ real }, &b{ imag }, &c{ rhs.real }, &d{ rhs.imag };
        return { a*c - b*d, b*c + a*d };
    }

    inline constexpr complex operator/( const complex& rhs ) const
    {
        const T &a{ real }, &b{ imag }, &c{ rhs.real }, &d{ rhs.imag };
        const T denom{ static_cast<T>(c*c + d*d) };
        return { (a*c + b*d)/denom, (b*c - a*d)/denom };
    }

    inline constexpr complex& operator-=( const complex& rhs )
    {
        *this = operator-(rhs);
        return *this;
    }

    inline constexpr complex& operator+=( const complex& rhs )
    {
        *this = operator+(rhs);
        return *this;
    }

    inline constexpr complex& operator*=( const complex& rhs )
    {
        *this = operator*(rhs);
        return *this;
    }

    inline constexpr complex& operator/=( const complex& rhs )
    {
        *this = operator/(rhs);
        return *this;
    }

    inline constexpr complex conj() const { return { real, static_cast<T>(-imag) }; }
    inline constexpr T abs() const { return std::sqrt( abs_sqr() ); }
    inline constexpr T abs_sqr() const { return real*real + imag*imag; }

    T real{};
    T imag{};
};

template <typename T>
inline constexpr complex<T> operator-( const T& lhs, const complex<T>& rhs )
{
    return complex<T>{ lhs } - rhs;
}

template <typename T>
inline constexpr complex<T> operator+( const T& lhs, const complex<T>& rhs )
{
    return complex<T>{ lhs } + rhs;
}

template <typename T>
inline constexpr complex<T> operator*( const T& lhs, const complex<T>& rhs )
{
    return complex<T>{ lhs } * rhs;
}

template <typename T>
inline constexpr complex<T> operator/( const T& lhs, const complex<T>& rhs )
{
    return complex<T>{ lhs } / rhs;
}

template <typename T>
inline constexpr complex<T> exp( const complex<T>& c )
{

    const T e{ static_cast<T>(std::exp( c.real )) };
    return complex<T>{ e * std::cos( c.imag ), e * std::sin( c.imag ) };
}

using c_8  = complex<int8_t>;
using c_16 = complex<int16_t>;
using c_32 = complex<int32_t>;
using c_f  = complex<double>;

template < typename T >
std::ostream& operator<<(std::ostream& os, const complex<T>& v )
{
    static const char* sign[] = { " ", " +" };
    os << "complex(" << +v.real << sign[ v.imag< 0 ? 0 : 1 ] << +v.imag << "j)";
    return os;
}


#pragma warning(disable: 4244) // conversion from U to T, possible loss of data
// greyscale
template < typename T >
struct g
{
    using value_t = T;
    constexpr static T max() { return std::numeric_limits<T>::max(); }
    constexpr static T min() { return std::numeric_limits<T>::min(); }

    g() = default;
    g(const g&) = default;
    g(g&&) = default;
    template < typename U,
               typename = typename std::enable_if_t< std::is_convertible_v< U, T > > >
    g( U v_ ) : v(v_) {}

    g& operator=(const g&) = default;
    g& operator=(g&&) = default;
    template < typename U,
               typename = typename std::enable_if_t< std::is_convertible_v< U, T > > >
    inline g& operator=(U v_) { v = v_; return *this; }

    inline operator T() const { return v; }

    T v{};
};
#pragma warning(default: 4244)

// greyscale
using g_8  = g<uint8_t>;
using g_16 = g<uint16_t>;
using g_32 = g<uint32_t>;
using g_f  = g<double>;

inline g_8  operator ""_g8 ( unsigned long long v ) { return g_8( v ); }
inline g_16 operator ""_g16( unsigned long long v ) { return g_16( v ); }
inline g_32 operator ""_g32( unsigned long long v ) { return g_32( v ); }
inline g_f  operator ""_gf ( long double v )        { return g_f( v ); }

template < typename T >
std::ostream& operator<<(std::ostream& os, const g<T>& v )
{
    os << "g(" << +v.v << ")";
    return os;
}

//
//
//
template <typename T>
struct is_pixeltype : std::false_type {};

template <typename T>
struct is_pixeltype<g<T>> : std::true_type {};

template <typename T>
struct is_pixeltype<rgba<T>> : std::true_type {};

template <typename T>
struct is_pixeltype<yuva<T>> : std::true_type {};

template <typename T>
struct is_pixeltype<complex<T>> : std::true_type {};

template <typename T>
inline constexpr bool is_pixeltype_v = is_pixeltype<T>::value;

//
//
//
template <typename T>
struct is_real_mono_pixeltype : std::false_type {};

template <typename T>
struct is_real_mono_pixeltype<g<T>> : std::true_type {};

template <typename T>
inline constexpr bool is_real_mono_pixeltype_v = is_real_mono_pixeltype<T>::value;

//
//
//
template <typename T>
struct is_complex_mono_pixeltype : std::false_type {};

template <typename T>
struct is_complex_mono_pixeltype<complex<T>> : std::true_type {};

template <typename T>
inline constexpr bool is_complex_mono_pixeltype_v = is_complex_mono_pixeltype<T>::value;


//
//
// conversion functions between pixel types
template<typename From, typename To,
         typename = typename std::enable_if_t<
             is_pixeltype_v<From> && is_pixeltype_v<To>
             > >
class pixeltype_is_convertible_helper
{
    template<typename From1, typename To1,
             typename = decltype( convert( std::declval<From1>(), std::declval<To1&>() ) ) >
    static std::true_type test(int);

    template<typename, typename>
    static std::false_type test(...);

public:
    using type = decltype( test<From, To>(0) );
    static inline constexpr bool value = type::value;
};


template<typename From, typename To>
struct pixeltype_is_convertible
    : public pixeltype_is_convertible_helper<From, To>::type
{};

template<typename From, typename To>
inline constexpr bool pixeltype_is_convertible_v = pixeltype_is_convertible<From, To>::value;

// identity case; should be optimized away
template < template <typename> class PT, typename T >
inline constexpr
typename std::enable_if_t< is_pixeltype_v< PT<T> > >
convert( const PT<T>& from, PT<T>& to )
{
    to = from;
}

// same pixel type but different contained types
template < template <typename> class PT, typename From, typename To >
inline constexpr
typename std::enable_if_t< std::is_convertible_v<From, To> >
convert( const PT<From>& from, PT<To>& to )
{
    to = from;
}

/// convert from rgb to greyscale
template < typename From, typename To >
inline constexpr
typename
std::enable_if_t<
    std::is_convertible_v<From, To> &&
    std::is_integral_v<From> &&
    sizeof(From)<64 >
convert( const rgba<From>& rgb, g<To>& g_ )
{
    g_ = (rgb.r*218ULL + rgb.g*732ULL + rgb.b*74ULL) >> 10;
}

template < typename From, typename To >
inline constexpr
typename
std::enable_if_t<
    std::is_convertible_v<From, To> &&
    std::is_floating_point_v<From> >
convert( const rgba<From>& rgb, g<To>& g_ )
{
    g_ = 0.2126*rgb.r + 0.7152*rgb.g + 0.0722*rgb.b;
}

template < typename From, typename To >
inline constexpr
typename
std::enable_if_t< std::is_convertible_v<From, To> >
convert( const g<From>& g, rgba<To>& rgba_ )
{
    rgba_ = rgba<To>{ g };
}

/// convert from complex to greyscale
template < typename From, typename To >
inline constexpr
typename
std::enable_if_t< std::is_convertible_v<From, To> >
convert( const complex<From>& c, g<To>& g_ )
{
    g_ = c.abs();
}

template < typename From, typename To >
inline constexpr
typename
std::enable_if_t< std::is_convertible_v<From, To> >
convert( const g<From>& g_, complex<To>& c )
{
    c = complex<To>{ g_ };
}

//
template <typename T>
constexpr std::string_view pixeltype_name()
{
    if constexpr (std::is_same_v<T, g_8>)
        return "g<uint8_t>";
    if constexpr (std::is_same_v<T, g_16>)
        return "g<uint16_t>";
    if constexpr (std::is_same_v<T, g_32>)
        return "g<uint32_t>";
    if constexpr (std::is_same_v<T, g_f>)
        return "g<double>";

    if constexpr (std::is_same_v<T, c_8>)
        return "complex<uint8_t>";
    if constexpr (std::is_same_v<T, c_16>)
        return "complex<uint16_t>";
    if constexpr (std::is_same_v<T, c_32>)
        return "complex<uint32_t>";
    if constexpr (std::is_same_v<T, c_f>)
        return "complex<double>";

    if constexpr (std::is_same_v<T, rgba_8>)
        return "rgba<uint8_t>";
    if constexpr (std::is_same_v<T, rgba_16>)
        return "rgba<uint16_t>";
    if constexpr (std::is_same_v<T, rgba_32>)
        return "rgba<uint32_t>";

    if constexpr (std::is_same_v<T, yuva_8>)
        return "yuva<uint8_t>";
    if constexpr (std::is_same_v<T, yuva_16>)
        return "yuva<uint16_t>";
    if constexpr (std::is_same_v<T, yuva_32>)
        return "yuva<uint32_t>";

    return "unknown pixeltype";
}



#pragma pack(pop)

}
