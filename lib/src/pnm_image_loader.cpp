
#include "loaders/pnm_image_loader.h"

// std
#include <array>
#include <istream>
#include <ostream>
#include <memory>

// local
#include "core/image.h"
#include "core/image_type_traits.h"
#include "core/util.h"
#include "algos/stats.h"

namespace openpiv::core {

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


    static const bool registeredloader = image_loader::register_loader< pnm_image_loader >();

    pnm_image_loader::~pnm_image_loader()
    {}

    /// extract header info along with a flag to
    /// indicate if the header is valid
    std::tuple<bool, uint8_t> get_header_info( char* b )
    {
        bool result = b[0] == 'P';
        uint8_t type = b[1];
        result &= '5' <= type && type <= '6';

        return std::tuple<bool, uint8_t>{ result, type - '0' };
    }

    /// check if \a b contains "P5" or "P6"; ignore binary pbm images
    bool is_supported( char* b )
    {
        bool result;
        std::tie( result, std::ignore ) = get_header_info( b );

        return result;
    }

    bool pnm_image_loader::can_load( std::istream& is ) const
    {
        if ( !is.good() )
            exception_builder<image_loader_exception>() << "input stream is not ready for reading";

        // record position
        peeker peek(is);

        // fetch header data
        std::array<char, 2> header;
        is.read(&header[0], sizeof(header));
        if ( !is.good() )
            exception_builder<image_loader_exception>() << "input stream doesn't contain enough data";

        if ( is_supported(&header[0]) )
            return true;

        return false;
    }

    bool pnm_image_loader::can_save() const
    {
        return true;
    }

    template < typename PixelT, uint16_t SampleN, uint16_t BitsPerSample >
    image<PixelT> copy(std::istream& is, uint32_t width, uint32_t height);

    // 8-bit greyscale to 16-bit greyscale
    template <>
    g16_image copy<g_16, 1, 8>(std::istream& is, uint32_t width, uint32_t height)
    {
        g16_image im(width, height, 0);

        // copy per line
        std::vector<char> buffer(width);
        uint32_t h = 0;
        while ( !is.eof() && h<height )
        {
            is.read( &buffer[0], width );
            uint8_t* srcp = reinterpret_cast<uint8_t*>(&buffer[0]);
            g_16* destp = im.line(h);
            for ( uint32_t w=0; w<width; ++w )
                *destp++ = *srcp++;

            ++h;
        }

        return im;
    }

    // 16-bit greyscale to 16-bit greyscale
    template <>
    g16_image copy<g_16, 1, 16>(std::istream& is, uint32_t width, uint32_t height)
    {
        g16_image im(width, height, 0);

        // copy per line
        const size_t bytesPerLine = width * sizeof(g_16);
        std::vector<char> buffer(bytesPerLine);
        uint32_t h = 0;
        while ( !is.eof() && h<height )
        {
            is.read( &buffer[0], bytesPerLine );
            uint16_t* srcp = reinterpret_cast<uint16_t*>(&buffer[0]);
            g_16* destp = im.line(h);
            for ( uint32_t w=0; w<width; ++w )
                *destp++ = betos<__BYTE_ORDER__>( *srcp++ );

            ++h;
        }


        return im;
    }

