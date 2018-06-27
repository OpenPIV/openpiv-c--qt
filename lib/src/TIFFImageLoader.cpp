
#include "TIFFImageLoader.h"

// std
#include <array>
#include <istream>
#include <memory>

// libtiff
#include <tiffio.h>
#include <tiffio.hxx>

// local
#include "Util.h"
#include "Image.h"


static const bool registeredloader = ImageLoader::registerLoader< TIFFImageLoader >();

TIFFImageLoader::~TIFFImageLoader()
{}

bool TIFFImageLoader::canLoad( std::istream& is ) const
{
    if ( !is.good() )
        Thrower<ImageLoaderException>() << "input stream is not ready for reading";

    // record position
    Peeker peek(is);

    // fetch header data
    std::array<char, 4> header;
    is.read(&header[0], sizeof(header));
    if ( !is.good() )
        Thrower<ImageLoaderException>() << "input stream doesn't contain enough data";

    static std::array<char, 4> LE_HEADER{ { 0x49, 0x49, 0x2a, 0x00 } };
    static std::array<char, 4> BE_HEADER{ { 0x4d, 0x4d, 0x00, 0x2a } };

    if ( header == LE_HEADER || header == BE_HEADER )
        return true;

    return false;
}

bool TIFFImageLoader::canSave() const
{
    return false;
}

enum class PlanarConfig : uint16_t {
    CONTIG   = 1,
    SEPARATE = 2
};

enum class SampleFormat : uint16_t {
    UINT          = 1,
    INT           = 2,
    IEEEFP        = 3,
    VOID          = 4,
    COMPLEXINT    = 5,
    COMPLEXIEEEFP = 6
};

template < typename PixelT, PlanarConfig, SampleFormat, uint16_t SampleN, uint16_t BitsPerSample >
Image<PixelT> copy(TIFF* tiff, uint32_t width, uint32_t height);

// 8-bit greyscale to 16-bit greyscale
template <>
G16Image copy<G16, PlanarConfig::CONTIG, SampleFormat::UINT, 1, 8>(TIFF* tiff, uint32_t width, uint32_t height)
{
    G16Image im(width, height, 0);

    // allocate line buffer
    size_t bytesPerLine = TIFFScanlineSize( tiff );
    std::vector<G8> buf(bytesPerLine);

    // copy the data
    for (size_t h=0; h<height; ++h)
    {
        // Reading the data line by line; source and destination match so just memcpy
        TIFFReadScanline(tiff, &buf[0], h, 0);
        G16* destp = im.line(h);
        G8* srcp = &buf[0];
        for ( size_t w=0; w<width; ++w )
            *destp++ = *srcp++;
    }

    return im;
}

// 16-bit greyscale to 16-bit greyscale
template <>
G16Image copy<G16, PlanarConfig::CONTIG, SampleFormat::UINT, 1, 16>(TIFF* tiff, uint32_t width, uint32_t height)
{
    G16Image im(width, height, 0);

    // copy the data
    for (size_t h=0; h<height; ++h)
    {
        // Reading the data line by line; source and destination match so just memcpy
        TIFFReadScanline(tiff, im.line(h), h, 0);
    }

    return im;
}

// 16-bit RGB to 16-bit RGBA
template <>
RGBA16Image copy<RGBA16, PlanarConfig::CONTIG, SampleFormat::UINT, 3, 16>(TIFF* tiff, uint32_t width, uint32_t height)
{
    RGBA16Image im(width, height, 0);

    // allocate line buffer
    size_t bytesPerLine = TIFFScanlineSize( tiff );
    std::vector<G16> buf(bytesPerLine >> 1);

    // copy the data
    for (size_t h=0; h<height; ++h)
    {
        TIFFReadScanline(tiff, &buf[0], h, 0);
        RGBA16* destp = im.line(h);
        G16* srcp = &buf[0];
        for ( size_t w=0; w<width; ++w )
        {
            destp->r = *srcp++;
            destp->g = *srcp++;
            destp->b = *srcp++;
            ++destp;
        }
    }

    return im;
}

