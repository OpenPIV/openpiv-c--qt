
#pragma once

// std
#include <cinttypes>
#include <tuple>

// to be tested
#include "Image.h"
#include "ImageLoader.h"
#include "ImageUtils.h"

template < typename ContainedT >
Image< ContainedT > loadFromFile( const std::string& filename )
{
    std::ifstream is(filename, std::ios::binary);
    if ( !is.is_open() )
        Thrower<std::runtime_error>() << "failed to open " << filename;

    std::shared_ptr<ImageLoader> loader{ ImageLoader::findLoader(is) };
    if ( !loader )
        Thrower<std::runtime_error>() << "failed to find loader for " << filename;

    Image< ContainedT > result;
    loader->load( is, result );

    return result;
}

template < template<typename> class ImageT,
           typename ContainedT,
           typename E = typename std::enable_if<
               std::is_same< ImageT<ContainedT>, Image<ContainedT> >::value ||
               std::is_same< ImageT<ContainedT>, ImageView<ContainedT> >::value >::type >
bool saveToFile( const std::string& filename, const ImageT< ContainedT >& im )
{
    std::shared_ptr<ImageLoader> writer{ ImageLoader::findLoader("image/x-portable-anymap") };
    if ( !writer )
        Thrower<std::runtime_error>() << "failed to find image writer";

    std::fstream os( filename, std::ios_base::trunc | std::ios_base::out );
    writer->save( os, im );

    return true;
}

static std::tuple< G8Image, G8 > createAndFill( Size s, G8 v )
{
    G8Image result(s.width(), s.height());
    fill( result, v );

    return std::make_tuple( result, v );
}

#define _REQUIRE_THROWS_MATCHES( p, ExceptionT, matcher )               \
    {                                                                   \
        bool caught{false};                                             \
        try {                                                           \
            p;                                                          \
        }                                                               \
        catch(ExceptionT& e)                                            \
        {                                                               \
            caught = matcher.match( std::string( e.what() ) );          \
        }                                                               \
        REQUIRE(caught);                                                \
    }
