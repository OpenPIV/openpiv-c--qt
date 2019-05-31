
#include "tiff_image_loader.h"

// std
#include <array>
#include <istream>
#include <memory>

// libtiff
#include <tiffio.h>
#include <tiffio.hxx>

// local
#include "util.h"
#include "image.h"

namespace openpiv::core {

static const bool registeredloader = image_loader::register_loader< tiff_image_loader >();

tiff_image_loader::~tiff_image_loader()
{}

bool tiff_image_loader::can_load( std::istream& is ) const
{
    if ( !is.good() )
        exception_builder<image_loader_exception>() << "input stream is not ready for reading";

    // record position
    peeker peek(is);

    // fetch header data
    std::array<char, 4> header;
    is.read(&header[0], sizeof(header));
    if ( !is.good() )
        exception_builder<image_loader_exception>() << "input stream doesn't contain enough data";

    static std::array<char, 4> LE_HEADER{ { 0x49, 0x49, 0x2a, 0x00 } };
    static std::array<char, 4> BE_HEADER{ { 0x4d, 0x4d, 0x00, 0x2a } };

    if ( header == LE_HEADER || header == BE_HEADER )
        return true;

    return false;
}

bool tiff_image_loader::can_save() const
{
    return false;
}

enum class planar_config : uint16_t {
    CONTIG   = 1,
    SEPARATE = 2
};

enum class sample_format : uint16_t {
    UINT          = 1,
    INT           = 2,
    IEEEFP        = 3,
    VOID          = 4,
    COMPLEXINT    = 5,
    COMPLEXIEEEFP = 6
};

template < typename PixelT, planar_config, sample_format, uint16_t SampleN, uint16_t BitsPerSample >
image<PixelT> copy(TIFF* tiff, uint32_t width, uint32_t height);

// 8-bit greyscale to 16-bit greyscale
template <>
g16_image copy<g_16, planar_config::CONTIG, sample_format::UINT, 1, 8>(TIFF* tiff, uint32_t width, uint32_t height)
{
    g16_image im(width, height, 0);

    // allocate line buffer
    size_t bytesPerLine = TIFFScanlineSize( tiff );
    std::vector<g_8> buf(bytesPerLine);

    // copy the data
    for (size_t h=0; h<height; ++h)
    {
        // Reading the data line by line; source and destination match so just memcpy
        TIFFReadScanline(tiff, &buf[0], h, 0);
        g_16* destp = im.line(h);
        g_8* srcp = &buf[0];
        for ( size_t w=0; w<width; ++w )
            *destp++ = *srcp++;
    }

    return im;
}

// 16-bit greyscale to 16-bit greyscale
template <>
g16_image copy<g_16, planar_config::CONTIG, sample_format::UINT, 1, 16>(TIFF* tiff, uint32_t width, uint32_t height)
{
    g16_image im(width, height, 0);

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
rgba16_image copy<rgba_16, planar_config::CONTIG, sample_format::UINT, 3, 16>(TIFF* tiff, uint32_t width, uint32_t height)
{
    rgba16_image im(width, height, 0);

    // allocate line buffer
    size_t bytesPerLine = TIFFScanlineSize( tiff );
    std::vector<g_16> buf(bytesPerLine >> 1);

    // copy the data
    for (size_t h=0; h<height; ++h)
    {
        TIFFReadScanline(tiff, &buf[0], h, 0);
        rgba_16* destp = im.line(h);
        g_16* srcp = &buf[0];
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
rgba16_image copy<rgba_16, planar_config::CONTIG, sample_format::UINT, 3, 8>(TIFF* tiff, uint32_t width, uint32_t height)
{
    rgba16_image im(width, height, 0);

    // allocate line buffer
    size_t bytesPerLine = TIFFScanlineSize( tiff );
    std::vector<g_8> buf(bytesPerLine);

    // copy the data
    for (size_t h=0; h<height; ++h)
    {
        TIFFReadScanline(tiff, &buf[0], h, 0);
        rgba_16* destp = im.line(h);
        g_8* srcp = &buf[0];
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
static void load_( std::istream& is, image<PixelT>& im )
{
    std::unique_ptr<TIFF, decltype(&TIFFClose)> tiff( TIFFStreamOpen( "sbTIFF", &is ), &TIFFClose );

    if ( !tiff )
        throw image_loader_exception( "failed to open io for reading" );

    // Reading the data from the tiff file's header
    uint32_t width;        TIFFGetField( tiff.get(), TIFFTAG_IMAGEWIDTH, &width );
    uint32_t height;       TIFFGetField( tiff.get(), TIFFTAG_IMAGELENGTH, &height );
    uint16_t bps;          TIFFGetField( tiff.get(), TIFFTAG_BITSPERSAMPLE, &bps );
    uint16_t spp;          TIFFGetField( tiff.get(), TIFFTAG_SAMPLESPERPIXEL, &spp );
    sample_format format;  TIFFGetField( tiff.get(), TIFFTAG_SAMPLEFORMAT, &format );
    planar_config planar;  TIFFGetField( tiff.get(), TIFFTAG_PLANARCONFIG, &planar );

    if ( spp != 1 && spp != 3 )
        exception_builder<image_loader_exception>() << "images with spp != 1 or 3 not yet supported: (" << spp << ")";

    if ( spp == 1 )
    {
        if ( bps == 8 )
            im = copy<g_16, planar_config::CONTIG, sample_format::UINT, 1, 8>(tiff.get(), width, height);
        else
            im = copy<g_16, planar_config::CONTIG, sample_format::UINT, 1, 16>(tiff.get(), width, height);
    }
    else
    {
        if ( bps == 8 )
            im = copy<rgba_16, planar_config::CONTIG, sample_format::UINT, 3, 8>(tiff.get(), width, height);
        else
            im = copy<rgba_16, planar_config::CONTIG, sample_format::UINT, 3, 16>(tiff.get(), width, height);
    }
}


void tiff_image_loader::load( std::istream& is, g16_image& im ) const
{
    load_(is, im);
}

void tiff_image_loader::load( std::istream& is, gf_image& im ) const
{
    load_(is, im);
}

void tiff_image_loader::load( std::istream& is, rgba16_image& im ) const
{
    load_(is, im);
}

void tiff_image_loader::save( std::ostream&, const g16_image& ) const
{
    exception_builder<image_loader_exception>() << name() << ": cannot save data";
}

void tiff_image_loader::save( std::ostream&, const g16_image_view& ) const
{
    exception_builder<image_loader_exception>() << name() << ": cannot save data";
}

void tiff_image_loader::save( std::ostream&, const gf_image& ) const
{
    exception_builder<image_loader_exception>() << name() << ": cannot save data";
}

void tiff_image_loader::save( std::ostream&, const gf_image_view& ) const
{
    exception_builder<image_loader_exception>() << name() << ": cannot save data";
}

void tiff_image_loader::save( std::ostream&, const rgba16_image& ) const
{
    exception_builder<image_loader_exception>() << name() << ": cannot save data";
}

void tiff_image_loader::save( std::ostream&, const rgba16_image_view& ) const
{
    exception_builder<image_loader_exception>() << name() << ": cannot save data";
}

std::string tiff_image_loader::name() const
{
    return "image/tiff";
}

int tiff_image_loader::priority() const
{
    return 1;
}

}
