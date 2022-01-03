
#include "tiffimageloader.h"

// c++
#include <streambuf>
#include <iostream>

// qt
#include <QByteArray>
#include <QIODevice>
#include <QScopedArrayPointer>

// libtiff
#include <tiffio.h>
#include <tiffio.hxx>

// local
#include <imagedata.h>


static const bool registeredloader = ImageLoader::registerLoader< TIFFImageLoader >();

// helper wrapper around QIODevice to allow opening by libtiff
class QIODeviceStreamBuf : public std::streambuf
{
public:
    QIODeviceStreamBuf( QIODevice* iodevice )
        : iodevice_( iodevice )
    {
        if ( !iodevice )
            throw ImageLoaderException( "iodevice is NULL" );
    }
        
protected:
    int_type uflow()
    {
        if ( iodevice_->atEnd() )
            return traits_type::eof();

        char c;
        if ( iodevice_->peek( &c, 1 ) != 1 )
            throw ImageLoaderException( "failed to read from io" );
        
        return c;
    }
    
    int_type underflow()
    {
        if ( iodevice_->atEnd() )
            return traits_type::eof();

        char c;
        if ( iodevice_->read( &c, 1 ) != 1 )
            throw ImageLoaderException( "failed to read from io" );
        
        return c;
    }
    
    int_type pbackfail(int_type ch)
    {
        Q_UNUSED( ch );
        
        // don't support putting data back; this is a read-only
        // wrapper
        return traits_type::eof();
    }
    
    std::streamsize showmanyc()
    {
        return iodevice_->size();
    }

private:
    QIODevice* iodevice_;
};


TIFFImageLoader::~TIFFImageLoader()
{}

bool TIFFImageLoader::canLoad( QIODevice* iodevice ) const
{
    if ( !iodevice || !iodevice->isOpen() || !iodevice->bytesAvailable() )
        throw ImageLoaderException( "QIODevice is not ready for reading" );

    QByteArray header = iodevice->peek( 4 );
    if ( header.size() != 4 )
        throw ImageLoaderException( "QIODevice doesn't contain enough data" );

    static const char LE_HEADER[] = { 0x49, 0x49, 0x2b, 0x00 };
    static const char BE_HEADER[] = { 0x4d, 0x4d, 0x00, 0x2b };

    if ( header == QByteArray::fromRawData( LE_HEADER, sizeof( LE_HEADER ) ) ||
         header == QByteArray::fromRawData( BE_HEADER, sizeof( BE_HEADER ) ) )
        return true;

    return false;
}

template < uint16_t N >
struct CopyTrait
{};

template <>
struct CopyTrait< 8 >
{
    typedef uint8_t type;
};

template <>
struct CopyTrait< 16 >
{
    typedef uint16_t type;
};

template < uint16_t BPP >
void copyData( ImageData::PixelType* dst, void* src_, size_t col, size_t spp )
{
    typename CopyTrait< BPP >::type* src = (typename CopyTrait< BPP >::type*)src_;
    if ( spp == 1 )
    {
        *(dst + col) = *(src + col);
    }
    else if ( spp == 3 )
    {
        int r = *(src + col*spp + 0);
        int g = *(src + col*spp + 1);
        int b = *(src + col*spp + 2);
        *(dst + col) = qGray(r,g,b);
    }
}


QSharedPointer< ImageData > TIFFImageLoader::load( QIODevice* iodevice ) const
{
    QIODeviceStreamBuf streambuf( iodevice );
    std::istream istream( &streambuf );
    TIFF *tiff = TIFFStreamOpen( "sbTIFF", &istream );

    if ( !tiff )
        throw ImageLoaderException( "failed to open io for reading" );
    
    // Reading the data from the tiff file's header
    uint32_t height; TIFFGetField( tiff, TIFFTAG_IMAGELENGTH, &height );
    uint32_t width;  TIFFGetField( tiff, TIFFTAG_IMAGEWIDTH, &width );
    uint16_t bpp;    TIFFGetField( tiff, TIFFTAG_BITSPERSAMPLE, &bpp );
    uint16_t spp;    TIFFGetField( tiff, TIFFTAG_SAMPLESPERPIXEL, &spp );
    size_t bytesPerLine = TIFFScanlineSize( tiff );

    // create an image to hold the data
    QSharedPointer< ImageData > im( new ImageData( QSize( width, height ) ) );
    QScopedArrayPointer< char > buf( new char[ bytesPerLine ] );

    // find the correct conversion function
    auto fn = copyData< 8 >;
    if ( bpp == 16 )
        fn = copyData< 16 >;
    
    // copy the data
    for (size_t i=0; i<height; ++i)
    {
        // Reading the data line by line
        TIFFReadScanline(tiff, buf.data(), i, 0);
        for (size_t j=0; j<width; ++j)
        {
            fn( im->linebuffer( i ), buf.data(), j, spp );
        }
    }

    TIFFClose(tiff);

    return im;
}

QString TIFFImageLoader::name() const
{
    return "TIFF image loader";
}

int TIFFImageLoader::priority() const
{
    return 1;
}

