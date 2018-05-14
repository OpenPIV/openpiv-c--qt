
#pragma once

// std
#include <cinttypes>
#include <tuple>

// to be tested
#include "Image.h"
#include "ImageUtils.h"

static std::tuple< UInt8Image, uint8_t > createAndFill( Size s, uint8_t v )
{
    UInt8Image result(s.width(), s.height());
    fill( result, v );

    return std::make_tuple( result, v );
}
