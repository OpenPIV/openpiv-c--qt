
#pragma once

// std
#include <cstdint>
#include <exception>
#include <iostream>
#include <typeinfo>
#include <type_traits>
#include <utility>
#include <vector>

// local
#include "ImageInterface.h"
#include "PixelTypes.h"
#include "Size.h"
#include "Util.h"

/// basic 2-dimensional image; data is stored as a
/// contiguous array of type T
template < typename T >
class Image : public ImageInterface< Image, T >
{
public:
    using PixelType = T;
    using DataType = typename std::vector<T>;

    // ctor
    Image() = default;
    Image( const Image& ) = default;
    Image( Image&& ) = default;

    /// empty image
    Image( uint32_t w, uint32_t h )
        : width_( w )
        , height_( h )
        , data_( w*h )
    {}

    /// image with default value
    Image( uint32_t w, uint32_t h, T value )
        : width_( w )
        , height_( h )
        , data_( w*h, value )
    {}

    /// conversion from another similar image; hideously expensive!
    template < template<typename> class ImageT,
               typename ContainedT,
               typename E = typename std::enable_if<
                   std::is_convertible<ContainedT, T>::value>::type >
    explicit Image( const ImageInterface< ImageT, ContainedT >& p )
        : width_( p.width() )
        , height_( p.height() )
        , data_( p.pixel_count() )
    {
        // no alternative but to iterate
        for ( decltype(p.pixel_count()) i=0; i<p.pixel_count(); ++i )
            this->operator[](i) = p[i];
    }

    /// assignment
    Image& operator=(const Image& rhs)
    {
        width_ = rhs.width();
        height_ = rhs.height();
        data_ = rhs.data_;
        return *this;
    }

    /// move assignment
    Image& operator=(Image&& rhs)
    {
        data_   = std::move(rhs.data_);
        width_  = std::move(rhs.width_);
        height_ = std::move(rhs.height_);

        return *this;
    }

    /// conversion assignment
    template < template<typename> class ImageT,
               typename ContainedT,
               typename E = typename std::enable_if<
                   std::is_convertible<ContainedT, T>::value>::type >
    Image& operator=( const ImageT<ContainedT>& p )
    {
        Image im{ p };
        swap(im);

        return *this;
    }


    /// equality
    inline bool operator==(const Image& rhs) const
    {
        return
            width_ == rhs.width_ &&
            height_ == rhs.height_ &&
            data_ == rhs.data_;
    }
    inline bool operator!=(const Image& rhs) const { return !operator==(rhs); }

    /// pixel accessor
    constexpr inline T& operator[](size_t i) { return data_[i]; }
    constexpr inline const T& operator[](size_t i) const { return const_cast<Image*>(this)->operator[](i); }

    /// pixel accessor by point
    constexpr inline T& operator[]( const UInt2DPoint& xy ) { return data_[xy[1]*width_ + xy[0]]; }
    constexpr inline const T& operator[]( const UInt2DPoint& xy ) const
    {
        return const_cast<Image*>(this)->operator[](xy);
    }

    /// raw data accessor
    constexpr inline T* data() { return &data_[0]; }
    constexpr inline const T* data() const { return const_cast<Image*>(this)->data(); }

    /// raw data by line
    constexpr inline T* line( size_t i )
    {
        if (i>height_)
            Thrower<std::range_error>() << "line out of range (" << i << ", max is: " << height_ << ")";

        return &data_[i*width_];
    }
    constexpr inline const T* line( size_t i ) const { return const_cast<Image*>(this)->line(i); }

    /// geometry accessors
    constexpr inline const uint32_t width() const { return width_; }
    constexpr inline const uint32_t height() const { return height_; }
    constexpr inline const uint32_t pixel_count() const { return width_ * height_; }

    /// swap
    void swap( Image& rhs )
    {
        std::swap( width_, rhs.width_ );
        std::swap( height_, rhs.height_ );
        std::swap( data_, rhs.data_ );
    }

private:
    uint32_t width_;
    uint32_t height_;
    DataType data_;
};


template <typename PixelT>
void swap( Image<PixelT>& lhs, Image<PixelT>& rhs )
{
    lhs.swap(rhs);
}

/// ostream operator
template < typename T >
std::ostream& operator<<( std::ostream& os, const Image<T>& p )
{
    os << "Image<" << typeid(T).name() << ">[" << p.width() << ", " << p.height() << "]";

    return os;
}


/// standard image types
using G8Image  = Image< G8 >;
using G16Image = Image< G16 >;
using DoubleImage = Image< GF >;
using RGBA8Image = Image< RGBA8 >;
using RGBA16Image = Image< RGBA16 >;
