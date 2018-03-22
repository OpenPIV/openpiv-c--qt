
#pragma once

// std
#include <stdexcept>

// qt
#include <QSharedPointer>

class QString;
class QIODevice;
class ImageData;

class ImageLoaderException : public std::runtime_error
{
public:
    ImageLoaderException( const std::string& what )
        : std::runtime_error( what )
    {}
};

/// interface for ImageLoaders; the QIODevice passed should be at
/// the start of the available data i.e. start of file or received
/// data. 
class ImageLoader
{
public:
    virtual ~ImageLoader() {}

    /// \return true if this loader can load the data fuond in \a iodevice
    virtual bool canLoad( QIODevice* iodevice ) const = 0;

    /// Load the image from \a iodevice
    /// \return true if success, false otherwise; may throw
    /// ImageLoaderException if there is a serious issue
    virtual QSharedPointer< ImageData > load( QIODevice* iodevice ) const = 0;

    /// return the name of this loader
    virtual QString name() const = 0;

    /// return the priority of this loader
    virtual int priority() const = 0;

    /// find a loader for an image; if no ImageLoader is returned
    /// then this image is unhandled; the image loader returned is owned
    /// by ImageLoader
    static QSharedPointer< ImageLoader > findLoader( QIODevice* );

    /// register a loader; done this way to avoid the provider knowing
    /// anything about the allocation/ownership of the loader.
    template < typename T >
    static bool registerLoader()
    { 
        return registerLoader( new T );
    }

private:
    static bool registerLoader( ImageLoader* loader );
};
