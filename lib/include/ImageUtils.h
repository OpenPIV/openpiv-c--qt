
#pragma once

// std
#include <type_traits>

// local
#include "Image.h"

/// fill an image with a constant value
template < template<typename> class ImageT, typename ContainedT >
ImageInterface< ImageT, ContainedT >& fill( ImageInterface< ImageT, ContainedT >& im, ContainedT value )
{
    auto p = std::begin( im );
    auto e = std::cend( im );
    while ( p != e )
        *p++ = value;

    return im;
}

template < template<typename> class ImageT, typename ContainedT, typename R >
R pixel_sum_impl( const ImageInterface< ImageT, ContainedT >& im )
{
    R result = 0;
    auto p = std::begin( im );
    auto e = std::cend( im );
    while ( p != e )
    {
        result += *p++;
    }

    return result;
}

/// find the sum of all pixels
template < template<typename> class ImageT, typename ContainedT >
typename std::enable_if<std::is_integral<ContainedT>::value, int64_t>::type
pixel_sum( const ImageInterface< ImageT, ContainedT >& im )
{
    return pixel_sum_impl<ImageT, ContainedT, int64_t>(im);
}

template < template<typename> class ImageT, typename ContainedT >
typename std::enable_if<!std::is_integral<ContainedT>::value, double>::type
pixel_sum( const ImageInterface< ImageT, ContainedT >& im )
{
    return pixel_sum_impl<ImageT, ContainedT, double>(im);
}

/// split an RGBA image into channels
template < template<typename> class ImageT,
           typename T,
           typename ReturnImageT = Image<G<T>>,
           typename R = std::tuple< ReturnImageT, ReturnImageT, ReturnImageT, ReturnImageT >>
R
split_to_channels( const ImageInterface< ImageT, RGBA<T> >& rgba )
{
    Image<G<T>> r_im( rgba.width(), rgba.height() );
    Image<G<T>> g_im( rgba.width(), rgba.height() );
    Image<G<T>> b_im( rgba.width(), rgba.height() );
    Image<G<T>> a_im( rgba.width(), rgba.height() );

    auto r = std::begin( r_im );
    auto g = std::begin( g_im );
    auto b = std::begin( b_im );
    auto a = std::begin( a_im );

    auto p = std::cbegin( rgba );
    auto e = std::cend( rgba );
    while ( p != e )
    {
        *r++ = p->r;
        *g++ = p->g;
        *b++ = p->b;
        *a++ = p->a;
        ++p;
    }

    return std::make_tuple( r_im, g_im, b_im, a_im );
}

/// join channels into an RGBA image
template < template<typename> class ImageT,
           typename T,
           typename ReturnImageT = Image<RGBA<T>> >
ReturnImageT
join_from_channels( const ImageInterface< ImageT, G<T>>& r_im,
                    const ImageInterface< ImageT, G<T>>& g_im,
                    const ImageInterface< ImageT, G<T>>& b_im,
                    const ImageInterface< ImageT, G<T>>& a_im )
{
    if ( r_im.size() != g_im.size() ||
         g_im.size() != b_im.size() ||
         b_im.size() != a_im.size() )
        Thrower<std::runtime_error>() << "source images must have matching dimensions";

    Image<RGBA<T>> rgba_im( r_im.width(), r_im.height() );

    auto rgba = std::begin( rgba_im );

    auto r = std::cbegin( r_im );
    auto e = std::cend( r_im );
    auto g = std::cbegin( g_im );
    auto b = std::cbegin( b_im );
    auto a = std::cbegin( a_im );

    while ( r != e )
    {
        rgba->r = *r++;
        rgba->g = *g++;
        rgba->b = *b++;
        rgba->a = *a++;
        ++rgba;
    }

    return rgba_im;
}
