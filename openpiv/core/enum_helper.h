
#pragma once

// std
#include <initializer_list>
#include <string>
#include <type_traits>
#include <unordered_map>

/// EnumHelper provides a standard way to produce string representations
/// of enumerations. The enum mapping is not particularly efficient being
/// done at initialization time (rather than compile time) however is
/// sufficient.
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
    static std::string to_string(E e)                                   \
    {                                                                   \
        if ( !EnumHelper<E>::storage().count(e) )                       \
            return std::to_string(EnumHelper<E>::underlying_t(e));      \
                                                                        \
        return EnumHelper<E>::storage().at(e);                          \
    }                                                                   \
                                                                        \
    static std::ostream& operator<<( std::ostream& os, E e )            \
    {                                                                   \
        return os << to_string(e);                                      \
    }
