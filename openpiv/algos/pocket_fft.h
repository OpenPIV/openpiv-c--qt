#pragma once

// std
#include <algorithm>
#include <exception>
#include <functional>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <vector>

// pocket
#define POCKETFFT_NO_MULTITHREADING
#include "pocketfft_hdronly.h"
#undef POCKETFFT_NO_MULTITHREADING

// local
#include "algos/fft_common.h"
#include "core/enum_helper.h"
#include "core/exception_builder.h"
#include "core/image.h"
#include "core/image_utils.h"
#include "core/pixel_types.h"
#include "core/util.h"

namespace openpiv::algos {

    using namespace core;
    namespace pfft = pocketfft;

    /// Wrapper for PocketFFT
    ///
    /// This class is thread-safe
    class PocketFFT
    {
        const size size_;

        /// storage for intermediate data
        struct data_t
        {
            image_c_f output;
            std::vector< c_f > fft_buffer;
            image_c_f temp;
        };

        /// helpers to allow TLS for intermediate storage
        using storage_t = std::vector< std::tuple<PocketFFT*, data_t> >;
        storage_t& storage() const
        {
            thread_local static storage_t static_data;
            return static_data;
        }

        /// \fn cache contains a per-thread, per-instance copy of data
        /// that is lazily initialized; this allows a single instance
        /// of FFT to be called from multiple threads without locking
        data_t& cache() const
        {
            PocketFFT* self = const_cast<PocketFFT*>(this);
            for ( auto& [fft, data] : storage() )
            {
                if ( fft == self && data.output.size() == size_ )
                    return data;
            }

            data_t data;
            size_t N{ maximal_size( size_ ).width() };
            data.output.resize( size_ );
            data.temp.resize( transpose(size_) );
            data.fft_buffer.resize( N );
            auto& [fft, result] = storage().emplace_back(self, std::move(data));

            return result;
        }

    public:
        PocketFFT( const core::size& size )
            : size_(size)
        {
            // ensure power-of-two sizes
            if ( !(is_pow2(size_.width()) && is_pow2(size_.height()) ) )
                exception_builder<std::runtime_error>() << "dimensions must be power of 2: " << size_;
        }

        /// Perform a 2-D FFT; will always produce a complex floating point image output
        template < template <typename> class ImageT,
                   typename ContainedT,
                   typename = typename std::enable_if_t< is_imagetype_v<ImageT<ContainedT>> >
                   >
        const image_c_f& transform( const ImageT<ContainedT>& input, direction d = direction::FORWARD ) const
        {
            DECLARE_ENTRY_EXIT
            if ( input.size() != size_ )
            {
                exception_builder< std::runtime_error >()
                    << "image size is different from expected: " << input.size() << ", " << size_;
            }

            using value_t = typename ContainedT::value_t;

            // copy data, converting to complex
            cache().temp = input;
            cache().output.resize( input.size() );

            const pfft::shape_t shape = {size_.width(), size_.height()};
            const auto [stride_x, stride_y] = cache().temp.stride();
            const pfft::stride_t stride = {static_cast<long>(stride_x), static_cast<long>(stride_y)};

            // can reinterpret core::complex to std::complex because core::complex is packed and
            // std::complex is also packed and makes guarantees about accessibility through array
            // access
            pfft::c2c<value_t>(
                shape,
                stride,
                stride,                  // input and output strides should be the same
                { 0, 1 },                // axes
                d == direction::FORWARD, // forward
                reinterpret_cast<const std::complex<value_t>*>(cache().temp.data()),
                reinterpret_cast<std::complex<value_t>*>(cache().output.data()),
                1.0 );

            return cache().output;
        }

