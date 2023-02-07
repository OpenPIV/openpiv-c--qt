
#pragma once

// openpiv
#include "core/image.h"

// std
#include <array>
#include <vector>

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

template <size_t N, size_t M>
static constexpr bool
operator==(const char_array<N>& lhs, const char (&rhs)[M])
{
    static_assert(N >= M);
    for (size_t i=0; i<M; ++i)
        if (lhs[i] != rhs[i])
            return false;

    return true;
}

// tests
static_assert(to_array<6>("hello") == "hello");

static constexpr auto h = to_array<10>("hello");
static_assert(h.size() == 10);
static_assert(h == "hello");

static constexpr auto w = to_array<10>("world");
static_assert(w.size() == 10);
static_assert(w == "world");

static constexpr auto c = cat<21>(h, w);
static_assert(c.size() == 21);
static_assert(c == "hello_world");

template <typename T>
struct pixel_underlying_type_trait
{
    template <size_t N=15>
    static constexpr char_array<N> name()
    {
        if constexpr (std::is_same_v<T, uint8_t> )
            return to_array<N>("u8");
        else if constexpr (std::is_same_v<T, uint16_t> )
            return to_array<N>("u16");
        else if constexpr (std::is_same_v<T, uint32_t> )
            return to_array<N>("u32");
        else if constexpr (std::is_same_v<T, int8_t> )
            return to_array<N>("8");
        else if constexpr (std::is_same_v<T, int16_t> )
            return to_array<N>("16");
        else if constexpr (std::is_same_v<T, int32_t> )
            return to_array<N>("32");
        else if constexpr (std::is_same_v<T, double> )
            return to_array<N>("f");

        return to_array<N>("unknown type");
    }
};

// test
static_assert(pixel_underlying_type_trait<uint8_t>::name() == "u8");
static_assert(pixel_underlying_type_trait<uint16_t>::name() == "u16");
static_assert(pixel_underlying_type_trait<uint32_t>::name() == "u32");
static_assert(pixel_underlying_type_trait<int8_t>::name() == "8");
static_assert(pixel_underlying_type_trait<int16_t>::name() == "16");
static_assert(pixel_underlying_type_trait<int32_t>::name() == "32");
static_assert(pixel_underlying_type_trait<double>::name() == "f");


template <template <typename> class T, typename U>
struct pixel_type_trait
{
    template <size_t N=32>
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

// test
static_assert(pixel_type_trait<g, uint8_t>::name() == "g_u8");
static_assert(pixel_type_trait<rgba, uint16_t>::name() == "rgba_u16");
static_assert(pixel_type_trait<yuva, uint32_t>::name() == "yuva_u32");
static_assert(pixel_type_trait<complex, double>::name() == "c_f");


template <template <typename> class I,
          template <typename> class P,
          typename U>
struct image_type_trait
{
    template <size_t N=64>
    static constexpr char_array<N> name()
    {
        return cat<N>(to_array<8>("image"), pixel_type_trait<P, U>::name());
    }

    /// this returns strides in matrix convention
    static std::vector<size_t> strides(const I<P<U>>& im)
    {
        const auto [column_stride, row_stride] = im.stride();
        if constexpr (are_any_equal_v<P<U>, rgba<U>, yuva<U>>)
            return { row_stride, column_stride, 4 };
        else if constexpr (are_any_equal_v<P<U>, complex<U>>)
            return { row_stride, column_stride, 2 };
        else
            return { row_stride, column_stride };
    }

    /// this returns dimensions in matrix convention
    static std::vector<size_t> dimensions(const I<P<U>>& im)
    {
        if constexpr (are_any_equal_v<P<U>, rgba<U>, yuva<U>>)
            return { im.height(), im.width(), 4 };
        else if constexpr (are_any_equal_v<P<U>, complex<U>>)
            return { im.height(), im.width(), 2 };
        else
            return { im.height(), im.width() };
    }

};

// test
static_assert(image_type_trait<image, g, uint8_t>::name() == "image_g_u8");
static_assert(image_type_trait<image, rgba, uint16_t>::name() == "image_rgba_u16");
static_assert(image_type_trait<image, yuva, uint32_t>::name() == "image_yuva_u32");
static_assert(image_type_trait<image, complex, double>::name() == "image_c_f");

