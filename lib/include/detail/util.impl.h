
#pragma once

namespace openpiv::core {

template <typename T>
void* typed_memcpy( void* dest, const T* src, size_t count, size_t stride )
{
    if ( stride == 1 )
    {
        return memcpy( dest, src, count*sizeof( T ) );
    }
    else
    {
        T* dest_t = reinterpret_cast<T*>(dest);
        for ( size_t i=0; i<count; ++i )
        {
            *dest_t++ = *src;
            src += stride;
        }

        return dest;
    }
}


template <typename To, typename From>
constexpr
To checked_unsigned_conversion(const From& v)
{
    if constexpr ( std::is_signed_v<To> && std::is_unsigned_v<From> && (sizeof(To) > sizeof(From)) )
    {
        return static_cast<To>(v);
    }
    else if constexpr ( std::is_signed_v<To> && std::is_unsigned_v<From> && (sizeof(To) == sizeof(From)) )
    {
        if (v>std::numeric_limits<To>::max())
            exception_builder<std::range_error>()
                << "unable to convert " << v
                << " to " << typeid(To).name() << " as value would be truncated";

    }

    return static_cast<To>(v);
}

/// helpers to convert std::array<T, N> to std::array<U, N>
template<typename Dest, typename Src, std::size_t N, std::size_t... Is>
auto convert_array_to_impl(const std::array<Src, N> &src, std::index_sequence<Is...>) {
    return std::array<Dest, N>{{static_cast<Dest>(src[Is])...}};
}

template<typename Dest, typename Src, std::size_t N>
auto convert_array_to(const std::array<Src, N> &src) -> std::array<Dest, N>
{
    return convert_array_to_impl<Dest>(src, std::make_index_sequence<N>());
}

}
