
#pragma once

// std
#include <exception>
#include <iterator>
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
    using PixelType = typename Image<T>::PixelType;
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

    /// resize the image view; this may throw as the resized
    /// view would exceed the bounds of the source image; this
    /// will adjust the size of the viewed area but not the
    /// bottom-left coordinate
    void resize( uint32_t w, uint32_t h )
    {
        Rect newRect{ r_.bottomLeft(), Size{ w, h } };
        Rect imageRect{ Rect::fromSize( Size(im_.width(), im_.height())) };
        if ( !imageRect.contains( newRect ) )
            Thrower<std::out_of_range>()
                << "resize: image view (" << newRect << ") not contained within image (" << imageRect << ")";

        r_ = newRect;
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

        Point2<int32_t> bl{ r_.bottomLeft() };
        auto x = bl[0] + i % r_.width();
        auto y = bl[1] + i / r_.width();
        return im_[ Point2<uint32_t>(x, y) ];
    }
    inline const T& operator[]( const Point2<uint32_t>& xy ) const
    {
        return this->operator[](xy[1]*r_.width() + xy[0]);
    }

    inline const T* line(size_t i) const { return im_.line(r_.bottomLeft()[1] + i) + r_.bottomLeft()[0]; }
    inline const uint32_t width() const { return r_.width(); }
    inline const uint32_t height() const { return r_.height(); }
    inline const Size size() const { return r_.size(); }
    inline const uint32_t pixel_count() const { return r_.area(); }

    class const_iterator : public std::iterator< std::bidirectional_iterator_tag, PixelType >
    {
    public:
        using IndexT = Rect::PointT::type;

        explicit const_iterator( ImageView iv, IndexT i = 0 ) : iv_( iv ), i_( i ) {}
        const_iterator& operator++() { ++i_; return *this; }
        const_iterator operator++(int) { const_iterator result = *this; operator++(); return result; }
        const_iterator& operator--() { --i_; return *this; }
        const_iterator operator--(int) { const_iterator result = *this; operator--(); return result; }
        bool operator==(const const_iterator& rhs) const { return iv_ == rhs.iv_ && i_ == rhs.i_; }
        bool operator!=(const const_iterator& rhs) const { return !operator==(rhs); }
        const PixelType& operator*() const
        {
            if ( i_ < 0 || i_ >= iv_.pixel_count() )
                Thrower< std::out_of_range >()
                    << "attempting to dereference out of range iterator: "
                    << i_ << ", " << iv_.pixel_count();

            return iv_[ Point2<uint32_t>{i_ % iv_.width(), i_ / iv_.width()} ];
        }

    private:
        ImageView<T> iv_;
        IndexT i_ = {};
    };

    /// iterators
    const_iterator begin() const { return const_iterator( *this ); }
    const_iterator end() const { return const_iterator( *this, pixel_count() ); }

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
