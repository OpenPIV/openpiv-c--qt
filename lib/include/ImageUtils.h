
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
    const auto e = std::end( im );
    while ( p != e )
        *p++ = value;

    return im;
}

/// fill an image with a constant value
template < typename GeneratorT,
           template<typename> class ImageT,
           typename ContainedT>
ImageInterface< ImageT, ContainedT >& fill( ImageInterface< ImageT, ContainedT >& im, GeneratorT g )
{
    for ( uint32_t h=0; h<im.height(); ++h )
        for ( uint32_t w=0; w<im.width(); ++w )
            im[ {w, h} ] = g( w, h );

    return im;
}

template < template<typename> class ImageT, typename ContainedT, typename R >
R pixel_sum_impl( const ImageInterface< ImageT, ContainedT >& im )
{
    R result = 0;
    auto p = std::begin( im );
    const auto e = std::end( im );
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

/// split a Complex image into (real, imag) images
template < template<typename> class ImageT,
           typename T,
           typename ReturnImageT = Image<G<T>>,
           typename R = std::tuple< ReturnImageT, ReturnImageT >>
R
split_to_channels( const ImageInterface< ImageT, Complex<T> >& c )
{
    Image<G<T>> real_im( c.width(), c.height() );
    Image<G<T>> imag_im( c.width(), c.height() );

    auto r = std::begin( real_im );
    auto i = std::begin( imag_im );

    auto p = std::cbegin( c );
    auto e = std::cend( c );
    while ( p != e )
    {
        *r++ = p->real;
        *i++ = p->imag;
        ++p;
    }

    return std::make_tuple( real_im, imag_im );
}

/// join two images into a Complex image
template < template<typename> class ImageT,
           typename T,
           typename ReturnImageT = Image<Complex<T>> >
ReturnImageT
join_from_channels( const ImageInterface< ImageT, G<T>>& real_im,
                    const ImageInterface< ImageT, G<T>>& imag_im )
{
    if ( real_im.size() != imag_im.size() )
        Thrower<std::runtime_error>() << "source images must have matching dimensions";

    Image<Complex<T>> c_im( real_im.width(), real_im.height() );

    auto c = std::begin( c_im );

    auto r = std::cbegin( real_im );
    auto e = std::cend( real_im );
    auto i = std::cbegin( imag_im );

    while ( r != e )
    {
        c->real = *r++;
        c->imag = *i++;
        ++c;
    }

    return c_im;
}

/// transpose an image i.e. rows <-> columns; maps
/// from \a in to \out out; the dimensions of
/// \a in and \a out must be transposed as a
/// pre-condition.
///
/// \returns a reference to \a out.
template < template<typename> class ImageT,
           typename ContainedT,
           typename ReturnT = ImageInterface< ImageT, ContainedT> >
ReturnT& transpose( const ImageInterface< ImageT, ContainedT >& in, ImageInterface< ImageT, ContainedT >& out )
{
    if ( !(in.width() == out.height() && in.height() == out.width() ) )
        Thrower<std::runtime_error>() << "input and output must have transposed dimensions: "
                                      << in.size() << ", " << out.size();

    // get pointers to all result lines
    std::vector<ContainedT*> olines( out.height(), nullptr );
    for ( uint32_t h=0; h<out.height(); ++h )
        olines[h] = out.line(h);

    for ( uint32_t h=0; h<in.height(); ++h )
    {
        const ContainedT* p = in.line(h);
        for ( uint32_t w=0; w<in.width(); ++w )
            *olines[w]++ = *p++;
    }

    return out;
}

/// transpose an image i.e. rows <-> columns; \returns a new transposed image
template < template<typename> class ImageT,
           typename ContainedT,
           typename ReturnT = Image< ContainedT > >
ReturnT transpose( const ImageInterface< ImageT, ContainedT >& im )
{
    ReturnT result( im.height(), im.width() );
    transpose( im, result );

    return result;
}

/// swap quadrants of an even dimensioned image i.e.
/// - quadrant 1 <-> quadrant 3
/// - quadrant 2 <-> quadrant 4
template < template<typename> class ImageT,
           typename ContainedT,
           typename ReturnT = ImageInterface< ImageT, ContainedT > >
ReturnT& swap_quadrants( ImageInterface< ImageT, ContainedT >& in )
{
    const auto [width, height] = in.size().components();

    for ( uint32_t h=0; h<height; ++h )
    {
        ContainedT* i = in.line( h );
        ContainedT* o = in.line( (h + height/2) % height );

        for ( uint32_t w=0; w<width/2; ++w )
            std::swap( i[w], o[ (w + width/2) % width ] );
    }

    return in;
}
