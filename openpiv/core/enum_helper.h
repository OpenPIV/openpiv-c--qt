
#pragma once

// std
#include <initializer_list>
#include <iostream>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

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
    struct mapping_t {
        mapping_t( std::pair<E, std::string>&& v )
            : e(v.first)
            , s(std::move(v.second))
        {}

        E e;
        std::string s;
    };

    using storage_t = std::vector<mapping_t>;

    static storage_t& storage()
    {
        static storage_t storage_;
        return storage_;
    }

    static bool init(std::initializer_list<std::pair<E, std::string>> l)
    {
        for ( auto i : l )
            storage().push_back( std::move( i ) );

        return true;
    }
};

#define DECLARE_ENUM_HELPER( E, ... )                                   \
    static const auto E##EnumHelper__initialize__ = EnumHelper<E>::init( __VA_ARGS__ ); \
                                                                        \
    [[maybe_unused]] static std::string to_string(E e)                  \
    {                                                                   \
        for (const auto& v : EnumHelper<E>::storage())                  \
            if ( v.e == e )                                             \
                return v.s;                                             \
                                                                        \
        return std::to_string(EnumHelper<E>::underlying_t(e));          \
    }                                                                   \
                                                                        \
    template <typename E>                                               \
    [[maybe_unused]] static E from_string(const std::string_view& s)    \
    {                                                                   \
        for (const auto& v : EnumHelper<E>::storage())                  \
            if ( v.s == s )                                             \
                return v.e;                                             \
                                                                        \
        return {};                                                      \
    }                                                                   \
                                                                        \
    [[maybe_unused]] static std::istream& operator>>( std::istream& is, E& e ) \
    {                                                                   \
        std::string s;                                                  \
        is >> s;                                                        \
        e = from_string<E>(s);                                          \
        return is;                                                      \
    }                                                                   \
                                                                        \
    [[maybe_unused]] static std::ostream& operator<<( std::ostream& os, E e ) \
    {                                                                   \
        return os << to_string(e);                                      \
    }
