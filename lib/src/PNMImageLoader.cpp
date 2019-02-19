
#include "PNMImageLoader.h"

// std
#include <array>
#include <istream>
#include <ostream>
#include <memory>

// local
#include "Image.h"
#include "ImageStats.h"
#include "Util.h"


template <int> constexpr uint16_t stobe( uint16_t v );
template <> inline constexpr uint16_t stobe<__ORDER_BIG_ENDIAN__>( uint16_t v )
{
    return v;
}

template <> inline constexpr uint16_t stobe<__ORDER_LITTLE_ENDIAN__>( uint16_t v )
{
    return (v>>8) | (v<<8);
}

template <int E> inline constexpr uint16_t betos( uint16_t v )
{
    return stobe<E>(v);
}


static const bool registeredloader = ImageLoader::registerLoader< PNMImageLoader >();

PNMImageLoader::~PNMImageLoader()
{}

/// extract header info along with a flag to
/// indicate if the header is valid
std::tuple<bool, uint8_t> getHeaderInfo( char* b )
{
    bool result = b[0] == 'P';
    uint8_t type = b[1];
    result &= '5' <= type && type <= '6';

    return std::tuple<bool, uint8_t>{ result, type - '0' };
}

/// check if \a b contains "P5" or "P6"; ignore binary pbm images
bool isSupported( char* b )
{
    bool result;
    std::tie( result, std::ignore ) = getHeaderInfo( b );

    return result;
}

bool PNMImageLoader::canLoad( std::istream& is ) const
{
    if ( !is.good() )
        exception_builder<ImageLoaderException>() << "input stream is not ready for reading";

    // record position
    Peeker peek(is);

    // fetch header data
    std::array<char, 2> header;
    is.read(&header[0], sizeof(header));
    if ( !is.good() )
        exception_builder<ImageLoaderException>() << "input stream doesn't contain enough data";

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
template <>
G16Image copy<G16, 1, 8>(std::istream& is, uint32_t width, uint32_t height)
{
    G16Image im(width, height, 0);

    // copy per line
    std::vector<char> buffer(width);
    uint32_t h = 0;
    while ( !is.eof() && h<height )
    {
        is.read( &buffer[0], width );
        uint8_t* srcp = reinterpret_cast<uint8_t*>(&buffer[0]);
        G16* destp = im.line(h);
        for ( uint32_t w=0; w<width; ++w )
            *destp++ = *srcp++;

        ++h;
    }

    return im;
}

// 16-bit greyscale to 16-bit greyscale
template <>
G16Image copy<G16, 1, 16>(std::istream& is, uint32_t width, uint32_t height)
{
    G16Image im(width, height, 0);

    // copy per line
    const size_t bytesPerLine = width * sizeof(G16);
    std::vector<char> buffer(bytesPerLine);
    uint32_t h = 0;
    while ( !is.eof() && h<height )
    {
        is.read( &buffer[0], bytesPerLine );
        uint16_t* srcp = reinterpret_cast<uint16_t*>(&buffer[0]);
        G16* destp = im.line(h);
        for ( uint32_t w=0; w<width; ++w )
            *destp++ = betos<__BYTE_ORDER__>( *srcp++ );

        ++h;
    }


    return im;
}

// 8-bit RGB to 16-bit RGBA
template <>
RGBA16Image copy<RGBA16, 3, 8>(std::istream& is, uint32_t width, uint32_t height)
{
    RGBA16Image im(width, height, 0);

    // copy per line
    const size_t bytesPerLine = 3 * width;
    std::vector<char> buffer( bytesPerLine );
    uint32_t h = 0;
    while ( !is.eof() && h<height )
    {
        is.read( &buffer[0], bytesPerLine );
        RGBA16* destp = im.line(h);
        char* srcp = &buffer[0];
        for ( size_t w=0; w<width; ++w )
        {
            destp->r = *srcp++;
            destp->g = *srcp++;
            destp->b = *srcp++;
            ++destp;
        }

        ++h;
    }

    return im;
}

// 16-bit RGB to 16-bit RGBA
template <>
RGBA16Image copy<RGBA16, 3, 16>(std::istream& is, uint32_t width, uint32_t height)
{
    RGBA16Image im(width, height, 0);

    // copy per line
    const size_t bytesPerLine = 3 * width * sizeof(G16);
    std::vector<char> buffer( bytesPerLine );
    uint32_t h = 0;
    while ( !is.eof() && h<height )
    {
        is.read( &buffer[0], bytesPerLine );
        RGBA16* destp = im.line(h);
        uint16_t* srcp = reinterpret_cast<uint16_t*>(&buffer[0]);
        for ( size_t w=0; w<width; ++w )
        {
            destp->r = betos<__BYTE_ORDER__>(*srcp++);
            destp->g = betos<__BYTE_ORDER__>(*srcp++);
            destp->b = betos<__BYTE_ORDER__>(*srcp++);
            ++destp;
        }

        ++h;
    }

    return im;
}


// support chomping comment lines
struct Comment {};
Comment comment() { return {}; }
std::istream& operator>>( std::istream& is, const Comment& )
{
    if ( is.peek() == '#' )
        is.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );

    return is;
}

