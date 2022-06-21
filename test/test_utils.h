
#pragma once

// std
#include <cinttypes>
#include <fstream>
#include <string>
#include <tuple>

// to be tested
#include "core/image.h"
#include "core/image_utils.h"
#include "loaders/image_loader.h"

using namespace openpiv::core;

template < typename ContainedT >
image< ContainedT > load_from_file( const std::string& filename )
{
    std::ifstream is(filename, std::ios::binary);
    if ( !is.is_open() )
        exception_builder<std::runtime_error>() << "failed to open " << filename;

    std::shared_ptr<image_loader> loader{ image_loader_registry::find(is) };
    if ( !loader )
        exception_builder<std::runtime_error>() << "failed to find loader for " << filename;

    image< ContainedT > result;
    loader->load( is, result );

    return result;
}

template < template<typename> class ImageT,
           typename ContainedT,
           typename E = typename std::enable_if_t<
               std::is_same_v< ImageT<ContainedT>, image<ContainedT> > ||
               std::is_same_v< ImageT<ContainedT>, image_view<ContainedT> > > >
bool save_to_file( const std::string& filename, const ImageT< ContainedT >& im )
{
    std::shared_ptr<image_loader> writer{ image_loader_registry::find("image/x-portable-anymap") };
    if ( !writer )
        exception_builder<std::runtime_error>() << "failed to find image writer";

    std::fstream os( filename, std::ios_base::trunc | std::ios_base::out );
    writer->save( os, im );

    return true;
}

template < typename T >
typename std::enable_if_t< is_pixeltype_v<T>, std::tuple< image<T>, T > >
create_and_fill( const size& s, T v )
{
    image<T> result( s );
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
            std::string what = e.what();                                \
            caught = matcher.match( what );                             \
        }                                                               \
        catch(...)                                                      \
        {}                                                              \
        REQUIRE(caught);                                                \
    }
