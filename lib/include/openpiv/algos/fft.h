#pragma once

// std
#include <algorithm>
#include <exception>
#include <functional>
#include <thread>
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
        struct data
        {
            cf_image output;
            std::vector< c_f > fft_buffer;
            cf_image temp;
        };

        /// helpers to allow TLS for intermediate storage
        /// and appropriate cleanup
        using cleanup_t = std::vector<std::function<void()>>;
        mutable cleanup_t cleanup_;

        using storage_t = std::unordered_map<FFT*, data>;
        storage_t& storage() const
        {
            thread_local static std::unordered_map<FFT*, data> static_data;
            return static_data;
        }

        /// \fn cache contains a per-thread, per-instance copy of data
        /// that is lazily initialized; this allows a single instance
        /// of FFT to be called from multiple threads without locking
        data& cache() const
        {
            FFT* self = const_cast<FFT*>(this);
            if ( !storage().count(self) )
            {
                auto& d = storage()[self];
                size_t N{ maximal_size( size_ ).width() };
                d.output.resize( size_ );
                d.temp.resize( transpose(size_) );
                d.fft_buffer.resize( N );

                cleanup_.emplace_back( [&s = storage(), self](){ s.erase(self); } );
            }

            return storage().at(self);
        }

        void clear_cache() const
        {
            for ( const auto& c : cleanup_ )
                c();
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

        ~FFT()
        {
            clear_cache();
        }

        /// Perform a 2-D FFT; will always produce a complex floating point image output
        template < template <typename> class ImageT,
                   typename ContainedT,
                   typename = typename std::enable_if_t< is_imagetype<ImageT<ContainedT>>::value >
                   >
        const cf_image& transform( const ImageT<ContainedT>& input, direction d = direction::FORWARD ) const
        {
            DECLARE_ENTRY_EXIT
            if ( input.size() != size_ )
            {
                exception_builder< std::runtime_error >()
                    << "image size is different from expected: " << input.size() << ", " << size_;
            }

            // copy data, converting to complex
            cache().output.operator=( input );

            // iterate over rows first
            for ( uint32_t h = 0; h < cache().output.height(); ++h )
                fft( cache().output.line(h), cache().output.width(), d );

            // transpose outout -> temp
            transpose( cache().output, cache().temp );

            // now do columns
            for ( uint32_t h = 0; h < cache().temp.height(); ++h )
                fft( cache().temp.line(h), cache().temp.width(), d );

            // flip back: temp -> output
            transpose( cache().temp, cache().output );

            return cache().output;
        }

        template < template <typename> class ImageT,
                   typename ContainedT,
                   typename = typename std::enable_if_t< is_imagetype<ImageT<ContainedT>>::value >
                   >
        const cf_image& cross_correlate( const ImageT<ContainedT>& a, const ImageT<ContainedT>& b ) const
        {
            cf_image a_fft{ transform( a, direction::FORWARD ) };
            const cf_image& b_fft = transform( b, direction::FORWARD );

            a_fft = b_fft * conj( a_fft );
            cache().output = real( transform( a_fft, direction::REVERSE ) );
            swap_quadrants( cache().output );

            return cache().output;
        }

        template < template <typename> class ImageT,
                   typename ContainedT,
                   typename = typename std::enable_if_t< is_imagetype<ImageT<ContainedT>>::value >
                   >
        const cf_image& auto_correlate( const ImageT<ContainedT>& a ) const
        {
            cf_image a_fft{ transform( a, direction::FORWARD ) };

            a_fft = abs_sqr( a_fft );
            cache().output = real( transform( a_fft, direction::REVERSE ) );
            swap_quadrants( cache().output );

            return cache().output;
        }

    private:
        void fft_inner( c_f* in, c_f* out, const c_f* scaling, size_t n, size_t step, direction d ) const
        {
            DECLARE_ENTRY_EXIT

            if (step < n)
            {
                const size_t doublestep{ 2*step };
                fft_inner(out,        in,        scaling, n, doublestep, d);
                fft_inner(out + step, in + step, scaling, n, doublestep, d);

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

            typed_memcpy( &cache().fft_buffer[0], in, n, stride );
            const auto& scaling = (d == direction::FORWARD ? forward_scaling_ : reverse_scaling_).at(n);
            fft_inner( in, &cache().fft_buffer[0], &scaling[0], n, 1, d );
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
                    [i=0, n, d, scaling]() mutable {
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
