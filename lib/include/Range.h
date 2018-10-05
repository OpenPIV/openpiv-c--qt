
#pragma once

// std
#include <algorithm>
#include <optional>
#include <type_traits>
#include <vector>

template <typename T>
auto make_range( const T& s );

/// simple Range class; inefficient but fine for loop initialization
template <typename T, typename E = typename std::enable_if<std::is_integral<T>::value>::type>
class Range
{
    Range( T s ) : s_( s ) {}
    std::optional< T > s_;

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
