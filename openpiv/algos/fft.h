#pragma once

// std
#include <algorithm>
#include <exception>
#include <functional>
#include <thread>
#include <tuple>
#include <unordered_map>
#include <vector>

// local
#include "core/enum_helper.h"
#include "core/exception_builder.h"
#include "core/image.h"
#include "core/image_utils.h"
#include "core/pixel_types.h"
#include "core/util.h"

namespace openpiv::algos {

    using namespace core;

    enum class direction {
        FORWARD,
        REVERSE
    };

    DECLARE_ENUM_HELPER( direction, {
            { direction::FORWARD, "forward" },
            { direction::REVERSE, "reverse" }
        } )

    /// A basic decimate-in-time FFT algorithm; not highly optimized.
    ///
    /// This class is thread-safe
    class FFT
    {
        const size size_;

        /// contains "twiddle factors" for each n: 2^n < N, n>=0
        using scaling_map_t = std::unordered_map< size_t, std::vector<c_f> >;
        const scaling_map_t forward_scaling_;
        const scaling_map_t reverse_scaling_;

        /// storage for intermediate data
        struct data_t
        {
            image_c_f output;
            std::vector< c_f > fft_buffer;
            image_c_f temp;
        };

        /// helpers to allow TLS for intermediate storage
        using storage_t = std::vector< std::tuple<FFT*, data_t> >;
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
            FFT* self = const_cast<FFT*>(this);
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
        FFT( const core::size& size )
            : size_(size)
            , forward_scaling_( generate_scaling_factors(size, direction::FORWARD) )
            , reverse_scaling_( generate_scaling_factors(size, direction::REVERSE) )
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

            // copy data, converting to complex
            cache().output = input;
            cache().temp.resize( input.size() );

            // iterate over rows first
            for ( uint32_t h = 0; h < cache().output.height(); ++h )
                fft( cache().output.line(h), cache().output.width(), d );

            // transpose output -> temp
            transpose( cache().output, cache().temp );

            // now do columns
            for ( uint32_t h = 0; h < cache().temp.height(); ++h )
                fft( cache().temp.line(h), cache().temp.width(), d );

            // flip back: temp -> output
            transpose( cache().temp, cache().output );

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
        std::tuple<image_c_f, image_c_f>
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
            cache().output = join_from_channels(a, b);
            cache().temp.resize( cache().output.size() );

            // iterate over rows first
            for ( uint32_t h = 0; h < cache().output.height(); ++h )
                fft( cache().output.line(h), cache().output.width(), d );

            // transpose output -> temp
            transpose( cache().output, cache().temp );

            // now do columns
            for ( uint32_t h = 0; h < cache().temp.height(); ++h )
                fft( cache().temp.line(h), cache().temp.width(), d );

            // flip back: temp -> output
            transpose( cache().temp, cache().output );

            // and unravel
            const auto& transformed = cache().output;
            auto out_a = image_c_f( transformed.size() );
            auto out_b = image_c_f( transformed.size() );

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
        const image_c_f& cross_correlate( const ImageT<ContainedT>& a,
                                         const ImageT<ContainedT>& b ) const
        {
            image_c_f a_fft{ transform( a, direction::FORWARD ) };
            const image_c_f& b_fft = transform( b, direction::FORWARD );

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
        const image_c_f& cross_correlate_real( const ImageT<ContainedT>& a,
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
        const image_c_f& auto_correlate( const ImageT<ContainedT>& a ) const
        {
            image_c_f a_fft{ transform( a, direction::FORWARD ) };

            a_fft = abs_sqr( a_fft );
            cache().output = real( transform( a_fft, direction::REVERSE ) );
            swap_quadrants( cache().output );

            return cache().output;
        }

    private:
        void fft_inner( c_f* in, c_f* out, const c_f* scaling, size_t n, size_t step ) const
        {
            DECLARE_ENTRY_EXIT

            if (step < n)
            {
                const size_t doublestep{ 2*step };
                fft_inner(out,        in,        scaling, n, doublestep);
                fft_inner(out + step, in + step, scaling, n, doublestep);

                for ( size_t i=0; i<n; i+=doublestep )
                {
                    const c_f e{ out[i] };
                    const c_f o{ out[i + step] * scaling[i] };
                    in[ i/2 ]      = e + o;
                    in[ (i+n)/2 ]  = e - o;
                }
            }
        }

        void fft( c_f* in, size_t n, direction d, size_t stride = 1 ) const
        {
            DECLARE_ENTRY_EXIT

            typed_memcpy( cache().fft_buffer.data(), in, n, stride );
            const auto& scaling = (d == direction::FORWARD ? forward_scaling_ : reverse_scaling_).at(n);
            fft_inner( in, cache().fft_buffer.data(), &scaling[0], n, 1 );
        }

        static scaling_map_t generate_scaling_factors( const core::size& size, direction d )
        {
            scaling_map_t result;
            size_t n = maximal_size( size ).width();
            do {
                const double scaling{ d == direction::FORWARD ? -1.0 : 1.0 };
                std::vector<c_f> twiddle(n);
                std::generate(
                    std::begin(twiddle),
                    std::end(twiddle),
                    [i=0, n, scaling]() mutable {
                        const double theta = (scaling * M_PI * i++)/n;
                        return c_f{ std::cos( theta ), std::sin( theta ) };
                    } );

                result[n] = twiddle;
                n /= 2;
            } while (n > 2);

            return result;
        }
    };

}
