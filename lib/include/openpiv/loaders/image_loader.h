
#pragma once

// std
#include <iosfwd>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>

// local
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

    /// interface for ImageLoaders; the std::istream passed should be at
    /// the start of the available data i.e. start of file or received
    /// data.
    class image_loader
    {
    public:
        virtual ~image_loader() {}

        /// \return true if this loader can load the data fuond in \a stream
        virtual bool can_load( std::istream& ) const = 0;

        /// \return true if this loader can save data
        virtual bool can_save() const = 0;

        /// Load the image from \a stream
        /// may throw ImageLoaderException if there is a serious issue
        virtual void load( std::istream&, g16_image& ) const = 0;
        virtual void load( std::istream&, gf_image& ) const = 0;
        virtual void load( std::istream&, rgba16_image& ) const = 0;

        /// Save the image to \a stream
        /// may throw ImageLoaderException if there is a serious issue
        virtual void save( std::ostream&, const g16_image& ) const = 0;
        virtual void save( std::ostream&, const gf_image& ) const = 0;
        virtual void save( std::ostream&, const rgba16_image& ) const = 0;
        virtual void save( std::ostream&, const g16_image_view& ) const = 0;
        virtual void save( std::ostream&, const gf_image_view& ) const = 0;
        virtual void save( std::ostream&, const rgba16_image_view& ) const = 0;

        /// return the name of this loader: MIME type
        /// \todo: change to have a list of supported
        /// MIME types
        virtual std::string name() const = 0;

        /// return the priority of this loader
        virtual int priority() const = 0;

        /// find a loader for an image; if no ImageLoader is returned
        /// then this image is unhandled; the image loader returned is owned
        /// by ImageLoader
        static std::shared_ptr< image_loader > find_loader( std::istream& );

        /// find a loader by name for saving/forced loading
        static std::shared_ptr< image_loader > find_loader( const std::string& );

        /// register a loader; done this way to avoid the provider knowing
        /// anything about the allocation/ownership of the loader.
        template < typename T >
        static
        typename std::enable_if_t< std::is_base_of_v< image_loader, T >, bool >
        register_loader()
        {
            return register_loader( std::shared_ptr<image_loader>( new T ) );
        }

    private:
        static bool register_loader( std::shared_ptr<image_loader> loader );
    };

}
