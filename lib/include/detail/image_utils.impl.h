
#pragma once

// std
#include <algorithm>
#include <type_traits>
#include <vector>

// local
#include "exception_builder.h"
#include "image.h"
#include "image_view.h"
#include "image_type_traits.h"

namespace openpiv::core {

/// get the underlying image
template < typename  ContainedT >
image<ContainedT>& get_underlying( image<ContainedT>& im ) { return im; }

template < typename ContainedT >
const image<ContainedT>& get_underlying( const image<ContainedT>& im ) { return im; }

template < typename ContainedT >
image<ContainedT>& get_underlying( image_view<ContainedT>& iv ) { return iv.underlying(); }

template < typename ContainedT >
const image<ContainedT>& get_underlying( const image_view<ContainedT>& iv ) { return iv.underlying(); }

/// Find highest \a num_peaks peaks in an image and return a vector of peaks.
/// The peaks are returned as \sa image_view and the size of the
/// image_view can be adjusted by setting \a peak_radius
template < template<typename> class ImageT,
           typename ContainedT,
           typename ReturnT = std::vector<image_view<ContainedT>>,
           typename = typename std::enable_if_t< is_imagetype_v<ImageT<ContainedT>> >
           >
ReturnT find_peaks( const ImageT<ContainedT>& im, uint16_t num_peaks, uint32_t peak_radius )
{
    ReturnT result;
    for ( uint32_t h=peak_radius; h<im.height()-2*peak_radius; ++h )
    {
        const ContainedT* above = im.line( h-1 );
        const ContainedT* line = im.line( h );
        const ContainedT* below = im.line( h+1 );

        for ( uint32_t w=peak_radius; w<im.width()-peak_radius; ++w )
            if ( line[w-1] < line[w] && line[w+1] < line[w] && above[w] < line[w]  && below[w] < line[w] )
                result.emplace_back(
                    create_image_view(
                        im,
                        rect( {w-peak_radius, h-peak_radius},
                              {2*peak_radius + 1, 2*peak_radius + 1} ) ) );
    }

    // sort and cull
    std::sort( std::begin(result), std::end(result),
               [peak_radius](const auto& a, const auto& b) -> bool {
                   return b[{peak_radius, peak_radius}] < a[{peak_radius, peak_radius}];
               } );
    result.resize(num_peaks);

    return result;
}

/// apply a function to each pixel
template < template<typename> class ImageT,
           typename ContainedT,
           typename OpT,
           typename ReturnT = ImageT<ContainedT>,
           typename IndexT = typename ImageT<ContainedT>::index_type,
           typename = std::enable_if_t<
               std::is_invocable_v<
                   OpT,
                   typename ImageT<ContainedT>::index_type,
                   typename ImageT<ContainedT>::pixel_type
                   >
               >,
           typename = typename std::enable_if_t< is_imagetype_v<ImageT<ContainedT>> >
           >
ReturnT& apply( ImageT<ContainedT>& im, OpT op )
{
    for ( IndexT i=0; i<im.pixel_count(); ++i )
        im[i] = op(i, im[i]);

    return im;
}

/// fill an image with constant values
template < template<typename> class ImageT,
           typename ContainedT,
           typename ReturnT = ImageT<ContainedT>,
           typename = typename std::enable_if_t< is_imagetype_v<ImageT<ContainedT>> >
           >
ReturnT& fill( ImageT<ContainedT>& im, const ContainedT& v )
{
    for ( uint32_t h=0; h<im.height(); ++h )
        for ( uint32_t w=0; w<im.width(); ++w )
            im[ {w, h} ] = v;

    return im;
}

/// fill an image with values produced by a generator function
template < typename GeneratorT,
           template<typename> class ImageT,
           typename ContainedT,
           typename ReturnT = ImageT<ContainedT>,
           typename = typename std::enable_if_t< is_imagetype_v<ImageT<ContainedT>> >,
           typename = std::enable_if_t<std::is_invocable_v<GeneratorT, uint32_t, uint32_t>>
           >
ReturnT& fill( ImageT<ContainedT >& im, GeneratorT g )
{
    for ( uint32_t h=0; h<im.height(); ++h )
        for ( uint32_t w=0; w<im.width(); ++w )
            im[ {w, h} ] = g( w, h );

    return im;
}

template < template<typename> class ImageT,
           typename ContainedT,
           typename ReturnT,
           typename = typename std::enable_if_t< is_imagetype_v<ImageT<ContainedT>> >
           >
ReturnT pixel_sum_impl( const ImageT<ContainedT>& im )
{
    ReturnT result = 0;
    auto p = std::begin( im );
    const auto e = std::end( im );
    while ( p != e )
    {
        result += *p++;
    }

    return result;
}

/// find the sum of all pixels
template < template<typename> class ImageT,
           typename ContainedT,
           typename = typename std::enable_if_t< is_imagetype_v<ImageT<ContainedT>> >
           >
typename std::enable_if<std::is_integral_v<ContainedT>, int64_t>::type
pixel_sum( const ImageT<ContainedT>& im )
{
    return pixel_sum_impl<ImageT, ContainedT, int64_t>(im);
}

template < template<typename> class ImageT,
           typename ContainedT,
           typename = typename std::enable_if_t< is_imagetype_v<ImageT<ContainedT>> >
           >
typename std::enable_if<!std::is_integral_v<ContainedT>, double>::type
pixel_sum( const ImageT<ContainedT>& im )
{
    return pixel_sum_impl<ImageT, ContainedT, double>(im);
}

/// split an RGBA image into channels
template < template<typename> class ImageT,
           typename T,
           typename ReturnImageT = image<g<T>>,
           typename ReturnT = std::tuple< ReturnImageT, ReturnImageT, ReturnImageT, ReturnImageT >,
           typename = typename std::enable_if_t< is_imagetype_v<ImageT<T>> >
           >
ReturnT split_to_channels( const ImageT<rgba<T>>& rgba )
{
    ReturnImageT r_im( rgba.width(), rgba.height() );
    ReturnImageT g_im( rgba.width(), rgba.height() );
    ReturnImageT b_im( rgba.width(), rgba.height() );
    ReturnImageT a_im( rgba.width(), rgba.height() );

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
           typename ReturnImageT = image<rgba<T>>,
           typename = typename std::enable_if_t< is_imagetype_v<ImageT<T>> >
           >
ReturnImageT join_from_channels( const ImageT<g<T>>& r_im,
                                 const ImageT<g<T>>& g_im,
                                 const ImageT<g<T>>& b_im,
                                 const ImageT<g<T>>& a_im )
{
    if ( r_im.size() != g_im.size() ||
         g_im.size() != b_im.size() ||
         b_im.size() != a_im.size() )
        exception_builder<std::runtime_error>() << "source images must have matching dimensions";

    ReturnImageT rgba_im( r_im.width(), r_im.height() );

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
           typename ReturnImageT = image<g<T>>,
           typename ReturnT = std::tuple< ReturnImageT, ReturnImageT >,
           typename = typename std::enable_if_t< is_imagetype_v<ImageT<T>> >
           >
ReturnT split_to_channels( const ImageT<complex<T>>& c )
{
    ReturnImageT real_im( c.width(), c.height() );
    ReturnImageT imag_im( c.width(), c.height() );

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
           typename ReturnImageT = image<complex<T>>,
           typename = typename std::enable_if_t< is_imagetype_v<ImageT<T>> >
           >
ReturnImageT join_from_channels( const ImageT<g<T>>& real_im, const ImageT<g<T>>& imag_im )
{
    if ( real_im.size() != imag_im.size() )
        exception_builder<std::runtime_error>() << "source images must have matching dimensions";

    ReturnImageT c_im( real_im.width(), real_im.height() );

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
           typename ReturnT = ImageT<ContainedT>,
           typename = typename std::enable_if_t< is_imagetype_v<ImageT<ContainedT>> >
           >
ReturnT& transpose( const ImageT<ContainedT>& in, ImageT<ContainedT>& out )
{
    if ( !(in.width() == out.height() && in.height() == out.width() ) )
        exception_builder<std::runtime_error>()
            << "input and output must have transposed dimensions: "
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
           typename ReturnT = image< ContainedT >,
           typename = typename std::enable_if_t< is_imagetype_v<ImageT<ContainedT>> >
           >
ReturnT transpose( const ImageT<ContainedT>& im )
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
           typename ReturnT = ImageT<ContainedT>,
           typename = typename std::enable_if_t< is_imagetype_v<ImageT<ContainedT>> >
           >
ReturnT& swap_quadrants( ImageT<ContainedT>& in )
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


}
