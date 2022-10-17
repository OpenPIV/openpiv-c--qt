
#include "loaders/tiff_image_loader.h"

// std
#include <array>
#include <istream>
#include <memory>

// libtiff
#include <tiffio.h>
#include <tiffio.hxx>

// local
#include "core/util.h"
#include "core/image.h"

namespace {

    using namespace openpiv::core;

    enum class planar_config : uint16_t {
        UNDEFINED = 0,
        CONTIG    = 1,
        SEPARATE  = 2
    };

    enum class sample_format : uint16_t {
        UNDEFINED     = 0,
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
    image_g_16 copy<g_16, planar_config::CONTIG, sample_format::UINT, 1, 8>(TIFF* tiff, uint32_t width, uint32_t height)
    {
        image_g_16 im(width, height, 0);

        // allocate line buffer
        size_t bytesPerLine = TIFFScanlineSize( tiff );
        std::vector<g_8> buf(bytesPerLine);

        // copy the data
        for (uint32_t h=0; h<height; ++h)
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
    image_g_16 copy<g_16, planar_config::CONTIG, sample_format::UINT, 1, 16>(TIFF* tiff, uint32_t width, uint32_t height)
    {
        image_g_16 im(width, height, 0);

        // copy the data
        for (uint32_t h=0; h<height; ++h)
        {
            // Reading the data line by line; source and destination match so just memcpy
            TIFFReadScanline(tiff, im.line(h), h, 0);
        }

        return im;
    }

    // 16-bit RGB to 16-bit RGBA
    template <>
    image_rgba_16 copy<rgba_16, planar_config::CONTIG, sample_format::UINT, 3, 16>(TIFF* tiff, uint32_t width, uint32_t height)
    {
        image_rgba_16 im(width, height, 0);

        // allocate line buffer
        size_t bytesPerLine = TIFFScanlineSize( tiff );
        std::vector<g_16> buf(bytesPerLine >> 1);

        // copy the data
        for (uint32_t h=0; h<height; ++h)
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
    image_rgba_16 copy<rgba_16, planar_config::CONTIG, sample_format::UINT, 3, 8>(TIFF* tiff, uint32_t width, uint32_t height)
    {
        image_rgba_16 im(width, height, 0);

        // allocate line buffer
        size_t bytesPerLine = TIFFScanlineSize( tiff );
        std::vector<g_8> buf(bytesPerLine);

        // copy the data
        for (uint32_t h=0; h<height; ++h)
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


    static const bool registeredloader = image_loader_registry::add< tiff_image_loader >();
}


namespace openpiv::core {

    struct tiff_image_loader::impl
    {
        std::istream& is;
        std::unique_ptr<TIFF, decltype(&TIFFClose)> tiff;
        uint32_t width = 0;
        uint32_t height = 0;
        uint16_t bps = 0;
        uint16_t spp = 0;
        sample_format format = sample_format::UNDEFINED;
        planar_config planar = planar_config::UNDEFINED;
        size_t num_images = 0;

        impl( std::istream& is_ )
            : is(is_)
            , tiff( nullptr, &TIFFClose )
        {}

        bool open()
        {
            tiff = std::unique_ptr<TIFF, decltype(&TIFFClose)>( TIFFStreamOpen( "sbTIFF", &is ), &TIFFClose );

            if ( !tiff )
            {
                // throw image_loader_exception( "failed to open io for reading" );
                // \todo: add logging
                std::cerr << "failed to open TIFF for reading\n";
                return false;
            }

            TIFF* tiff_ = tiff.get();

            // Reading the data from the tiff file's header; N.B. this is the
            // first image's dimensions
            TIFFGetField( tiff_, TIFFTAG_IMAGEWIDTH,      &width );
            TIFFGetField( tiff_, TIFFTAG_IMAGELENGTH,     &height );
            TIFFGetField( tiff_, TIFFTAG_BITSPERSAMPLE,   &bps );
            TIFFGetField( tiff_, TIFFTAG_SAMPLESPERPIXEL, &spp );
            TIFFGetField( tiff_, TIFFTAG_SAMPLEFORMAT,    &format );
            TIFFGetField( tiff_, TIFFTAG_PLANARCONFIG,    &planar );

            // find number of images
            do {
                ++num_images;
            } while (TIFFReadDirectory(tiff_));

            // ... and reset current directory
            TIFFSetDirectory( tiff_, 0 );

            return true;
        }

        template <typename PixelT>
        bool extract( size_t index, image<PixelT>& im ) const
        {
            if ( !tiff )
                return false;

            if ( spp != 1 && spp != 3 )
            {
                // exception_builder<image_loader_exception>() << "images with spp != 1 or 3 not yet supported: (" << spp << ")";
                std::cerr << "images with spp != 1 or 3 not yet supported: (" << spp << ")" << "\n";
                return false;
            }

            if ( num_images <= index )
            {
                std::cerr << "image index out of range (index: " << index << ", num_images: " << num_images << "\n";
                return false;
            }

            // get correct image
            if ( TIFFSetDirectory( tiff.get(), (uint16_t)index ) == 0 )
            {
                std::cerr << "failed to set directory to: " << index << "\n";
                return false;
            }

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

            return true;
        }
    };

    //
    //
    //
    tiff_image_loader::tiff_image_loader()
    {}

    tiff_image_loader::~tiff_image_loader()
    {}

    std::string tiff_image_loader::name() const
    {
        return "image/tiff";
    }

    int tiff_image_loader::priority() const
    {
        return 1;
    }

    image_loader_ptr_t tiff_image_loader::clone() const
    {
        return std::make_unique<tiff_image_loader>();
    }

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

        static const std::array<char, 4> LE_HEADER{ { 0x49, 0x49, 0x2a, 0x00 } };
        static const std::array<char, 4> BE_HEADER{ { 0x4d, 0x4d, 0x00, 0x2a } };

        if ( header == LE_HEADER || header == BE_HEADER )
            return true;

        return false;
    }

    bool tiff_image_loader::can_save() const
    {
        return false;
    }

    size_t tiff_image_loader::num_images() const
    {
        if ( !impl_ || !impl_->tiff )
            return 0;

        return impl_->num_images;
    }

    bool tiff_image_loader::open( std::istream& is )
    {
        impl_ = std::make_unique<impl>(is);
        return impl_->open();
    }

    bool tiff_image_loader::extract( size_t index, image_g_16& im )
    {
        return impl_->extract( index, im );
    }

    bool tiff_image_loader::extract( size_t index, image_g_f& im )
    {
        return impl_->extract( index, im );
    }

    bool tiff_image_loader::extract( size_t index, image_rgba_16& im )
    {
        return impl_->extract( index, im );
    }

    void tiff_image_loader::save( std::ostream&, const image_g_16& ) const
    {
        exception_builder<image_loader_exception>() << name() << ": cannot save data";
    }

    void tiff_image_loader::save( std::ostream&, const image_g_16_view& ) const
    {
        exception_builder<image_loader_exception>() << name() << ": cannot save data";
    }

    void tiff_image_loader::save( std::ostream&, const image_g_f& ) const
    {
        exception_builder<image_loader_exception>() << name() << ": cannot save data";
    }

    void tiff_image_loader::save( std::ostream&, const image_g_f_view& ) const
    {
        exception_builder<image_loader_exception>() << name() << ": cannot save data";
    }

    void tiff_image_loader::save( std::ostream&, const image_rgba_16& ) const
    {
        exception_builder<image_loader_exception>() << name() << ": cannot save data";
    }

    void tiff_image_loader::save( std::ostream&, const image_rgba_16_view& ) const
    {
        exception_builder<image_loader_exception>() << name() << ": cannot save data";
    }

}
