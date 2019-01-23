
#pragma once

// std
#include <iosfwd>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>

// local
#include "Image.h"
#include "ImageView.h"

class ImageLoaderException : public std::runtime_error
{
public:
    ImageLoaderException( const std::string& what )
        : std::runtime_error( what )
    {}
};

/// interface for ImageLoaders; the std::istream passed should be at
/// the start of the available data i.e. start of file or received
/// data.
class ImageLoader
{
public:
    virtual ~ImageLoader() {}

    /// \return true if this loader can load the data fuond in \a stream
    virtual bool canLoad( std::istream& ) const = 0;

    /// \return true if this loader can save data
    virtual bool canSave() const = 0;

    /// Load the image from \a stream
    /// may throw ImageLoaderException if there is a serious issue
    virtual void load( std::istream&, G16Image& ) const = 0;
    virtual void load( std::istream&, GFImage& ) const = 0;
    virtual void load( std::istream&, RGBA16Image& ) const = 0;

    /// Save the image to \a stream
    /// may throw ImageLoaderException if there is a serious issue
    virtual void save( std::ostream&, const G16Image& ) const = 0;
    virtual void save( std::ostream&, const GFImage& ) const = 0;
    virtual void save( std::ostream&, const RGBA16Image& ) const = 0;
    virtual void save( std::ostream&, const G16ImageView& ) const = 0;
    virtual void save( std::ostream&, const GFImageView& ) const = 0;
    virtual void save( std::ostream&, const RGBA16ImageView& ) const = 0;

    /// return the name of this loader: MIME type
    /// \todo: change to have a list of supported
    /// MIME types
    virtual std::string name() const = 0;

    /// return the priority of this loader
    virtual int priority() const = 0;

    /// find a loader for an image; if no ImageLoader is returned
    /// then this image is unhandled; the image loader returned is owned
    /// by ImageLoader
    static std::shared_ptr< ImageLoader > find_loader( std::istream& );

    /// find a loader by name for saving/forced loading
    static std::shared_ptr< ImageLoader > find_loader( const std::string& );

    /// register a loader; done this way to avoid the provider knowing
    /// anything about the allocation/ownership of the loader.
    template < typename T >
    static
    typename std::enable_if< std::is_base_of< ImageLoader, T >::value, bool >::type
    registerLoader()
    {
        return registerLoader( std::shared_ptr<ImageLoader>( new T ) );
    }

private:
    static bool registerLoader( std::shared_ptr<ImageLoader> loader );
};
