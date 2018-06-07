
#pragma once

// std
#include <tuple>

// local
#include "Image.h"

template < template<typename> class ImageT, typename ContainedT >
std::tuple< ContainedT, ContainedT >
findImageRange( const ImageInterface< ImageT, ContainedT >& im )
{
    ContainedT min, max;
    const ContainedT* p = im.data();
    const ContainedT* e = im.data() + im.pixel_count();
    min = max = *p++;
    while ( p < e )
    {
        min = *p < min ? *p : min;
        max = *p > max ? *p : max;
        ++p;
    }

    return std::make_tuple( min, max );
}
