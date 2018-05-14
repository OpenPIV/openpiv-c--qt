
#pragma once

// std
#include <cstdint>
#include <exception>
#include <iostream>
#include <typeinfo>
#include <vector>

// local
#include "ImageInterface.h"
#include "Size.h"
#include "Util.h"

/// basic 2-dimensional image; data is stored as a
/// contiguous array of type T
template < typename T >
class Image : public ImageInterface< Image, T >
{
public:
    using type = T;
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
    template < typename U,
               typename = typename std::enable_if<std::is_convertible<U, T>::value>::type >
    explicit Image( const Image< U >& p )
        : width_( p.width() )
        , height_( p.height() )
        , data_( p.data().size() )
    {
        // no alternative but to iterate
        typename DataType::iterator TI = std::begin( data_ );
        typename DataType::const_iterator SI = std::cbegin( p.data() );
        while ( TI != std::end(data_) )
            *TI = *SI;
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
        if (i<=height_)
            return &data_[i*width_];

        Thrower<std::out_of_range>() << "line out of range (" << i << ", max is: " << height_ << ")";
        return nullptr;
    }
    constexpr inline const T* line( size_t i ) const { return const_cast<Image*>(this)->line(i); }

    /// geometry accessors
    constexpr inline const uint32_t width() const { return width_; }
    constexpr inline const uint32_t height() const { return height_; }
    constexpr inline const uint32_t pixel_count() const { return width_ * height_; }

private:
    uint32_t width_;
    uint32_t height_;
    DataType data_;
};

/// ostream operator
template < typename T >
std::ostream& operator<<( std::ostream& os, const Image<T>& p )
{
    os << "Image<" << typeid(T).name() << ">[" << p.width() << ", " << p.height() << "]";

    return os;
}


/// standard image types
using UInt8Image  = Image< uint8_t >;
using UInt16Image = Image< uint16_t >;
using Int32Image  = Image< int32_t >;
using DoubleImage = Image< double >;
