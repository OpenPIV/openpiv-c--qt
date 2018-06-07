
#include "PNMImageLoader.h"

// std
#include <array>
#include <istream>
#include <ostream>
#include <memory>

// local
#include "Image.h"
#include "Util.h"


template <int> uint16_t stobe( uint16_t v );
template <> inline uint16_t stobe<__ORDER_BIG_ENDIAN__>( uint16_t v )
{
    return v;
}

template <> inline uint16_t stobe<__ORDER_LITTLE_ENDIAN__>( uint16_t v )
{
    return (v>>8) | (v<<8);
}


static const bool registeredloader = ImageLoader::registerLoader< PNMImageLoader >();

PNMImageLoader::~PNMImageLoader()
{}

/// check if \a b contains "P4", "P5" or "P6"
bool isSupported( char* b )
{
    return
        b[0] == 'P' &&
        b[1] - '4' <= 2;
}

bool PNMImageLoader::canLoad( std::istream& is ) const
{
    if ( !is.good() )
        Thrower<ImageLoaderException>() << "input stream is not ready for reading";

    // record position
    Peeker peek(is);

    // fetch header data
    std::array<char, 2> header;
    is.read(&header[0], sizeof(header));
    if ( !is.good() )
        Thrower<ImageLoaderException>() << "input stream doesn't contain enough data";

    if ( isSupported(&header[0]) )
        return true;

    return false;
}

bool PNMImageLoader::canSave() const
{
    return true;
}

template < typename PixelT, uint16_t SampleN, uint16_t BitsPerSample >
Image<PixelT> copy(std::istream& is, uint32_t width, uint32_t height);

// 8-bit greyscale to 16-bit greyscale
// template <>
// G16Image copy<G16, 1, 8>(std::istream& is, uint32_t width, uint32_t height)
// {
//     G16Image im(width, height, 0);

//     // copy per line
//     std::vector<uint8_t> buffer(width);
//     uint32_t h = 0;
//     while ( !is.eof() && h<height )
//     {
//         is.read( &buffer[0], width );
//         uint8_t* src = &buffer[0];
//         G16* dest = im.line(h);
//         for ( uint32_t w=0; w<width; ++w )
//             *dest++ = *src++;

//         p += lineLength;
//         ++lineCount;
//     }


//     // allocate line buffer
//     size_t bytesPerLine = TIFFScanlineSize( tiff );
//     std::vector<G8> buf(bytesPerLine);

//     // copy the data
//     for (size_t h=0; h<height; ++h)
//     {
//         // Reading the data line by line; source and destination match so just memcpy
//         TIFFReadScanline(tiff, &buf[0], h, 0);
//         G16* destp = im.line(h);
//         G8* srcp = &buf[0];
//         for ( size_t w=0; w<width; ++w )
//             *destp++ = *srcp++;
//     }

//     return im;
// }

// template <typename PixelT>
// void load_( std::istream& is, Image<PixelT>& im )
// {
//     // PPM header is very simple:
//     // P6
//     // # CREATOR: GIMP PNM Filter Version 1.1
//     // 1920 1080
//     // 255
//     char buffer[ 1024 ];
//     is.getline( &buffer[0], sizeof( buffer ) );
//     if ( !isSupported(buffer) )
//         Thrower<ImageLoaderException>() << "image type not supported: " << std::string( buffer, 2 );

//     bool haveBitDepth{ false };
//     bool haveDimensions{ false };
//     uint32_t width{};
//     uint32_t height{};
//     uint16_t depth{};
//     while ( !haveBitDepth || !haveDimensions )
//     {
//         s.getline( &buffer[0], sizeof( buffer ) );

//         if ( buffer[0] == '#' )
//             continue;

//         if ( !haveDimensions && sscanf( &buffer[0], "%d %d", &width, &height ) == 2 )
//         {
//             haveDimensions = true;
//             continue;
//         }

//         if ( !haveBitDepth && sscanf( &buffer[0], "%d", &depth ) == 1 )
//         {
//             haveBitDepth = true;
//             continue;
//         }
//     }

//     return result;

// }

void PNMImageLoader::load( std::istream& is, G16Image& im ) const
{
    Thrower<ImageLoaderException>() << name() << ": loading data nyi";
}

void PNMImageLoader::load( std::istream& is, DoubleImage& im ) const
{
    Thrower<ImageLoaderException>() << name() << ": loading data nyi";
}

void PNMImageLoader::load( std::istream& is, RGBA16Image& im ) const
{
    Thrower<ImageLoaderException>() << name() << ": loading data nyi";
}

void PNMImageLoader::save( std::ostream& os, const G16Image& im ) const
{
    os << "P5\n"
       << "# created by PNMImageLoader\n"
       << im.width() << " " << im.height() << "\n"
       << "65535\n";

    const size_t bytesPerLine{ im.width() * sizeof(G16Image::PixelType) };

    std::vector<G16> buffer( im.width() );
    for ( uint32_t h=0; h<im.height(); ++h )
    {
        std::memcpy( &buffer[0], im.line(h), bytesPerLine );
        G16* p = &buffer[0];
        for ( uint32_t w=0; w<im.width(); ++w )
        {
            *p = stobe<__BYTE_ORDER__>( *p );
            ++p;
        }

        os.write( reinterpret_cast<const char*>( &buffer[0] ), bytesPerLine );
    }

    os.flush();
}

void PNMImageLoader::save( std::ostream& os, const DoubleImage& im ) const
{
    Thrower<ImageLoaderException>() << name() << ": cannot save DoubleImage";
}

void PNMImageLoader::save( std::ostream& os, const RGBA16Image& im ) const
{
    os << "P6\n"
       << "# created by PNMImageLoader\n"
       << im.width() << " " << im.height() << "\n"
       << "65535\n";

    // only write RGB data, not RGBA
    using SampleType = RGBA16Image::PixelType::Type;
    const size_t sampleCount = im.width() * 3;
    const size_t bytesPerLine{ sampleCount * sizeof(SampleType) };
    std::vector<SampleType> buffer( bytesPerLine );

    for ( uint32_t h=0; h<im.height(); ++h )
    {
        const RGBA16* src = im.line(h);
        SampleType* dest = &buffer[0];
        for ( uint32_t w=0; w<im.width(); ++w )
        {
            *dest++ = stobe<__BYTE_ORDER__>( src->r );
            *dest++ = stobe<__BYTE_ORDER__>( src->g );
            *dest++ = stobe<__BYTE_ORDER__>( src->b );
            ++src;
        }
        os.write( reinterpret_cast<const char*>( &buffer[0] ), buffer.size() );
    }

    os.flush();
}

std::string PNMImageLoader::name() const
{
    return "image/x-portable-anymap";
}

int PNMImageLoader::priority() const
{
    return 1;
}

