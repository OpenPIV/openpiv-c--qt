
#pragma once

// std
#include <array>

template <size_t N>
using char_array = std::array<char, N>;

template <size_t R, size_t N, size_t M>
static constexpr auto
cat(const char_array<N>& a, const char_array<M>& b) -> char_array<R>
{
    static_assert(N + M < R);
    char_array<R> result = {};
    size_t r=0;
    for (size_t i=0; i<N && a[i]; ++i, ++r)
        result[r] = a[i];
    result[r++] = '_';
    for (size_t i=0; i<M && b[i]; ++i, ++r)
        result[r] = b[i];
    result[r] = 0;

    return result;
}

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

