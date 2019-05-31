
#pragma once

// std
#include <tuple>

// local
#include "image.h"
#include "image_type_traits.h"

namespace openpiv::core {

template < template<typename> class ImageT,
           typename ContainedT,
           typename = typename std::enable_if_t< is_imagetype_v<ImageT<ContainedT>> >
           >
std::tuple< ContainedT, ContainedT >
find_image_range( const ImageT<ContainedT>& im )
{
    ContainedT min, max;
    auto p = std::cbegin( im );
    auto e = std::cend( im );
    min = max = *p++;
    while ( p != e )
    {
        min = *p < min ? *p : min;
        max = *p > max ? *p : max;
        ++p;
    }

    return std::make_tuple( min, max );
}

}