// 8-bit RGB to 16-bit RGBA
template <>
RGBA16Image copy<RGBA16, PlanarConfig::CONTIG, SampleFormat::UINT, 3, 8>(TIFF* tiff, uint32_t width, uint32_t height)
{
    RGBA16Image im(width, height, 0);

    // allocate line buffer
    size_t bytesPerLine = TIFFScanlineSize( tiff );
    std::vector<G8> buf(bytesPerLine);

    // copy the data
    for (size_t h=0; h<height; ++h)
    {
        TIFFReadScanline(tiff, &buf[0], h, 0);
        RGBA16* destp = im.line(h);
        G8* srcp = &buf[0];
        for ( size_t w=0; w<width; ++w )
        {
            destp->r = *srcp++;
            destp->g = *srcp++;
            destp->b = *srcp++;
            ++destp;
        }
    }

    return im;
}

template <typename PixelT>
static void load_( std::istream& is, Image<PixelT>& im )
{
    std::unique_ptr<TIFF, decltype(&TIFFClose)> tiff( TIFFStreamOpen( "sbTIFF", &is ), &TIFFClose );

    if ( !tiff )
        throw ImageLoaderException( "failed to open io for reading" );

    // Reading the data from the tiff file's header
    uint32_t width;       TIFFGetField( tiff.get(), TIFFTAG_IMAGEWIDTH, &width );
    uint32_t height;      TIFFGetField( tiff.get(), TIFFTAG_IMAGELENGTH, &height );
    uint16_t bps;         TIFFGetField( tiff.get(), TIFFTAG_BITSPERSAMPLE, &bps );
    uint16_t spp;         TIFFGetField( tiff.get(), TIFFTAG_SAMPLESPERPIXEL, &spp );
    SampleFormat format;  TIFFGetField( tiff.get(), TIFFTAG_SAMPLEFORMAT, &format );
    PlanarConfig planar;  TIFFGetField( tiff.get(), TIFFTAG_PLANARCONFIG, &planar );

    if ( spp != 1 && spp != 3 )
        Thrower<ImageLoaderException>() << "images with spp != 1 or 3 not yet supported: (" << spp << ")";

    if ( spp == 1 )
    {
        if ( bps == 8 )
            im = copy<G16, PlanarConfig::CONTIG, SampleFormat::UINT, 1, 8>(tiff.get(), width, height);
        else
            im = copy<G16, PlanarConfig::CONTIG, SampleFormat::UINT, 1, 16>(tiff.get(), width, height);
    }
    else
    {
        if ( bps == 8 )
            im = copy<RGBA16, PlanarConfig::CONTIG, SampleFormat::UINT, 3, 8>(tiff.get(), width, height);
        else
            im = copy<RGBA16, PlanarConfig::CONTIG, SampleFormat::UINT, 3, 16>(tiff.get(), width, height);
    }
}


void TIFFImageLoader::load( std::istream& is, G16Image& im ) const
{
    load_(is, im);
}

void TIFFImageLoader::load( std::istream& is, DoubleImage& im ) const
{
    load_(is, im);
}

void TIFFImageLoader::load( std::istream& is, RGBA16Image& im ) const
{
    load_(is, im);
}

void TIFFImageLoader::save( std::ostream&, const G16Image& ) const
{
    Thrower<ImageLoaderException>() << name() << ": cannot save data";
}

void TIFFImageLoader::save( std::ostream&, const DoubleImage& ) const
{
    Thrower<ImageLoaderException>() << name() << ": cannot save data";
}

void TIFFImageLoader::save( std::ostream&, const RGBA16Image& ) const
{
    Thrower<ImageLoaderException>() << name() << ": cannot save data";
}

std::string TIFFImageLoader::name() const
{
    return "image/tiff";
}

int TIFFImageLoader::priority() const
{
    return 1;
}

