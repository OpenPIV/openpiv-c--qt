
#pragma once

// std
#include <exception>
#include <iterator>
#include <sstream>
#include <string>

// local
#include "core/exception_builder.h"
#include "core/image.h"
#include "core/propagate_const.h"
#include "core/rect.h"
#include "core/util.h"

namespace openpiv::core {

    /// view onto an image; view must be less than or equal to the
    /// size of the source image; aim is to provide a lightweight
    /// respresentation  of a portion of an image with the same
    /// interface as Image
    template < typename T >
    class image_view
    {
    public:
        using value_t = T;
        using pixel_t = typename image<T>::pixel_t;
        using index_t = typename image<T>::index_t;
        using data_t = typename image<T>::data_t;

        // ctor
        image_view() = default;
        image_view( const image_view& ) = default;
        image_view( image_view&& ) = default;
        image_view( image<T>& im, const core::rect& r )
            : im_(&im),
              r_( r )
        {
            // check that view makes sense
            core::rect image_rect{ core::rect::from_size( {im.width(), im.height()} ) };
            if ( !image_rect.contains( r ) )
                core::exception_builder<std::out_of_range>()
                    << "image view (" << r << ") not contained within image (" << image_rect << ")";
        }

        image_view( const image<T>& im, const core::rect& r )
            : image_view( const_cast< image<T>& >( im ), r )
        {}

        /// resize the image view; this may throw as the resized
        /// view would exceed the bounds of the source image; this
        /// will adjust the size of the viewed area but not the
        /// bottom-left coordinate
        void resize( uint32_t w, uint32_t h )
        {
            core::rect new_rect{ r_.bottomLeft(), {w, h} };
            core::rect image_rect{ core::rect::from_size( {im_->width(), im_->height()} ) };
            if ( !image_rect.contains( new_rect ) )
                core::exception_builder<std::out_of_range>()
                    << "resize: image view (" << new_rect << ") not contained within image (" << image_rect << ")";

            r_ = new_rect;
        }

        // assignment
        image_view& operator=(const image_view&) = default;
        image_view& operator=(image_view&& rhs) = default;

        /// equality; this means we're looking at the same
        /// portion of the same image
        inline bool operator==(const image_view& rhs) const
        {
            return
                im_ == rhs.im_ &&
                r_ == rhs.r_;
        }
        inline bool operator!=(const image_view& rhs) const { return !operator==(rhs); }

        inline T& operator[](size_t i)
        {
            if ( i > r_.area() )
                core::exception_builder<std::out_of_range>() << "index outside of allowed area: " << i << " > " << r_.area();

            const point2<int32_t> bl{ r_.bottomLeft() };
            auto i_div_width = i / r_.width();
            auto x = bl[0] + (i - i_div_width * r_.width());
            auto y = bl[1] + i_div_width;
            return im_->operator[]( {x, y} );
        }
        inline const T& operator[](size_t i) const { return const_cast<image_view<T>*>(this)->operator[](i); }

        inline T& operator[]( const point2<uint32_t>& xy )
        {
            return this->operator[](xy[1]*r_.width() + xy[0]);
        }
        inline const T& operator[]( const point2<uint32_t>& xy ) const { return const_cast<image_view<T>*>(this)->operator[](xy); }

        inline const T* line(size_t i) const { return im_->line(r_.bottomLeft()[1] + i) + r_.bottomLeft()[0]; }
        inline T* line(size_t i) { return im_->line(r_.bottomLeft()[1] + i) + r_.bottomLeft()[0]; }
        inline uint32_t width() const { return r_.width(); }
        inline uint32_t height() const { return r_.height(); }
        inline core::size size() const { return r_.size(); }
        inline index_t pixel_count() const { return r_.area(); }
        inline core::rect rect() const { return r_; }

        class iterator : public std::iterator< std::bidirectional_iterator_tag, pixel_t >
        {
        public:
            explicit iterator( image_view& iv, index_t i = 0 ) : iv_( iv ), i_( i ) {}
            iterator& operator++() { ++i_; return *this; }
            iterator operator++(int) { iterator result = *this; operator++(); return result; }
            iterator& operator--() { --i_; return *this; }
            iterator operator--(int) { iterator result = *this; operator--(); return result; }
            bool operator==(const iterator& rhs) const { return iv_ == rhs.iv_ && i_ == rhs.i_; }
            bool operator!=(const iterator& rhs) const { return !operator==(rhs); }

            pixel_t& operator*()
            {
                if ( i_ < 0 || i_ >= iv_.pixel_count() )
                    core::exception_builder< std::out_of_range >()
                        << "attempting to dereference out of range iterator: "
                        << i_ << ", " << iv_.pixel_count();

                return iv_[ {i_ % iv_.width(), i_ / iv_.width()} ];
            }

        private:
            image_view& iv_;
            index_t i_ = {};
        };

        class const_iterator : public std::iterator< std::bidirectional_iterator_tag, pixel_t >
        {
        public:
            explicit const_iterator( const image_view& iv, index_t i = 0 ) : iv_( iv ), i_( i ) {}
            const_iterator& operator++() { ++i_; return *this; }
            const_iterator operator++(int) { const_iterator result = *this; operator++(); return result; }
            const_iterator& operator--() { --i_; return *this; }
            const_iterator operator--(int) { const_iterator result = *this; operator--(); return result; }
            bool operator==(const const_iterator& rhs) const { return iv_ == rhs.iv_ && i_ == rhs.i_; }
            bool operator!=(const const_iterator& rhs) const { return !operator==(rhs); }

            const pixel_t& operator*()
            {
                if ( i_ < 0 || i_ >= iv_.pixel_count() )
                    core::exception_builder< std::out_of_range >()
                        << "attempting to dereference out of range iterator: "
                        << i_ << ", " << iv_.pixel_count();

                return iv_[ {i_ % iv_.width(), i_ / iv_.width()} ];
            }

        private:
            const image_view& iv_;
            index_t i_ = {};
        };

        /// iterators
        iterator begin() { return iterator( *this ); }
        iterator end() { return iterator( *this, pixel_count() ); }
        const_iterator begin() const { return const_iterator( *this ); }
        const_iterator end() const { return const_iterator( *this, pixel_count() ); }

        /// return the underlying image type - useful if trying to
        /// e.g. create an image_view from another image_view
        image<T>& underlying() { return *im_; }
        const image<T>& underlying() const { return *im_; }

    private:
        propagate_const<image<T>> im_;
        core::rect r_;
    };

    // user-defined deduction guide
    template <typename T>
    image_view( image<T>&, const core::rect& ) -> image_view<T>;

    template <typename T>
    image_view( const image<T>&, const core::rect& ) -> image_view<T>;

    template <typename T>
    const image_view<T> create_image_view( const image<T>& im, core::rect r )
    {
        return { im, r };
    }

    template <typename T>
    image_view<T> create_image_view( image<T>& im, core::rect r )
    {
        return { im, r };
    }

    /// ostream operator
    template < typename T >
    std::ostream& operator<<( std::ostream& os, const image_view<T>& p )
    {
        os << "image_view<" << typeid(T).name() << ">[" << p.width() << ", " << p.height() << "]";

        return os;
    }

    /// standard image view types
    using g8_image_view     = image_view< g_8 >;
    using g16_image_view    = image_view< g_16 >;
    using gf_image_view     = image_view< g_f >;
    using rgba8_image_view  = image_view< rgba_8 >;
    using rgba16_image_view = image_view< rgba_16 >;
    using cf_image_view     = image_view< c_f >;

}
