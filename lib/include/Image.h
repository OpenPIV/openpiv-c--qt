
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
#include "ImageExpression.h"
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
    using type = T;
    using PixelType = T;
    using DataType = typename std::vector<T>;
    using iterator = typename DataType::iterator;
    using const_iterator = typename DataType::const_iterator;
    using reverse_iterator = typename DataType::reverse_iterator;
    using const_reverse_iterator = typename DataType::const_reverse_iterator;

    // ctor
    Image()
        : width_( 0 )
        , height_( 0 )
        {}

    Image( const Image& ) = default;
    Image( Image&& ) = default;

    /// empty image
    Image( uint32_t w, uint32_t h )
        : width_( w )
        , height_( h )
        , data_( w*h )
    {}
    Image( const Size& s )
        : width_( s.width() )
        , height_( s.height() )
        , data_( s.area() )
    {}

    /// image with default value
    Image( uint32_t w, uint32_t h, T value )
        : width_( w )
        , height_( h )
        , data_( w*h, value )
    {}
    Image( const Size& s, T value )
        : width_( s.width() )
        , height_( s.height() )
        , data_( s.area(), value )
    {}

    /// conversion from another similar image; expensive!
    template < template<typename> class ImageT,
               typename ContainedT,
               typename E = typename std::enable_if<pixeltype_is_convertible< ContainedT, T >::value>::type >
    explicit Image( const ImageInterface< ImageT, ContainedT >& p )
        : width_( p.width() )
        , height_( p.height() )
        , data_( p.pixel_count() )
    {
        // no alternative but to iterate
        for ( decltype(p.pixel_count()) i=0; i<p.pixel_count(); ++i )
            convert( p[i], this->operator[](i) );
    }

    template <typename E,
              typename = typename std::enable_if_t< is_imageexpression<E>::value > >
    Image( const E& e )
    {
        resize( e.size() );

        for ( decltype(pixel_count()) i=0; i<pixel_count(); ++i )
            operator[](i) = e[i];
    }

    /// resize the image; this is destructive and any data contained
    /// after a re-size should be considered invalid
    void resize( uint32_t w, uint32_t h )
    {
        if ( w == width_ && h == height_ )
            return;

        width_ = w;
        height_ = h;
        data_.resize( pixel_count() );
    }

    /// resize the image; this is destructive and any data contained
    /// after a re-size should be considered invalid
    void resize( const Size& s )
    {
        resize( s.width(), s.height() );
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
               typename E = typename std::enable_if<pixeltype_is_convertible< ContainedT, T >::value>::type >
    Image& operator=( const ImageInterface< ImageT, ContainedT >& p )
    {
        resize( p.size() );

        // no alternative but to iterate
        for ( decltype(p.pixel_count()) i=0; i<p.pixel_count(); ++i )
            convert( p[i], this->operator[](i) );

        return *this;
    }

    // import operator= from ImageInterface
    using ImageInterface< Image, T >::operator=;


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
    inline T& operator[](size_t i) { return data_[i]; }
    inline const T& operator[](size_t i) const { return const_cast<Image*>(this)->operator[](i); }

    /// pixel accessor by point
    inline T& operator[]( const Point2<uint32_t>& xy ) { return data_[xy[1]*width_ + xy[0]]; }
    inline const T& operator[]( const Point2<uint32_t>& xy ) const
    {
        return const_cast<Image*>(this)->operator[](xy);
    }

    /// raw data accessor
    inline T* data() { return &data_[0]; }
    inline const T* data() const { return const_cast<Image*>(this)->data(); }

    /// raw data by line
    inline T* line( size_t i )
    {
        if (i>height_)
            Thrower<std::range_error>() << "line out of range (" << i << ", max is: " << height_ << ")";

        return &data_[i*width_];
    }
    inline const T* line( size_t i ) const { return const_cast<Image*>(this)->line(i); }

    /// iterators
    iterator begin() { return std::begin( data_ ); }
    iterator end() { return std::end( data_ ); }
    const_iterator begin() const { return std::begin( data_ ); }
    const_iterator end() const { return std::end( data_ ); }
    reverse_iterator rbegin() { return std::rbegin( data_ ); }
    reverse_iterator rend() { return std::rend( data_ ); }
    const_reverse_iterator rbegin() const { return std::rbegin( data_ ); }
    const_reverse_iterator rend() const { return std::rend( data_ ); }

    /// geometry accessors
    inline const uint32_t width() const { return width_; }
    inline const uint32_t height() const { return height_; }
    inline const Size size() const { return Size( width_, height_ ); }
    inline const uint32_t pixel_count() const { return width_ * height_; }

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
    os << "Image<" << typeid(T).name() << ">[" << p.width() << ", " << p.height() << "] data @ " << (void*)p.data();

    return os;
}


/// standard image types
using G8Image     = Image< G8 >;
using G16Image    = Image< G16 >;
using GFImage     = Image< GF >;
using RGBA8Image  = Image< RGBA8 >;
using RGBA16Image = Image< RGBA16 >;
using CFImage     = Image< CF >;
