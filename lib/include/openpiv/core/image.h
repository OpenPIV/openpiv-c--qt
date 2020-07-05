
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
#include "core/image_expression.h"
#include "core/image_type_traits.h"
#include "core/pixel_types.h"
#include "core/point.h"
#include "core/rect.h"
#include "core/size.h"
#include "core/util.h"
#include "core/exception_builder.h"

namespace openpiv::core {

/// basic 2-dimensional image; data is stored as a
/// contiguous array of type T
template < typename T >
class image
{
public:
    using type = T;
    using pixel_t = T;
    using index_t = size_t;
    using data_t = typename std::vector<T>;

    using iterator = typename data_t::iterator;
    using const_iterator = typename data_t::const_iterator;
    using reverse_iterator = typename data_t::reverse_iterator;
    using const_reverse_iterator = typename data_t::const_reverse_iterator;

    // ctor
    image()
        : width_( 0 )
        , height_( 0 )
        {}

    image( const image& ) = default;
    image( image&& ) = default;

    /// empty image
    image( uint32_t w, uint32_t h )
        : width_( w )
        , height_( h )
        , data_( w*h )
    {}
    image( const core::size& s )
        : width_( s.width() )
        , height_( s.height() )
        , data_( s.area() )
    {}

    /// image with default value
    image( uint32_t w, uint32_t h, T value )
        : width_( w )
        , height_( h )
        , data_( w*h, value )
    {}
    image( const core::size& s, T value )
        : width_( s.width() )
        , height_( s.height() )
        , data_( s.area(), value )
    {}

    /// conversion from another similar image; expensive!
    template < template<typename> class ImageT,
               typename ContainedT,
               typename = typename std::enable_if_t<
                   pixeltype_is_convertible_v< ContainedT, T > &&
                   is_imagetype_v< ImageT<ContainedT> > >
               >
    explicit image( const ImageT<ContainedT>& p )
        : width_( p.width() )
        , height_( p.height() )
        , data_( p.pixel_count() )
    {
        *this = p;
    }

    template <typename E,
              typename = typename std::enable_if_t< is_imageexpression_v<E> > >
    explicit image( const E& e )
    {
        *this = e;
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
    void resize( const core::size& s )
    {
        resize( s.width(), s.height() );
    }


    /// assignment
    image& operator=(const image& rhs)
    {
        width_ = rhs.width();
        height_ = rhs.height();
        data_ = rhs.data_;
        return *this;
    }

    /// move assignment
    image& operator=(image&& rhs)
    {
        data_   = std::move(rhs.data_);
        width_  = std::move(rhs.width_);
        height_ = std::move(rhs.height_);

        return *this;
    }

    /// conversion assignment
    template < template<typename> class ImageT,
               typename ContainedT,
               typename = typename std::enable_if_t<
                   pixeltype_is_convertible_v< ContainedT, T > &&
                   is_imagetype_v< ImageT<ContainedT> > >
               >
    image& operator=( const ImageT<ContainedT>& p )
    {
        resize( p.size() );

        // no alternative but to iterate
        for ( index_t i=0; i<p.pixel_count(); ++i )
            convert( p[i], data_[i] );

        return *this;
    }

    template <typename E,
              typename = typename std::enable_if_t< is_imageexpression_v<E> > >
    image& operator=(const E& e)
    {
        resize( e.size() );
        for ( index_t i=0; i<pixel_count(); ++i )
            data_[i] = e[i];

        return *this;
    }

    /// equality
    inline bool operator==(const image& rhs) const
    {
        return
            width_ == rhs.width_ &&
            height_ == rhs.height_ &&
            data_ == rhs.data_;
    }
    inline bool operator!=(const image& rhs) const { return !operator==(rhs); }

    /// pixel accessor
    inline T& operator[](size_t i) { return data_[i]; }
    inline const T& operator[](size_t i) const { return const_cast<image*>(this)->operator[](i); }

    /// pixel accessor by point
    inline T& operator[]( const point2<uint32_t>& xy ) { return data_[xy[1]*width_ + xy[0]]; }
    inline const T& operator[]( const point2<uint32_t>& xy ) const
    {
        return const_cast<image*>(this)->operator[](xy);
    }

    /// raw data accessor
    inline T* data() { return &data_[0]; }
    inline const T* data() const { return const_cast<image*>(this)->data(); }

    /// raw data by line
    inline T* line( size_t i )
    {
        if (i>height_)
            exception_builder<std::range_error>() << "line out of range (" << i << ", max is: " << height_ << ")";

        return &data_[i*width_];
    }
    inline const T* line( size_t i ) const { return const_cast<image*>(this)->line(i); }

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
    inline constexpr uint32_t width() const { return width_; }
    inline constexpr uint32_t height() const { return height_; }
    inline constexpr core::size size() const { return { width_, height_ }; }
    inline constexpr index_t pixel_count() const { return width_ * height_; }
    inline constexpr core::rect rect() const { return core::rect{ {}, { width_, height_ } }; }

    /// swap
    void swap( image& rhs )
    {
        std::swap( width_, rhs.width_ );
        std::swap( height_, rhs.height_ );
        std::swap( data_, rhs.data_ );
    }

private:
    uint32_t width_ = {};
    uint32_t height_ = {};
    data_t data_;
};


template <typename PixelT>
void swap( image<PixelT>& lhs, image<PixelT>& rhs )
{
    lhs.swap(rhs);
}

/// ostream operator
template < typename T >
std::ostream& operator<<( std::ostream& os, const image<T>& p )
{
    os << "image<" << typeid(T).name() << ">[" << p.width() << ", " << p.height() << "] "
       << "data @ " << (void*)p.data();

    return os;
}


/// standard image types
using g8_image     = image< g_8 >;
using g16_image    = image< g_16 >;
using gf_image     = image< g_f >;
using rgba8_image  = image< rgba_8 >;
using rgba16_image = image< rgba_16 >;
using cf_image     = image< c_f >;

}
