#pragma once

// std
#include <algorithm>
#include <exception>
#include <thread>
#include <unordered_map>
#include <vector>

// local
#include "Image.h"
#include "PixelTypes.h"
#include "Util.h"

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
    CFImage temp_;

    /// contains "twiddle factors" for each n: 2^n < N, n>=0
    std::unordered_map< size_t, std::vector<CF> > scaling_;

    /// track id of thread that created this instance
    std::thread::id thread_id_;

public:
    enum class Direction {
        FORWARD,
        REVERSE
    };

    FFT( const Size& size )
        : size_(size)
    {
        // ensure power-of-two sizes
        if ( !(is_pow2(size_.width()) && is_pow2(size_.height()) ) )
            Thrower<std::runtime_error>() << "dimensions must be power of 2: " << size_;

        // store thread id
        thread_id_ = std::this_thread::get_id();

        // allocate output, intermediate storage
        output_.resize( size );
        temp_.resize( transpose(size) );

        // make scaling factors
        size_t N{ maximal_size( size ).width() };
        do {
            scaling_[N] = generateScalingFactors(N);
            N >>= 1;
        } while (N > 2);
    }

    /// Perform a 2-D FFT; will always produce a complex floating point image output
    template < template <typename> class ImageT, typename ContainedT >
    const CFImage& transform( const ImageInterface< ImageT, ContainedT >& input, Direction d = Direction::FORWARD )
    {
        // sanity checks
        if ( std::this_thread::get_id() != thread_id_ )
            Thrower< std::runtime_error >() << "calling FFT::transform() from wrong thread";

        if ( input.size() != size_ )
            Thrower< std::runtime_error >() << "image size is different from expected: "
                                            << input.size() << ", " << size_;

        // copy data, converting to complex
        output_ = input;

        // iterate over rows first
        for ( uint32_t h = 0; h<output_.height(); ++h )
            fft( output_.line(h), temp_.line(h), output_.width(), d );

        // transpose
        transpose( temp_, output_ );

        // now do columns
        for ( uint32_t h = 0; h<temp_.height(); ++h )
            fft( output_.line(h), temp_.line(h), temp_.width(), d );

        // flip back
        transpose( temp_, output_ );

        // re-order
        swap_quadrants( output_ );

        return output_;
    }

    template < template <typename> class ImageT, typename ContainedT >
    CFImage cross_correlate( const ImageInterface< ImageT, ContainedT >& a, const ImageInterface< ImageT, ContainedT >& b )
    {
        CFImage a_fft{ transform( a, Direction::FORWARD ) };
        const CFImage& b_fft = transform( b, Direction::FORWARD );

        a_fft = a_fft * conj( b_fft );
        return transform( a_fft, Direction::REVERSE );
    }

    template < template <typename> class ImageT, typename ContainedT >
    CFImage auto_correlate( const ImageInterface< ImageT, ContainedT >& a )
    {
        CFImage a_fft{ transform( a, Direction::FORWARD ) };

        a_fft = a_fft * conj( a_fft );
        return transform( a_fft, Direction::REVERSE );
    }


private:
    void fft_inner( CF* in, CF* out, size_t n, size_t step, Direction d )
    {
        if (step < n)
        {
            const size_t doublestep{ 2*step };
            fft_inner(out,        in,        n, doublestep, d);
            fft_inner(out + step, in + step, n, doublestep, d);

            for ( size_t i=0; i<n; i+=doublestep )
            {
                // CF t = scaling_[n][i] * out[i + step];
                CF t = exp( CF{ 0, (d == Direction::FORWARD ? -1 : 1)*M_PI*i/n} ) * out[i + step];
                in[i / 2]     = out[i] + t;
                in[(i + n)/2] = out[i] - t;
            }
        }
    }

    void fft( CF* in, CF* out, size_t n, Direction d )
    {
        memcpy( out, in, n );
        fft_inner( in, out, n, 1, d );
    }

    std::vector<CF> generateScalingFactors( size_t N ) const
    {
        std::vector<CF> result(N);
        std::generate(
            std::begin(result),
            std::end(result),
            [i=0, N]() mutable { return exp( CF(0,-1*M_PI*(i++)/N) ); } );

        return result;
    }
};
