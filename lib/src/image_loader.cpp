
#include "loaders/image_loader.h"

// std
#include <algorithm>
#include <memory>
#include <vector>

// local
#include "core/util.h"

namespace openpiv::core {

    using  image_loader_container = std::vector< std::shared_ptr< image_loader > >;

    static image_loader_container& loaders()
    {
        static image_loader_container static_loaders;
        return static_loaders;
    }

    std::shared_ptr< image_loader > image_loader::find_loader( std::istream& s )
    {
        for ( auto& loader: loaders() )
            if ( loader->can_load( s ) )
                return loader;

        return std::shared_ptr< image_loader >();
    }

    std::shared_ptr< image_loader > image_loader::find_loader( const std::string& n )
    {
        for ( auto& loader: loaders() )
            if ( loader->name() == n )
                return loader;

        return std::shared_ptr< image_loader >();
    }

    bool image_loader::register_loader( std::shared_ptr<image_loader> loader )
    {
        if ( !loader )
            exception_builder<std::runtime_error>() << "attempting to register null image loader";

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

}
