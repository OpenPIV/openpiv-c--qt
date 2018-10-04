
#pragma once

// std
#include <cmath>
#include <limits>
#include <initializer_list>
#include <istream>
#include <numeric>
#include <experimental/optional>
#include <sstream>
#include <thread>
#include <typeinfo>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

template <typename T>
void* typed_memcpy( void* dest, const T* src, size_t count )
{
    return memcpy( dest, src, count*sizeof( T ) );
}

template <typename E,
          typename = typename std::enable_if_t< std::is_enum<E>::value >>
class EnumHelper
{
public:
    using underlying_t = typename std::underlying_type<E>::type;
    using map_t = std::unordered_map< E, std::string >;

    static map_t& storage()
    {
        static map_t storage_;
        return storage_;
    }

    static bool init(std::initializer_list<std::pair<E, std::string>> l)
    {
        for ( auto i : l )
            storage().emplace( std::move( i ) );

        return true;
    }
};

#define DECLARE_ENUM_HELPER( E, ... )                                   \
    static const auto e##EnumHelper__ = EnumHelper<E>::init( __VA_ARGS__ ); \
    std::ostream& operator<<( std::ostream& os, E e )                   \
    {                                                                   \
        if ( EnumHelper<E>::storage().empty() )                         \
            os << EnumHelper<E>::underlying_t(e);                       \
        else                                                            \
            os << EnumHelper<E>::storage().at(e);                       \
        return os;                                                      \
    }

/// simple entry/exit logger with indent
class EntryExitLogger
{
private:
    static thread_local uint8_t indent;

    std::ostream& os_;
    std::string s_;
    std::thread::id id_;

public:
    EntryExitLogger( std::ostream& os, const std::string& s )
        : os_( os ),
          s_( s ),
          id_( std::this_thread::get_id() )
    {
        ++indent;
        os_ << std::string( indent, '>' ) << " (" << id_ << ") " << s_ << "\n";
    }

    ~EntryExitLogger()
    {
        os_ << std::string( indent, '<' ) << " (" << id_ << ") " << s_ << "\n";
        --indent;
    }
};

#if defined(DEBUG)
# define DECLARE_ENTRY_EXIT EntryExitLogger __func__##EntryExitLogger__( std::cout, __func__ );
#else
# define DECLARE_ENTRY_EXIT
#endif

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

/// simple RAII for std::istream multi-char peek; allows caller
/// to "peek" at multiple bytes and will restore stream upon destruction:
///
///
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
        Thrower<std::range_error>() << "unable to convert " << v
                                    << " to " << typeid(To).name() << " as value would be truncated";

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
