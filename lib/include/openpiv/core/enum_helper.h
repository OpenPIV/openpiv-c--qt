
#pragma once

// std
#include <initializer_list>
#include <string>
#include <type_traits>
#include <unordered_map>

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
    static const auto E##EnumHelper__initialize__ = EnumHelper<E>::init( __VA_ARGS__ ); \
    std::ostream& operator<<( std::ostream& os, E e )                   \
    {                                                                   \
        if ( !EnumHelper<E>::storage().count(e) )                       \
            os << EnumHelper<E>::underlying_t(e);                       \
        else                                                            \
            os << EnumHelper<E>::storage().at(e);                       \
        return os;                                                      \
    }
