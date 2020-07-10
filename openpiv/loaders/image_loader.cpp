
#include "loaders/image_loader.h"

// std
#include <algorithm>
#include <memory>
#include <vector>

// local
#include "core/util.h"

namespace openpiv::core {

    using image_loader_container = std::vector<image_loader_ptr_t>;

    static image_loader_container& loaders()
    {
        static image_loader_container static_loaders;
        return static_loaders;
    }

    image_loader_ptr_t image_loader_registry::find( std::istream& s )
    {
        for ( auto& loader: loaders() )
            if ( loader->can_load( s ) )
                return loader->clone();

        return {};
    }

    image_loader_ptr_t image_loader_registry::find( const std::string& n )
    {
        for ( auto& loader: loaders() )
            if ( loader->name() == n )
                return loader->clone();

        return {};
    }

    bool image_loader_registry::add( image_loader_ptr_t&& loader )
    {
        if ( !loader )
            exception_builder<std::runtime_error>() << "attempting to register null image loader";

        auto name{ loader->name() };
        loaders().emplace_back( std::move(loader) );
        std::sort( loaders().begin(), loaders().end(),
                   []( const auto& lhs, const auto& rhs ) -> bool
                   {
                       return lhs->priority() < rhs->priority();
                   }
            );

        std::cout << "registered " << name << "\n";
        return true;
    }

}
