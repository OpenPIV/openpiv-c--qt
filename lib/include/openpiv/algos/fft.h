#pragma once

// std
#include <algorithm>
#include <exception>
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

namespace openpiv::core {

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
/// This class is not thread-safe; if performing FFT in multiple
/// threads then a new FFT instance should be created per-thread;
/// this is because the FFT class holds some intermediate storage
/// which is maintained until the next call to FFT::transform()
class FFT
{
    size size_;
    cf_image output_;
    std::vector< c_f > fft_buffer_;
    cf_image temp_;

    /// contains "twiddle factors" for each n: 2^n < N, n>=0
    using scaling_map_t = std::unordered_map< size_t, std::vector<c_f> >;
    scaling_map_t forward_scaling_;
    scaling_map_t reverse_scaling_;

    /// track id of thread that created this instance
    std::thread::id thread_id_;

public:
    FFT( const core::size& size )
        : size_(size)
    {
        // ensure power-of-two sizes
        if ( !(is_pow2(size_.width()) && is_pow2(size_.height()) ) )
            exception_builder<std::runtime_error>() << "dimensions must be power of 2: " << size_;

        // store thread id
        thread_id_ = std::this_thread::get_id();

        // allocate output, intermediate storage
        size_t N{ maximal_size( size ).width() };
        output_.resize( size );
        temp_.resize( transpose(size) );
        fft_buffer_.resize( N );

        // make scaling factors
        do {
            forward_scaling_[N] = generate_scaling_factors(N, direction::FORWARD);
            reverse_scaling_[N] = generate_scaling_factors(N, direction::REVERSE);
            N >>= 1;
        } while (N > 2);
    }

    /// Perform a 2-D FFT; will always produce a complex floating point image output
    template < template <typename> class ImageT,
               typename ContainedT,
               typename = typename std::enable_if_t< is_imagetype<ImageT<ContainedT>>::value >
               >
    const cf_image& transform( const ImageT<ContainedT>& input, direction d = direction::FORWARD )
    {
        DECLARE_ENTRY_EXIT

        // sanity checks
        if ( std::this_thread::get_id() != thread_id_ )
        {
            exception_builder< std::runtime_error >() << "calling FFT::transform() from wrong thread";
        }

        if ( input.size() != size_ )
        {
            exception_builder< std::runtime_error >()
                << "image size is different from expected: " << input.size() << ", " << size_;
        }

        // copy data, converting to complex
        output_.operator=( input );

        // iterate over rows first
        for ( uint32_t h = 0; h<output_.height(); ++h )
            fft( output_.line(h), output_.width(), d );

        // transpose outout -> temp
        transpose( output_, temp_ );

        // now do columns
        for ( uint32_t h = 0; h<temp_.height(); ++h )
            fft( temp_.line(h), temp_.width(), d );

        // flip back: temp -> output
        transpose( temp_, output_ );

        return output_;
    }

    template < template <typename> class ImageT,
               typename ContainedT,
               typename = typename std::enable_if_t< is_imagetype<ImageT<ContainedT>>::value >
               >
    cf_image cross_correlate( const ImageT<ContainedT>& a, const ImageT<ContainedT>& b )
    {
        cf_image a_fft{ transform( a, direction::FORWARD ) };
        const cf_image& b_fft = transform( b, direction::FORWARD );

        a_fft = b_fft * conj( a_fft );
        output_ = real( transform( a_fft, direction::REVERSE ) );
        swap_quadrants( output_ );

        return output_;
    }

    template < template <typename> class ImageT,
               typename ContainedT,
               typename = typename std::enable_if_t< is_imagetype<ImageT<ContainedT>>::value >
               >
    cf_image auto_correlate( const ImageT<ContainedT>& a )
    {
        cf_image a_fft{ transform( a, direction::FORWARD ) };

        a_fft = abs_sqr( a_fft );
        output_ = real( transform( a_fft, direction::REVERSE ) );
        swap_quadrants( output_ );

        return output_;
    }

private:
    void fft_inner( c_f* in, c_f* out, size_t n, size_t step, direction d )
    {
        DECLARE_ENTRY_EXIT

        const scaling_map_t& scaling_ = d == direction::FORWARD ? forward_scaling_ : reverse_scaling_;
        if (step < n)
        {
            const size_t doublestep{ 2*step };
            fft_inner(out,        in,        n, doublestep, d);
            fft_inner(out + step, in + step, n, doublestep, d);

            for ( size_t i=0; i<n; i+=doublestep )
            {
                c_f s = scaling_.at(n)[i];
                c_f e{ out[i] };
                c_f o{ out[i + step] };
                in[i/2]        = e + s*o;
                in[(i + n)/2]  = e - s*o;
            }
        }
    }

    void fft( c_f* in, size_t n, direction d, size_t stride = 1 )
    {
        DECLARE_ENTRY_EXIT

        typed_memcpy( &fft_buffer_[0], in, n, stride );
        fft_inner( in, &fft_buffer_[0], n, 1, d );
    }

    static std::vector<c_f> generate_scaling_factors( size_t n, direction d )
    {
        const double scaling{ d == direction::FORWARD ? -1.0 : 1.0 };
        std::vector<c_f> result(n);
        std::generate(
            std::begin(result),
            std::end(result),
            [i=0, n, d, scaling]() mutable {
                const double theta = (scaling * M_PI * i++)/n;
                return c_f{ std::cos( theta ), std::sin( theta ) };
            } );

        return result;
    }
};

}
