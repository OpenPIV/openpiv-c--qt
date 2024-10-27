
#pragma once

// std
#include <functional>
#include <iosfwd>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>

// local
#include "core/dll_export.h"
#include "core/image.h"
#include "core/image_view.h"

namespace openpiv::core {

    class image_loader_exception : public std::runtime_error
    {
    public:
        image_loader_exception( const std::string& what )
            : std::runtime_error( what )
        {}
    };

    class DLL_EXPORT image_loader;
    using image_loader_ptr_t = std::unique_ptr<image_loader>;

    /// central repository for registered image loaders
    class DLL_EXPORT image_loader_registry
    {
    public:
        /// find a loader for an image; if no ImageLoader is returned
        /// then this image is unhandled; the image loader returned is owned
        /// by the caller
        static image_loader_ptr_t find( std::istream& );

        /// find a loader by name for saving/forced loading
        static image_loader_ptr_t find( const std::string& );

        /// register a loader; done this way to avoid the provider knowing
        /// anything about the allocation/ownership of the loader.
        template < typename T >
        static
        typename std::enable_if_t<
            std::is_base_of_v< image_loader, T >,
            bool >
        add()
        {
            return add( std::make_unique<T>() );
        }

    private:
        static bool add( image_loader_ptr_t&& );
    };

    /// interface for ImageLoaders; the std::istream passed should be at
    /// the start of the available data i.e. start of file or received
    /// data.
    ///
    /// image_loader may be stateful: if an image format supports
    /// multiple images then the loader may store e.g. dimensional
    /// information along with offsets to the contained images.
    ///
    /// A call to load() or open() must reset the state held within
    /// a loader, and
    class DLL_EXPORT image_loader
    {
    public:
        virtual ~image_loader() {}

        /// return the name of this loader: MIME type
        /// \todo: change to have a list of supported
        /// MIME types
        virtual std::string name() const = 0;

        /// return the priority of this loader
        virtual int priority() const = 0;

        /// polymorphic clone
        virtual image_loader_ptr_t clone() const = 0;

        /// \return true if this loader can load the data fuond in \a stream
        virtual bool can_load( std::istream& ) const = 0;

        /// \return true if this loader can save data
        virtual bool can_save() const = 0;

        /// \return number of images contained within this file
        virtual size_t num_images() const = 0;

        /// Open an image file for reading, usually reading
        /// any header data.
        /// \return true if \a is contains valid image data
        virtual bool open( std::istream& is ) = 0;

        /// Extract an image from an opened stream;
        /// may throw ImageLoaderException if there is an issue
        virtual bool extract( size_t index, g16_image& ) = 0;
        virtual bool extract( size_t index, gf_image& ) = 0;
        virtual bool extract( size_t index, rgba16_image& ) = 0;

        /// Load the image from \a stream; this is a convenience method
        /// that combines open/extract for the first image found
        /// may throw ImageLoaderException if there is an issue
        virtual bool load( std::istream& is, g16_image& im ) { return open(is) && extract(0, im); }
        virtual bool load( std::istream& is, gf_image& im ) { return open(is) && extract(0, im); }
        virtual bool load( std::istream& is, rgba16_image& im ) { return open(is) && extract(0, im); }

        /// Save the image to \a stream
        /// may throw ImageLoaderException if there is an issue
        virtual void save( std::ostream&, const g16_image& ) const = 0;
        virtual void save( std::ostream&, const gf_image& ) const = 0;
        virtual void save( std::ostream&, const rgba16_image& ) const = 0;
        virtual void save( std::ostream&, const g16_image_view& ) const = 0;
        virtual void save( std::ostream&, const gf_image_view& ) const = 0;
        virtual void save( std::ostream&, const rgba16_image_view& ) const = 0;
    };

}
