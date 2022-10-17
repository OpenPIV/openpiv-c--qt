
#pragma once

// std
#include <array>


template <size_t N>
using char_array = std::array<char, N>;

/// concatinate two arrays of length N and M, separated with
/// an instance of \a sep
template <size_t R, size_t N, size_t M>
static constexpr auto
cat(const char_array<N>& a, const char_array<M>& b, char sep='_') -> char_array<R>
{
    static_assert(N + M < R);
    char_array<R> result = {};
    size_t r=0;
    for (size_t i=0; i<N && a[i]; ++i, ++r)
        result[r] = a[i];
    result[r++] = sep;
    for (size_t i=0; i<M && b[i]; ++i, ++r)
        result[r] = b[i];
    result[r] = 0;

    return result;
}

/// convert a c-style array to std::array, including
/// trailing zero
template <size_t R, size_t N>
static constexpr auto
to_array( const char (&s)[N] ) -> char_array<R>
{
    static_assert(N <= R);
    char_array<R> result = {};
    for (size_t i=0; i<N; ++i)
        result[i] = s[i];

    return result;
}


template <typename T>
struct pixel_underlying_type_trait
{
    template <size_t N=4>
    static constexpr char_array<N> name()
    {
        if constexpr (std::is_same_v<T, uint8_t> )
            return to_array<N>("8");
        else if constexpr (std::is_same_v<T, uint16_t> )
            return to_array<N>("16");
        else if constexpr (std::is_same_v<T, uint32_t> )
            return to_array<N>("32");
        else if constexpr (std::is_same_v<T, double> )
            return to_array<N>("f");

        return {};
    }
};

template <template <typename> class T, typename U>
struct pixel_type_trait
{
    template <size_t N=64>
    static constexpr char_array<N> name()
    {
        if constexpr (std::is_same_v<T<U>, rgba<U>> )
            return cat<N>(to_array<8>("rgba"), pixel_underlying_type_trait<U>::name());
        if constexpr (std::is_same_v<T<U>, yuva<U>> )
            return cat<N>(to_array<8>("yuva"), pixel_underlying_type_trait<U>::name());
        if constexpr (std::is_same_v<T<U>, complex<U>> )
            return cat<N>(to_array<8>("c"), pixel_underlying_type_trait<U>::name());
        if constexpr (std::is_same_v<T<U>, g<U>> )
            return cat<N>(to_array<8>("g"), pixel_underlying_type_trait<U>::name());

        return to_array<N>("unknown pixeltype");
    }
};
