
#pragma once

// std
#include <sstream>
#include <type_traits>
#include <utility>

/// wrapper to allow using stringstream to construct an
/// exception message; throw \ta E on destruction.
template < typename E >
class Thrower
{
public:
    Thrower() = default;
    ~Thrower()
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
        are_all_convertible<TypeA, R...>::value;
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
