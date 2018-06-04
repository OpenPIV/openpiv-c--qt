
#pragma once

// std
#include <iosfwd>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>

// local
#include "Image.h"

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

    /// Load the image from \a stream
    /// \return true if success, false otherwise; may throw
    /// ImageLoaderException if there is a serious issue
    virtual void load( std::istream&, G16Image& ) const = 0;
    virtual void load( std::istream&, DoubleImage& ) const = 0;
    virtual void load( std::istream&, RGBA16Image& ) const = 0;

    /// return the name of this loader
    virtual std::string name() const = 0;

    /// return the priority of this loader
    virtual int priority() const = 0;

    /// find a loader for an image; if no ImageLoader is returned
    /// then this image is unhandled; the image loader returned is owned
    /// by ImageLoader
    static std::shared_ptr< ImageLoader > findLoader( std::istream& );

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