        /// Perform a 2-D FFT of two real images; will produce two
        /// output images
        template < template <typename> class ImageT,
                   typename ContainedT,
                   typename = typename std::enable_if_t<
                       is_imagetype_v<ImageT<ContainedT>> &&
                       is_real_mono_pixeltype_v<ContainedT>
                       >
                   >
        std::tuple<image_c_f&, image_c_f&>
        transform_real( const ImageT<ContainedT>& a,
                        const ImageT<ContainedT>& b,
                        direction d = direction::FORWARD ) const
        {
            DECLARE_ENTRY_EXIT
            if ( a.size() != size_ || b.size() != size_ )
            {
                exception_builder< std::runtime_error >()
                    << "image size is different from expected: " << a.size()
                    << ", " << size_;
            }

            using value_t = typename ContainedT::value_t;

            cache().output.resize( a.size() );
            cache().temp.resize( b.size() );

            auto& out_a = cache().output;
            auto& out_b = cache().temp;

            constexpr auto stride_lambda = [](auto& im) -> pfft::stride_t
                {
                    const auto [stride_x, stride_y] = im.stride();
                    return {static_cast<long>(stride_x), static_cast<long>(stride_y)};
                };

            const pfft::shape_t shape = {size_.width(), size_.height()};
            const pfft::stride_t in_a_stride = stride_lambda(a);
            const pfft::stride_t in_b_stride = stride_lambda(b);
            const pfft::stride_t out_a_stride = stride_lambda(out_a);
            const pfft::stride_t out_b_stride = stride_lambda(out_b);

            pfft::r2c<value_t>(
                shape,
                in_a_stride,
                out_a_stride,
                { 0, 1 },                // axes
                d == direction::FORWARD, // forward
                reinterpret_cast<const value_t*>(a.data()),
                reinterpret_cast<std::complex<value_t>*>(out_a.data()),
                1.0 );

            pfft::r2c<value_t>(
                shape,
                in_b_stride,
                out_b_stride,
                { 0, 1 },                // axes
                d == direction::FORWARD, // forward
                reinterpret_cast<const value_t*>(b.data()),
                reinterpret_cast<std::complex<value_t>*>(out_b.data()),
                1.0 );

            return { out_a, out_b };
        }

        template < template <typename> class ImageT,
                   typename ContainedT,
                   typename ValueT = typename ContainedT::value_t,
                   typename OutT = image<g<ValueT>>,
                   typename = typename std::enable_if_t<
                       is_imagetype_v<ImageT<ContainedT>> &&
                       is_complex_mono_pixeltype_v<ContainedT>
                       >
                   >
        OutT
        transform_real( const ImageT<ContainedT>& in,
                        direction d = direction::FORWARD ) const
        {
            DECLARE_ENTRY_EXIT
            if ( in.size() != size_ )
            {
                exception_builder< std::runtime_error >()
                    << "image size is different from expected: " << in.size()
                    << ", " << size_;
            }

            OutT out{ in.size() };

            constexpr auto stride_lambda = [](auto& im) -> pfft::stride_t
                {
                    const auto [stride_x, stride_y] = im.stride();
                    return {static_cast<long>(stride_x), static_cast<long>(stride_y)};
                };

            const pfft::shape_t shape = {size_.width(), size_.height()};
            const pfft::stride_t in_stride = stride_lambda(in);
            const pfft::stride_t out_stride = stride_lambda(out);

            pfft::c2r<ValueT>(
                shape,
                in_stride,
                out_stride,
                { 0, 1 },                // axes
                d == direction::FORWARD, // forward
                reinterpret_cast<const std::complex<ValueT>*>(in.data()),
                reinterpret_cast<ValueT*>(out.data()),
                1.0 );

            return out;
        }

        template < template <typename> class ImageT,
                   typename ContainedT,
                   typename ValueT = typename ContainedT::value_t,
                   typename OutT = image<g<ValueT>>,
                   typename = typename std::enable_if_t< is_imagetype_v<ImageT<ContainedT>> >
                   >
        OutT
        cross_correlate( const ImageT<ContainedT>& a,
                         const ImageT<ContainedT>& b ) const
        {
            image_c_f a_fft{ transform( a, direction::FORWARD ) };
            image_c_f b_fft{ transform( b, direction::FORWARD ) };

            a_fft = b_fft * conj( a_fft );
            OutT output{ real( transform( a_fft, direction::REVERSE ) ) };
            swap_quadrants( output );

            return output;
        }

        template < template <typename> class ImageT,
                   typename ContainedT,
                   typename ValueT = typename ContainedT::value_t,
                   typename OutT = image<g<ValueT>>,
                   typename = typename std::enable_if_t<
                       is_imagetype_v<ImageT<ContainedT>> &&
                       is_real_mono_pixeltype_v<ContainedT>
                       >
                   >
        OutT
        cross_correlate_real( const ImageT<ContainedT>& a,
                              const ImageT<ContainedT>& b ) const
        {
            auto [a_fft, b_fft] = transform_real( a, b, direction::FORWARD );
            a_fft = b_fft * conj( a_fft );
            OutT output = transform_real( a_fft, direction::REVERSE );
            swap_quadrants( output );

            return output;
        }

        template < template <typename> class ImageT,
                   typename ContainedT,
                   typename = typename std::enable_if_t< is_imagetype_v<ImageT<ContainedT>> >
                   >
        const image_c_f& auto_correlate( const ImageT<ContainedT>& a ) const
        {
            image_c_f a_fft{ transform( a, direction::FORWARD ) };

            a_fft = abs_sqr( a_fft );
            cache().output = real( transform( a_fft, direction::REVERSE ) );
            swap_quadrants( cache().output );

            return cache().output;
        }
    };

}