template <typename PixelT>
static void load_( std::istream& is, Image<PixelT>& im )
{
    // PPM header is very simple:
    // P6
    // # CREATOR: GIMP PNM Filter Version 1.1
    // 1920 1080
    // 255
    char buffer[ 1024 ];
    is.getline( &buffer[0], sizeof( buffer ) );

    bool isValid;
    uint8_t type;
    std::tie( isValid, type ) = getHeaderInfo( buffer );

    if ( !isValid )
        exception_builder<ImageLoaderException>() << "image type not supported: " << std::string( buffer, 2 );

    // get dimensions & depth
    uint32_t width{};
    uint32_t height{};
    uint16_t depth{};

    is >> comment() >> width >> comment() >> height >> comment() >> depth;
    // ensure we chomp any remaining whitespace to EOL
    is.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );

    std::cout << "PNM" << std::to_string(type) << "[" << width << ", " << height << "], " << depth << "\n";

    // get image data
    if ( type == 5 )
    {
        // binary greyscale
        if ( depth > 255 )
            im = copy<G16, 1, 16>( is, width, height );
        else
            im = copy<G16, 1, 8>( is, width, height );
    }
    else
    {
        // binary RGB
        if ( depth > 255 )
            im = copy<RGBA16, 3, 16>( is, width, height );
        else
            im = copy<RGBA16, 3, 8>( is, width, height );
    }
}

void PNMImageLoader::load( std::istream& is, G16Image& im ) const
{
    load_(is, im);
}

void PNMImageLoader::load( std::istream& is, GFImage& im ) const
{
    load_(is, im);
}

void PNMImageLoader::load( std::istream& is, RGBA16Image& im ) const
{
    load_(is, im);
}

template < template <typename> class ImageT >
void save_( std::ostream& os, const ImageInterface< ImageT, G16 >& im )
{
    os << "P5\n"
       << "# created by PNMImageLoader\n"
       << im.width() << " " << im.height() << "\n"
       << "65535\n";

    const size_t bytesPerLine{ im.width() * sizeof(G16Image::pixel_type) };

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

void PNMImageLoader::save( std::ostream& os, const G16Image& im ) const
{
    save_( os, im );
}

void PNMImageLoader::save( std::ostream& os, const G16ImageView& im ) const
{
    save_( os, im );
}

template < template <typename> class ImageT >
void save_( std::ostream& os, const ImageInterface< ImageT, GF >& im )
{
    auto [min, max] = find_image_range( im );
    auto range = max - min;
    if ( max == min )
    {
        // single valued image; range should be 1
        range = 1.0;
    }

    // scale the image and write as 16-bit file
    os << "P5\n"
       << "# created by PNMImageLoader\n"
       << im.width() << " " << im.height() << "\n"
       << "65535\n";

    const size_t bytesPerLine{ im.width() * sizeof(G16Image::pixel_type) };

    std::vector<G16> buffer( im.width() );
    for ( uint32_t h=0; h<im.height(); ++h )
    {
        G16* bp = &buffer[0];
        const GF* ip = im.line(h);
        for ( uint32_t w=0; w<im.width(); ++w )
            *bp++ = stobe<__BYTE_ORDER__>( static_cast< uint16_t >( G16::max() * ((*ip++ - min)/range) ) );

        os.write( reinterpret_cast<const char*>( &buffer[0] ), bytesPerLine );
    }

    os.flush();
}

void PNMImageLoader::save( std::ostream& os, const GFImage& im ) const
{
    save_( os, im );
}

void PNMImageLoader::save( std::ostream& os, const GFImageView& im ) const
{
    save_( os, im );
}


template < template <typename> class ImageT >
void save_( std::ostream& os, const ImageInterface< ImageT, RGBA16 >& im )
{
    os << "P6\n"
       << "# created by PNMImageLoader\n"
       << im.width() << " " << im.height() << "\n"
       << "65535\n";

    // only write RGB data, not RGBA
    using SampleType = RGBA16Image::pixel_type::Type;
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

void PNMImageLoader::save( std::ostream& os, const RGBA16Image& im ) const
{
    save_( os, im );
}

void PNMImageLoader::save( std::ostream& os, const RGBA16ImageView& im ) const
{
    save_( os, im );
}

std::string PNMImageLoader::name() const
{
    return "image/x-portable-anymap";
}

int PNMImageLoader::priority() const
{
    return 1;
}

