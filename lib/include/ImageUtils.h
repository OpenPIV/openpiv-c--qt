
#pragma once

// local
#include "Image.h"

/// fill an image with a constant value
template < typename T >
Image< T >& fill( Image< T >& im, T value )
{
    Image< T > filled( im.width(), im.height(), value );
    im = std::move( filled );

    return im;
}
