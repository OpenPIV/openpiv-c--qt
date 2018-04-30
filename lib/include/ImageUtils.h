
#pragma once

// std
#include <type_traits>

// local
#include "Image.h"

/// fill an image with a constant value
template < typename T >
Image< T >& fill( Image< T >& im, T value )
{
    Image< T > filled( im.width(), im.height(), value );
    im = std::move( filled );

    return im;
}

template < typename T, typename R >
R pixel_sum_impl( const Image< T >& im )
{
    R result = 0;
    const T* p = im.data();
    const T* e = im.data() + im.pixel_count();
    while ( p < e )
    {
        result += *p++;
    }

    return result;
}

/// find the sum of all pixels
template < typename T >
typename std::enable_if<std::is_integral<T>::value, int64_t>::type
pixel_sum( const Image< T >& im )
{
    return pixel_sum_impl<T, int64_t>(im);
}

template < typename T >
typename std::enable_if<!std::is_integral<T>::value, double>::type
pixel_sum( const Image< T >& im )
{
    return pixel_sum_impl<T, double>(im);
}
