
#include "ImageLoader.h"

// std
#include <algorithm>
#include <memory>
#include <vector>

// local
#include "Util.h"

using  ImageLoaderContainer = std::vector< std::shared_ptr< ImageLoader > >;

static ImageLoaderContainer& loaders()
{
    static ImageLoaderContainer static_loaders;
    return static_loaders;
}

std::shared_ptr< ImageLoader > ImageLoader::find_loader( std::istream& s )
{
    for ( auto& loader: loaders() )
        if ( loader->canLoad( s ) )
            return loader;

    return std::shared_ptr< ImageLoader >();
}

std::shared_ptr< ImageLoader > ImageLoader::find_loader( const std::string& n )
{
    for ( auto& loader: loaders() )
        if ( loader->name() == n )
            return loader;

    return std::shared_ptr< ImageLoader >();
}

bool ImageLoader::registerLoader( std::shared_ptr<ImageLoader> loader )
{
    if ( !loader )
        Thrower<std::runtime_error>() << "attempting to register null image loader";

    loaders().push_back( loader );
    std::sort( loaders().begin(), loaders().end(),
               []( const auto& lhs, const auto& rhs ) -> bool
               {
                   return lhs->priority() < rhs->priority();
               }
        );

    std::cout << "registered " << loader->name() << "\n";
    return true;
}