    // 8-bit RGB to 16-bit RGBA
    template <>
    rgba16_image copy<rgba_16, 3, 8>(std::istream& is, uint32_t width, uint32_t height)
    {
        rgba16_image im(width, height, 0);

        // copy per line
        const size_t bytesPerLine = 3 * width;
        std::vector<char> buffer( bytesPerLine );
        uint32_t h = 0;
        while ( !is.eof() && h<height )
        {
            is.read( &buffer[0], bytesPerLine );
            rgba_16* destp = im.line(h);
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
    rgba16_image copy<rgba_16, 3, 16>(std::istream& is, uint32_t width, uint32_t height)
    {
        rgba16_image im(width, height, 0);

        // copy per line
        const size_t bytesPerLine = 3 * width * sizeof(g_16);
        std::vector<char> buffer( bytesPerLine );
        uint32_t h = 0;
        while ( !is.eof() && h<height )
        {
            is.read( &buffer[0], bytesPerLine );
            rgba_16* destp = im.line(h);
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
    struct comment_tag {};
    comment_tag comment() { return {}; }
    std::istream& operator>>( std::istream& is, const comment_tag& )
    {
        if ( is.peek() == '#' )
            is.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );

        return is;
    }

    template <typename PixelT>
    static void load_( std::istream& is, image<PixelT>& im )
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
        std::tie( isValid, type ) = get_header_info( buffer );

        if ( !isValid )
            exception_builder<image_loader_exception>() << "image type not supported: " << std::string( buffer, 2 );

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
                im = copy<g_16, 1, 16>( is, width, height );
            else
                im = copy<g_16, 1, 8>( is, width, height );
        }
        else
        {
            // binary RGB
            if ( depth > 255 )
                im = copy<rgba_16, 3, 16>( is, width, height );
            else
                im = copy<rgba_16, 3, 8>( is, width, height );
        }
    }

    void pnm_image_loader::load( std::istream& is, g16_image& im ) const
    {
        load_(is, im);
    }

    void pnm_image_loader::load( std::istream& is, gf_image& im ) const
    {
        load_(is, im);
    }

    void pnm_image_loader::load( std::istream& is, rgba16_image& im ) const
    {
        load_(is, im);
    }

    template < template <typename> class ImageT,
               typename = typename std::enable_if_t< is_imagetype<ImageT<g_16>>::value >
               >
    void save_( std::ostream& os, const ImageT<g_16>& im )
    {
        os << "P5\n"
           << "# created by pnm_image_loader\n"
           << im.width() << " " << im.height() << "\n"
           << "65535\n";

        const size_t bytesPerLine{ im.width() * sizeof(g16_image::pixel_t) };

        std::vector<g_16> buffer( im.width() );
        for ( uint32_t h=0; h<im.height(); ++h )
        {
            std::memcpy( &buffer[0], im.line(h), bytesPerLine );
            g_16* p = &buffer[0];
            for ( uint32_t w=0; w<im.width(); ++w )
            {
                *p = stobe<__BYTE_ORDER__>( *p );
                ++p;
            }

            os.write( reinterpret_cast<const char*>( &buffer[0] ), bytesPerLine );
        }

        os.flush();
    }

    void pnm_image_loader::save( std::ostream& os, const g16_image& im ) const
    {
        save_( os, im );
    }

    void pnm_image_loader::save( std::ostream& os, const g16_image_view& im ) const
    {
        save_( os, im );
    }

    template < template <typename> class ImageT,
               typename = typename std::enable_if_t< is_imagetype<ImageT<g_f>>::value >
               >
    void save_( std::ostream& os, const ImageT<g_f>& im )
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
           << "# created by pnm_image_loader\n"
           << im.width() << " " << im.height() << "\n"
           << "65535\n";

        const size_t bytesPerLine{ im.width() * sizeof(g16_image::pixel_t) };

        std::vector<g_16> buffer( im.width() );
        for ( uint32_t h=0; h<im.height(); ++h )
        {
            g_16* bp = &buffer[0];
            const g_f* ip = im.line(h);
            for ( uint32_t w=0; w<im.width(); ++w )
                *bp++ = stobe<__BYTE_ORDER__>( static_cast< uint16_t >( g_16::max() * ((*ip++ - min)/range) ) );

            os.write( reinterpret_cast<const char*>( &buffer[0] ), bytesPerLine );
        }

        os.flush();
    }

    void pnm_image_loader::save( std::ostream& os, const gf_image& im ) const
    {
        save_( os, im );
    }

    void pnm_image_loader::save( std::ostream& os, const gf_image_view& im ) const
    {
        save_( os, im );
    }


    template < template <typename> class ImageT,
               typename = typename std::enable_if_t< is_imagetype<ImageT<rgba_16>>::value >
               >
    void save_( std::ostream& os, const ImageT<rgba_16>& im )
    {
        os << "P6\n"
           << "# created by pnm_image_loader\n"
           << im.width() << " " << im.height() << "\n"
           << "65535\n";

        // only write RGB data, not RGBA
        using sample_type = rgba16_image::pixel_t::value_t;
        const size_t sampleCount = im.width() * 3;
        const size_t bytesPerLine{ sampleCount * sizeof(sample_type) };
        std::vector<sample_type> buffer( bytesPerLine );

        for ( uint32_t h=0; h<im.height(); ++h )
        {
            const rgba_16* src = im.line(h);
            sample_type* dest = &buffer[0];
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

    void pnm_image_loader::save( std::ostream& os, const rgba16_image& im ) const
    {
        save_( os, im );
    }

    void pnm_image_loader::save( std::ostream& os, const rgba16_image_view& im ) const
    {
        save_( os, im );
    }

    std::string pnm_image_loader::name() const
    {
        return "image/x-portable-anymap";
    }

    int pnm_image_loader::priority() const
    {
        return 1;
    }

}

