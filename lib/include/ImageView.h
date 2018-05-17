
#pragma once

// std
#include <exception>
#include <sstream>
#include <string>

// local
#include "ImageInterface.h"
#include "Image.h"
#include "Rect.h"
#include "Util.h"

/// const view onto an image; view must be less than or equal to the
/// size of the source image; aim is to provide a lightweight
/// respresentation  of a portion of an image with the same
/// interface as Image
template < typename T >
class ImageView : public ImageInterface< ImageView, T >
{
public:
    using type = T;
    using DataType = typename Image<T>::DataType;

    // ctor
    ImageView() = delete;
    ImageView( const ImageView& ) = default;
    ImageView( ImageView&& ) = default;
    ImageView( const Image<T>& im, const Rect& r )
        : im_(im),
          r_( r )
    {
        // check that view makes sense
        Rect imageRect{ Rect::fromSize( Size( im.width(), im.height())) };
        if ( !imageRect.contains( r ) )
            Thrower<std::out_of_range>()
                << "image view (" << r << ") not contained within image (" << imageRect << ")";
    }

    // assignment
    ImageView& operator=(const ImageView&) = default;
    ImageView& operator=(ImageView&& rhs) = default;

    /// equality; this means we're looking at the same
    /// portion of the same image
    inline bool operator==(const ImageView& rhs) const
    {
        return
            &im_ == &rhs.im_ &&
            r_ == rhs.r_;
    }
    inline bool operator!=(const ImageView& rhs) const { return !operator==(rhs); }

    inline const T& operator[](size_t i) const
    {
        if ( i > r_.area() )
            Thrower<std::out_of_range>() << "index outside of allowed area: " << i << " > " << r_.area();

        Int2DPoint bl{ r_.bottomLeft() };
        UInt2DPoint::type y = bl[1];
        while ( i > r_.width() )
        {
            ++y;
            i-= r_.width();
        }

        UInt2DPoint::type x = i + bl[0];
        return im_[ UInt2DPoint(x, y) ];
    }
    inline const T& operator[]( const UInt2DPoint& xy ) const
    {
        return this->operator[](xy[1]*r_.width() + xy[0]);
    }

    inline const T* data() const { return line(0); }
    inline const T* line(size_t i) const { return im_.line(r_.bottomLeft()[1] + i) + r_.bottomLeft()[0]; }
    inline const uint32_t width() const { return r_.width(); }
    inline const uint32_t height() const { return r_.height(); }
    inline const uint32_t pixel_count() const { return r_.area(); }

private:
    const Image<T>& im_;
    Rect r_;
};

template <typename T>
ImageView<T> createImageView( const Image<T>& im, Rect r)
{
    return { im, r };
}

/// ostream operator
template < typename T >
std::ostream& operator<<( std::ostream& os, const ImageView<T>& p )
{
    os << "ImageView<" << typeid(T).name() << ">[" << p.width() << ", " << p.height() << "]";

    return os;
}
