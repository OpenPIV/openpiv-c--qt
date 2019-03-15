
#pragma once

// std
#include <exception>
#include <iterator>
#include <sstream>
#include <string>
#include <type_traits>

// local
#include "Image.h"
#include "Rect.h"
#include "Util.h"

/// Const-propagating wrapper for pointers without the restrictions of std::experimental::propagate_const.
/// It treats the wrapped pointer as a pointer to const when accessed through a const access path, hence the name.
template <typename T>
struct propagate_const
{
    using type = typename std::remove_pointer_t<T>;
    using type_ptr = typename std::add_pointer_t<type>;
    using type_ref = typename std::add_lvalue_reference_t<type>;
    type_ptr _ptr{ nullptr };

    propagate_const() = default;
    propagate_const(const propagate_const&) = default;
    propagate_const(propagate_const&&) = default;
    propagate_const& operator=(const propagate_const&) = default;
    propagate_const& operator=(propagate_const&&) = default;

    propagate_const(type_ptr ptr) : _ptr( ptr ) {}
    propagate_const(std::add_const<type_ptr> ptr) : _ptr( const_cast<type_ptr>(ptr) ) {}

    constexpr operator type_ptr() { return _ptr; }
    constexpr operator const type_ptr() const { return _ptr; }
    constexpr type_ptr operator->() { return _ptr; }
    constexpr const type_ptr operator->() const { return _ptr; }
    constexpr type_ref operator*() { return *_ptr; }
    constexpr const type_ref operator*() const { return *_ptr; }

    bool operator==( const propagate_const& rhs ) const { return _ptr == rhs._ptr; }
    bool operator!=( const propagate_const& rhs ) const { return !operator==(rhs); }
};


/// view onto an image; view must be less than or equal to the
/// size of the source image; aim is to provide a lightweight
/// respresentation  of a portion of an image with the same
/// interface as Image
template < typename T >
class ImageView
{
public:
    using type = T;
    using pixel_type = typename Image<T>::pixel_type;
    using index_type = typename Image<T>::index_type;
    using data_type = typename Image<T>::data_type;

    // ctor
    ImageView() = default;
    ImageView( const ImageView& ) = default;
    ImageView( ImageView&& ) = default;
    ImageView( Image<T>& im, const Rect& r )
        : im_(&im),
          r_( r )
    {
        // check that view makes sense
        Rect imageRect{ Rect::from_size( Size( im.width(), im.height())) };
        if ( !imageRect.contains( r ) )
            exception_builder<std::out_of_range>()
                << "image view (" << r << ") not contained within image (" << imageRect << ")";
    }

    ImageView( const Image<T>& im, const Rect& r )
        : ImageView( const_cast< Image<T>& >( im ), r )
    {}

    /// resize the image view; this may throw as the resized
    /// view would exceed the bounds of the source image; this
    /// will adjust the size of the viewed area but not the
    /// bottom-left coordinate
    void resize( uint32_t w, uint32_t h )
    {
        Rect newRect{ r_.bottomLeft(), Size{ w, h } };
        Rect imageRect{ Rect::from_size( Size(im_->width(), im_->height())) };
        if ( !imageRect.contains( newRect ) )
            exception_builder<std::out_of_range>()
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
            im_ == rhs.im_ &&
            r_ == rhs.r_;
    }
    inline bool operator!=(const ImageView& rhs) const { return !operator==(rhs); }

    inline T& operator[](size_t i)
    {
        if ( i > r_.area() )
            exception_builder<std::out_of_range>() << "index outside of allowed area: " << i << " > " << r_.area();

        const Point2<int32_t> bl{ r_.bottomLeft() };
        auto x = bl[0] + i % r_.width();
        auto y = bl[1] + i / r_.width();
        return im_->operator[]( {x, y} );
    }
    inline const T& operator[](size_t i) const { return const_cast<ImageView<T>*>(this)->operator[](i); }

    inline T& operator[]( const Point2<uint32_t>& xy )
    {
        return this->operator[](xy[1]*r_.width() + xy[0]);
    }
    inline const T& operator[]( const Point2<uint32_t>& xy ) const { return const_cast<ImageView<T>*>(this)->operator[](xy); }

    inline const T* line(size_t i) const { return im_->line(r_.bottomLeft()[1] + i) + r_.bottomLeft()[0]; }
    inline T* line(size_t i) { return im_->line(r_.bottomLeft()[1] + i) + r_.bottomLeft()[0]; }
    inline uint32_t width() const { return r_.width(); }
    inline uint32_t height() const { return r_.height(); }
    inline Size size() const { return r_.size(); }
    inline index_type pixel_count() const { return r_.area(); }
    inline Rect rect() const { return r_; }

    class iterator : public std::iterator< std::bidirectional_iterator_tag, pixel_type >
    {
    public:
        using IndexT = Rect::PointT::type;

        explicit iterator( ImageView iv, IndexT i = 0 ) : iv_( iv ), i_( i ) {}
        iterator& operator++() { ++i_; return *this; }
        iterator operator++(int) { iterator result = *this; operator++(); return result; }
        iterator& operator--() { --i_; return *this; }
        iterator operator--(int) { iterator result = *this; operator--(); return result; }
        bool operator==(const iterator& rhs) const { return iv_ == rhs.iv_ && i_ == rhs.i_; }
        bool operator!=(const iterator& rhs) const { return !operator==(rhs); }

        pixel_type& operator*()
        {
            if ( i_ < 0 || i_ >= checked_unsigned_conversion<IndexT>(iv_.pixel_count()) )
                exception_builder< std::out_of_range >()
                    << "attempting to dereference out of range iterator: "
                    << i_ << ", " << iv_.pixel_count();

            return iv_[ {i_ % iv_.width(), i_ / iv_.width()} ];
        }

    private:
        ImageView<T> iv_;
        IndexT i_ = {};
    };

    class const_iterator : public std::iterator< std::bidirectional_iterator_tag, pixel_type >
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

        pixel_type& operator*()
        {
            if ( i_ < 0 || i_ >= checked_unsigned_conversion<IndexT>(iv_.pixel_count()) )
                exception_builder< std::out_of_range >()
                    << "attempting to dereference out of range iterator: "
                    << i_ << ", " << iv_.pixel_count();

            return iv_[ {i_ % iv_.width(), i_ / iv_.width()} ];
        }

    private:
        ImageView<T> iv_;
        IndexT i_ = {};
    };

    /// iterators
    iterator begin() { return iterator( *this ); }
    iterator end() { return iterator( *this, pixel_count() ); }
    const_iterator begin() const { return const_iterator( *this ); }
    const_iterator end() const { return const_iterator( *this, pixel_count() ); }

    /// return the underlying image type - useful if trying to
    /// e.g. create an ImageView from another ImageView
    Image<T>& underlying() { return *im_; }
    const Image<T>& underlying() const { return *im_; }

private:
    propagate_const<Image<T>> im_;
    Rect r_;
};

template <typename T>
const ImageView<T> create_image_view( const Image<T>& im, Rect r )
{
    return { im, r };
}

template <typename T>
ImageView<T> create_image_view( Image<T>& im, Rect r )
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

/// standard image view types
using G8ImageView     = ImageView< G8 >;
using G16ImageView    = ImageView< G16 >;
using GFImageView     = ImageView< GF >;
using RGBA8ImageView  = ImageView< RGBA8 >;
using RGBA16ImageView = ImageView< RGBA16 >;
using CFImageView     = ImageView< CF >;
