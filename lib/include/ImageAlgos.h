#pragma once

// std
#include <algorithm>
#include <exception>
#include <thread>
#include <unordered_map>
#include <vector>

// local
#include "EnumHelper.h"
#include "Image.h"
#include "PixelTypes.h"
#include "ImageUtils.h"
#include "Util.h"


enum class Direction {
    FORWARD,
    REVERSE
};

DECLARE_ENUM_HELPER( Direction, {
        { Direction::FORWARD, "forward" },
        { Direction::REVERSE, "reverse" }
} )

/// A basic decimate-in-time FFT algorithm; not highly optimized.
///
/// This class is not thread-safe; if performing FFT in multiple
/// threads then a new FFT instance should be created per-thread;
/// this is because the FFT class holds some intermediate storage
/// which is maintained until the next call to FFT::transform()
class FFT
{
    Size size_;
    CFImage output_;
    std::vector< CF > fft_buffer_;
    CFImage temp_;

    /// contains "twiddle factors" for each n: 2^n < N, n>=0
    using scaling_map_t = std::unordered_map< size_t, std::vector<CF> >;
    scaling_map_t forward_scaling_;
    scaling_map_t reverse_scaling_;

    /// track id of thread that created this instance
    std::thread::id thread_id_;

public:
    FFT( const Size& size )
        : size_(size)
    {
        // ensure power-of-two sizes
        if ( !(is_pow2(size_.width()) && is_pow2(size_.height()) ) )
            Thrower<std::runtime_error>() << "dimensions must be power of 2: " << size_;

        // store thread id
        thread_id_ = std::this_thread::get_id();

        // allocate output, intermediate storage
        size_t N{ maximal_size( size ).width() };
        output_.resize( size );
        temp_.resize( transpose(size) );
        fft_buffer_.resize( N );

        // make scaling factors
        do {
            forward_scaling_[N] = generateScalingFactors(N, Direction::FORWARD);
            reverse_scaling_[N] = generateScalingFactors(N, Direction::REVERSE);
            N >>= 1;
        } while (N > 2);
    }

    /// Perform a 2-D FFT; will always produce a complex floating point image output
    template < template <typename> class ImageT, typename ContainedT >
    const CFImage& transform( const ImageInterface< ImageT, ContainedT >& input, Direction d = Direction::FORWARD )
    {
        DECLARE_ENTRY_EXIT

        // sanity checks
        if ( std::this_thread::get_id() != thread_id_ )
        {
            Thrower< std::runtime_error >() << "calling FFT::transform() from wrong thread";
        }

        if ( input.size() != size_ )
        {
            Thrower< std::runtime_error >()
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

    template < template <typename> class ImageT, typename ContainedT >
    CFImage cross_correlate( const ImageInterface< ImageT, ContainedT >& a, const ImageInterface< ImageT, ContainedT >& b )
    {
        CFImage a_fft{ transform( a, Direction::FORWARD ) };
        const CFImage& b_fft = transform( b, Direction::FORWARD );

        a_fft = b_fft * conj( a_fft );
        output_ = real( transform( a_fft, Direction::REVERSE ) );
        swap_quadrants( output_ );

        return output_;
    }

    template < template <typename> class ImageT, typename ContainedT >
    CFImage auto_correlate( const ImageInterface< ImageT, ContainedT >& a )
    {
        CFImage a_fft{ transform( a, Direction::FORWARD ) };

        a_fft = abs_sqr( a_fft );
        output_ = real( transform( a_fft, Direction::REVERSE ) );
        swap_quadrants( output_ );

        return output_;
    }

private:
    void fft_inner( CF* in, CF* out, size_t n, size_t step, Direction d )
    {
        DECLARE_ENTRY_EXIT

        if (step < n)
        {
            const size_t doublestep{ 2*step };
            fft_inner(out,        in,        n, doublestep, d);
            fft_inner(out + step, in + step, n, doublestep, d);

            for ( size_t i=0; i<n; i+=doublestep )
            {
                const scaling_map_t& scaling_ = d == Direction::FORWARD ? forward_scaling_ : reverse_scaling_;
                CF s = scaling_.at(n)[i];
                CF e{ out[i] };
                CF o{ out[i + step] };
                in[i/2]        = e + s*o;
                in[(i + n)/2]  = e - s*o;
            }
        }
    }

    void fft( CF* in, size_t n, Direction d )
    {
        DECLARE_ENTRY_EXIT

        typed_memcpy( &fft_buffer_[0], in, n );
        fft_inner( in, &fft_buffer_[0], n, 1, d );
    }

    std::vector<CF> generateScalingFactors( size_t n, Direction d ) const
    {
        const double scaling{ d == Direction::FORWARD ? -1.0 : 1.0 };
        std::vector<CF> result(n);
        std::generate(
            std::begin(result),
            std::end(result),
            [i=0, n, d, scaling]() mutable {
                const double theta = (scaling * M_PI * i++)/n;
                return CF{ std::cos( theta ), std::sin( theta ) };
            } );

        return result;
    }
};
