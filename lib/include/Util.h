
#pragma once

// std
#include <cmath>
#include <limits>
#include <istream>
#include <numeric>
#include <experimental/optional>
#include <sstream>
#include <typeinfo>
#include <type_traits>
#include <utility>
#include <vector>


// thin wrapper around underlying contiguous data;
// provides an encapsulation for passing around
// blocks of data
template < typename PointerT,
           typename E = typename std::enable_if< std::is_pointer<PointerT>::value >::type >
class DataView
{
public:
    DataView( PointerT d, size_t length ) : d_( d ), length_( length ) {}
    DataView( DataView&& ) = default;
    DataView( const DataView& ) = default;
    DataView& operator=( DataView&& ) = default;
    DataView& operator=( const DataView& ) = default;

    PointerT data() const { return d_; }
    size_t length() const { return length_; }
    size_t size() const { return length_; }

private:
    PointerT d_;
    size_t length_;
};

/// determine if \a v is a power of two
inline constexpr bool is_pow2( uint64_t v )
{
    return v && (v == (v&-v));
}

/// wrapper to allow using stringstream to construct an
/// exception message; throw \ta E on destruction.
template < typename E >
class Thrower
{
public:
    Thrower() = default;
    ~Thrower() noexcept(false)
    {
        throw E(ss.str());
    }

    template < typename T >
    std::stringstream& operator<<( const T& v )
    {
        ss << v;
        return ss;
    }

    std::stringstream ss;
};

template <typename T>
auto make_range( const T& s );

/// simple Range class; inefficient but fine for loop initialization
template <typename T, typename E = typename std::enable_if<std::is_integral<T>::value>::type>
class Range
{
    Range( T s ) : s_( s ) {}
    std::experimental::optional< T > s_;

    friend auto make_range<T>( const T& s );

public:
    // returns a populated vector containing the closed range [s_, e]
    std::vector<T> to( T e ) const
    {
        T s = s_.value();
        std::vector<T> result( std::abs(e - s) + 1 );
        if ( s < e )
            std::iota( std::begin( result ), std::end( result ), s );
        else
            std::iota( std::rbegin( result ), std::rend( result ), e );

        return result;
    }

    // returns a populated vector containing the open range [s_, s_ + l)
    std::vector<T> length( T l ) const
    {
        T s = s_.value();
        std::vector<T> result( s + l - 1 );
        std::iota( std::begin( result ), std::end( result ), s );

        return result;
    }
};

template <typename T>
auto make_range( const T& s )
{
    return Range< T >(s);
}

template <typename T>
auto range_from( const T& s )
{
    return make_range(s);
}

template <typename T>
auto range_start_at( const T& s )
{
    return make_range(s);
}

/// simple RAII for std::istream multi-char peek
class Peeker
{
public:
    Peeker( std::istream& is )
        : is_( is )
    {
        pos_ = is.tellg();
        // this will only work if we can read and rewind
        if ( pos_ == -1 || !is.good() )
            Thrower<std::runtime_error>() << "input stream doesn't support input position indicator";
    }

    ~Peeker()
    {
        is_.seekg(pos_);
    }

private:
    std::istream& is_;
    std::istream::pos_type pos_ {-1};
};

/// allow safe conversion from unsigned to signed
template <typename To, typename From>
constexpr
typename std::enable_if<
    std::is_signed<To>::value && std::is_unsigned<From>::value && (sizeof(To) > sizeof(From)),
    To
    >::type
checked_unsigned_conversion(const From& v)
{
    return static_cast<To>(v);
}

/// allow safe conversion from unsigned to signed
template <typename To, typename From>
constexpr
typename std::enable_if<
    std::is_signed<To>::value && std::is_unsigned<From>::value && (sizeof(To) == sizeof(From)),
    To
    >::type
checked_unsigned_conversion(const From& v)
{
    if (v>std::numeric_limits<To>::max())
        Thrower<std::range_error>() << "unable to convert " << v << " to " << typeid(To).name() << " as value would be truncated";

    return static_cast<To>(v);
}

/// pair of helpers to determine if all types Ts are convertible to T
template <typename To, typename From, typename... R>
struct are_all_convertible {
    constexpr static bool value =
        std::is_convertible<From,To>::value &&
        are_all_convertible<To,R...>::value;
};

template <typename To, typename From>
struct are_all_convertible<To, From> {
    constexpr static bool value = std::is_convertible<From,To>::value;
};

/// pair of helpers to determine if all types Ts are equal to T
template <typename TypeA, typename TypeB, typename... R>
struct are_all_equal {
    constexpr static bool value =
        std::is_same<TypeA, TypeB>::value &&
        are_all_equal<TypeA, R...>::value;
};

template <typename TypeA, typename TypeB>
struct are_all_equal<TypeA, TypeB> {
    constexpr static bool value = std::is_same<TypeA, TypeB>::value;
};

/// helpers to convert std::array<T, N> to std::array<U, N>
template<typename Dest, typename Src, std::size_t N, std::size_t... Is>
auto convert_array_to_impl(const std::array<Src, N> &src, std::index_sequence<Is...>) {
    return std::array<Dest, N>{{static_cast<Dest>(src[Is])...}};
}

template<typename Dest, typename Src, std::size_t N>
auto convert_array_to(const std::array<Src, N> &src) {
    return convert_array_to_impl<Dest>(src, std::make_index_sequence<N>());
}
