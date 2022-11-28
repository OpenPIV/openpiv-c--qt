
#pragma once

// std
#include <exception>
#include <iterator>
#include <sstream>
#include <string>

// local
#include "core/exception_builder.h"
#include "core/image.h"
#include "core/log.h"
#include "core/propagate_const.h"
#include "core/vector.h"
#include "core/rect.h"
#include "core/util.h"

namespace logger = openpiv::core::logger;

namespace openpiv::core {

    /// view onto an image; view must be less than or equal to the
    /// size of the source image; aim is to provide a lightweight
    /// respresentation  of a portion of an image with the same
    /// interface as Image.
    ///
    /// Creation of an image_view is done relative to the parent
    /// image, and pixel access of the image_view is in local
    /// coordinates
    ///
    /// image and image_view both know their location - this is
    /// given by the position of the image or image_view rect().
    /// For an image_view, this position is in global coordinates
    /// i.e. for an image located at (10, 10) with size (50, 50)
    /// and an image_view created at (5, 5) in the image, the
    /// global location of the image_view is (15, 15)
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

            const rect::point_t iv_location{ i % r_.width(), i / r_.width() };
            return im_->operator[]({
                    r_.left() + iv_location[0],
                    r_.bottom() + iv_location[1]
                });
        }
        inline const T& operator[](size_t i) const { return const_cast<image_view<T>*>(this)->operator[](i); }

        inline T& operator[]( const point2<uint32_t>& xy )
        {
            return this->operator[](xy[1]*r_.width() + xy[0]);
        }
        inline const T& operator[]( const point2<uint32_t>& xy ) const { return const_cast<image_view<T>*>(this)->operator[](xy); }

        inline const T* line(size_t i) const { return im_->line(r_.bottom() + i) + r_.left(); }
        inline T* line(size_t i) { return im_->line(r_.bottom() + i) + r_.left(); }
        inline uint32_t width() const { return r_.width(); }
        inline uint32_t height() const { return r_.height(); }
        inline core::size size() const { return r_.size(); }
        inline index_t pixel_count() const { return r_.area(); }
        inline core::rect rect() const
        {
            const auto& im_rect = im_->rect();
            return {
                {im_rect.left() + r_.left(), im_rect.bottom() + r_.bottom()},
                r_.size()
            };
        }

        class iterator
        {
        public:
            using iterator_category = std::bidirectional_iterator_tag;
            using value_type = pixel_t;
            using difference_type = typename image<T>::data_t::difference_type;
            using pointer = pixel_t*;
            using reference = pixel_t&;

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

        class const_iterator
        {
        public:
            using iterator_category = std::bidirectional_iterator_tag;
            using value_type = pixel_t;
            using difference_type = typename image<T>::data_t::difference_type;
            using pointer = pixel_t*;
            using reference = pixel_t&;

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

        std::tuple<size_t, size_t> stride() const
        {
            return im_->stride();
        }

        /// return the underlying image type - useful if trying to
        /// e.g. create an image_view from another image_view
        image<T>& underlying() { return *im_; }
        const image<T>& underlying() const { return *im_; }

    private:
        image_view( image<T>& im, const core::rect& r )
            : im_(&im)
            , r_(r)
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

        image_view( image_view<T>& view, const core::rect& r )
            : im_(&view.underlying())
        {
            // check that r fits within existing image_view rect
            core::rect image_view_rect{ core::rect::from_size( {view.width(), view.height()} ) };
            if ( !image_view_rect.contains( r ) )
                core::exception_builder<std::out_of_range>()
                    << "image_view (" << r << ") not contained within image_view (" << image_view_rect << ")";

            // shift the origin
            r_ = { { view.rect().left() + r.left(), view.rect().bottom() + r.bottom() }, r.size() };
        }

        image_view( const image_view<T>& view, const core::rect& r )
            : image_view( const_cast< image_view<T>& >( view ), r )
        {}

        // create friends to allow access to constructors
        template <template<typename> class ImageT,
                  typename U>
        friend image_view<U> create_image_view( ImageT<U>& im, core::rect r );

        template <template<typename> class ImageT,
                  typename U>
        friend const image_view<U> create_image_view( const ImageT<U>& im, core::rect r );

        propagate_const<image<T>> im_;  ///< pointer to image
        core::rect r_;                  ///< rect relative to im_
    };

    // user-defined deduction guide
    template <typename T>
    image_view( image<T>&, const core::rect& ) -> image_view<T>;

    template <typename T>
    image_view( const image<T>&, const core::rect& ) -> image_view<T>;

    template <typename T>
    image_view( image_view<T>&, const core::rect& ) -> image_view<T>;

    template <typename T>
    image_view( const image_view<T>&, const core::rect& ) -> image_view<T>;

    // create an image_view; required to ensure const is correctly handled
    template <template<typename> class ImageT,
              typename T>
    const image_view<T> create_image_view( const ImageT<T>& im, core::rect r )
    {
        return { im, r };
    }

    template <template<typename> class ImageT,
              typename T>
    image_view<T> create_image_view( ImageT<T>& im, core::rect r )
    {
        return { im, r };
    }

    /// ostream operator
    template < typename T >
    std::ostream& operator<<( std::ostream& os, const image_view<T>& p )
    {
        os << "image_view<" << pixeltype_name<T>() << ">[" << p.rect() << "][" << (void*)&p.underlying() << "] "
           << "data @ " << (void*)p.underlying().data();

        return os;
    }

    /// standard image view types
    using image_g_8_view     = image_view< g_8 >;
    using image_g_16_view    = image_view< g_16 >;
    using image_g_f_view     = image_view< g_f >;
    using image_rgba_8_view  = image_view< rgba_8 >;
    using image_rgba_16_view = image_view< rgba_16 >;
    using image_c_f_view     = image_view< c_f >;

}
