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
            cf_image output;
            std::vector< c_f > fft_buffer;
            cf_image temp;
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
        const cf_image& transform( const ImageT<ContainedT>& input, direction d = direction::FORWARD ) const
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
        std::tuple<cf_image, cf_image>
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

            // copy data to (real, imag), converting to complex
            cache().output = transform( join_from_channels(a, b), d );

            // and unravel
            const auto& transformed = cache().output;
            auto out_a = cf_image( transformed.size() );
            auto out_b = cf_image( transformed.size() );

            const auto width = transformed.width();
            const auto height = transformed.height();
            for ( uint32_t h=1; h<height/2; ++h)
            {
                for (uint32_t w=1; w<width; ++w)
                {
                    const auto t1 = transformed[ {w, h} ];
                    const auto t2 = transformed[ {width - w, height - h} ];
                    auto a = 0.5*(t1 + t2.conj());
                    out_a[ {w, h} ] = a;
                    out_a[ {width - w, height - h} ] = a.conj();

                    auto b = 0.5*(t1 - t2.conj());
                    b = c_f(b.imag, -1.0 * b.real);
                    out_b[ {w, h} ] = b;
                    out_b[ {width - w, height - h} ] = b.conj();
                }
            }

            return { out_a, out_b };
        }

        template < template <typename> class ImageT,
                   typename ContainedT,
                   typename = typename std::enable_if_t< is_imagetype_v<ImageT<ContainedT>> >
                   >
        const cf_image& cross_correlate( const ImageT<ContainedT>& a,
                                         const ImageT<ContainedT>& b ) const
        {
            cf_image a_fft{ transform( a, direction::FORWARD ) };
            cf_image b_fft{ transform( b, direction::FORWARD ) };

            a_fft = b_fft * conj( a_fft );
            cache().output = real( transform( a_fft, direction::REVERSE ) );
            swap_quadrants( cache().output );

            return cache().output;
        }

        template < template <typename> class ImageT,
                   typename ContainedT,
                   typename = typename std::enable_if_t<
                       is_imagetype_v<ImageT<ContainedT>> &&
                       is_real_mono_pixeltype_v<ContainedT>
                       >
                   >
        const cf_image& cross_correlate_real( const ImageT<ContainedT>& a,
                                              const ImageT<ContainedT>& b ) const
        {
            auto [a_fft, b_fft] = transform_real( a, b, direction::FORWARD );
            a_fft = b_fft * conj( a_fft );
            cache().output = real( transform( a_fft, direction::REVERSE ) );
            swap_quadrants( cache().output );

            return cache().output;
        }

        template < template <typename> class ImageT,
                   typename ContainedT,
                   typename = typename std::enable_if_t< is_imagetype_v<ImageT<ContainedT>> >
                   >
        const cf_image& auto_correlate( const ImageT<ContainedT>& a ) const
        {
            cf_image a_fft{ transform( a, direction::FORWARD ) };

            a_fft = abs_sqr( a_fft );
            cache().output = real( transform( a_fft, direction::REVERSE ) );
            swap_quadrants( cache().output );

            return cache().output;
        }
    };

}
