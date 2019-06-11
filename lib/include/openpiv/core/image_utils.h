
#pragma once

// std
#include <algorithm>
#include <type_traits>
#include <vector>

// local
#include "core/exception_builder.h"
#include "core/image.h"
#include "core/image_view.h"
#include "core/image_type_traits.h"

namespace openpiv::core {

/// get the underlying image
template < typename  ContainedT >
image<ContainedT>& get_underlying( image<ContainedT>& im );

template < typename ContainedT >
const image<ContainedT>& get_underlying( const image<ContainedT>& im );

template < typename ContainedT >
image<ContainedT>& get_underlying( image_view<ContainedT>& iv );

template < typename ContainedT >
const image<ContainedT>& get_underlying( const image_view<ContainedT>& iv );

/// Find highest \a num_peaks peaks in an image and return a vector of peaks.
/// The peaks are returned as \sa ImageView and the size of the
/// ImageView can be adjusted by setting \a peak_radius
template < template<typename> class ImageT,
           typename ContainedT,
           typename ReturnT = std::vector<image_view<ContainedT>>,
           typename = typename std::enable_if_t< is_imagetype_v<ImageT<ContainedT>> >
           >
ReturnT find_peaks( const ImageT<ContainedT>& im, uint16_t num_peaks, uint32_t peak_radius );

/// apply a function to each pixel
template < template<typename> class ImageT,
           typename ContainedT,
           typename OpT,
           typename ReturnT = ImageT<ContainedT>,
           typename IndexT = typename ImageT<ContainedT>::index_t,
           typename = std::enable_if_t<
               std::is_invocable_v<
                   OpT,
                   typename ImageT<ContainedT>::index_t,
                   typename ImageT<ContainedT>::pixel_t
                   >
               >,
           typename = typename std::enable_if_t< is_imagetype_v<ImageT<ContainedT>> >
           >
ReturnT& apply( ImageT<ContainedT>& im, OpT op );

/// fill an image with constant values
template < template<typename> class ImageT,
           typename ContainedT,
           typename ReturnT = ImageT<ContainedT>,
           typename = typename std::enable_if_t< is_imagetype_v<ImageT<ContainedT>> >
           >
ReturnT& fill( ImageT<ContainedT>& im, const ContainedT& v );

/// fill an image with values produced by a generator function
template < typename GeneratorT,
           template<typename> class ImageT,
           typename ContainedT,
           typename ReturnT = ImageT<ContainedT>,
           typename = typename std::enable_if_t< is_imagetype_v<ImageT<ContainedT>> >,
           typename = std::enable_if_t<std::is_invocable_v<GeneratorT, uint32_t, uint32_t>>
           >
ReturnT& fill( ImageT<ContainedT >& im, GeneratorT g );

/// find the sum of all pixels
template < template<typename> class ImageT,
           typename ContainedT,
           typename = typename std::enable_if_t< is_imagetype_v<ImageT<ContainedT>> >
           >
typename std::enable_if<std::is_integral_v<ContainedT>, int64_t>::type
pixel_sum( const ImageT<ContainedT>& im );

template < template<typename> class ImageT,
           typename ContainedT,
           typename = typename std::enable_if_t< is_imagetype_v<ImageT<ContainedT>> >
           >
typename std::enable_if<!std::is_integral_v<ContainedT>, double>::type
pixel_sum( const ImageT<ContainedT>& im );

/// split an RGBA image into channels
template < template<typename> class ImageT,
           typename T,
           typename ReturnImageT = image<g<T>>,
           typename ReturnT = std::tuple< ReturnImageT, ReturnImageT, ReturnImageT, ReturnImageT >,
           typename = typename std::enable_if_t< is_imagetype_v<ImageT<T>> >
           >
ReturnT split_to_channels( const ImageT<rgba<T>>& rgba );

/// join channels into an RGBA image
template < template<typename> class ImageT,
           typename T,
           typename ReturnImageT = image<rgba<T>>,
           typename = typename std::enable_if_t< is_imagetype_v<ImageT<T>> >
           >
ReturnImageT join_from_channels( const ImageT<g<T>>& r_im,
                                 const ImageT<g<T>>& g_im,
                                 const ImageT<g<T>>& b_im,
                                 const ImageT<g<T>>& a_im );

/// split a Complex image into (real, imag) images
template < template<typename> class ImageT,
           typename T,
           typename ReturnImageT = image<g<T>>,
           typename ReturnT = std::tuple< ReturnImageT, ReturnImageT >,
           typename = typename std::enable_if_t< is_imagetype_v<ImageT<T>> >
           >
ReturnT split_to_channels( const ImageT<complex<T>>& c );

/// join two images into a Complex image
template < template<typename> class ImageT,
           typename T,
           typename ReturnImageT = image<complex<T>>,
           typename = typename std::enable_if_t< is_imagetype_v<ImageT<T>> >
           >
ReturnImageT join_from_channels( const ImageT<g<T>>& real_im, const ImageT<g<T>>& imag_im );

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
ReturnT& transpose( const ImageT<ContainedT>& in, ImageT<ContainedT>& out );

/// transpose an image i.e. rows <-> columns; \returns a new transposed image
template < template<typename> class ImageT,
           typename ContainedT,
           typename ReturnT = image< ContainedT >,
           typename = typename std::enable_if_t< is_imagetype_v<ImageT<ContainedT>> >
           >
ReturnT transpose( const ImageT<ContainedT>& im );

/// swap quadrants of an even dimensioned image i.e.
/// - quadrant 1 <-> quadrant 3
/// - quadrant 2 <-> quadrant 4
template < template<typename> class ImageT,
           typename ContainedT,
           typename ReturnT = ImageT<ContainedT>,
           typename = typename std::enable_if_t< is_imagetype_v<ImageT<ContainedT>> >
           >
ReturnT& swap_quadrants( ImageT<ContainedT>& in );

}

#include "core/detail/image_utils.impl.h"
