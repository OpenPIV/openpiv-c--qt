
#pragma once

// std
#include <cinttypes>
#include <tuple>

// to be tested
#include "Image.h"
#include "ImageUtils.h"

static std::tuple< G8Image, G8 > createAndFill( Size s, G8 v )
{
    G8Image result(s.width(), s.height());
    fill( result, v );

    return std::make_tuple( result, v );
}

#define _ASSERT_DEATH( p, ExceptionT, s )                               \
    {                                                                   \
        bool caught{false};                                             \
        try {                                                           \
            p;                                                          \
        }                                                               \
        catch(ExceptionT& e)                                            \
        {                                                               \
            caught = std::string( e.what() ).find( s ) != std::string::npos; \
        }                                                               \
        ASSERT_TRUE(caught);                                            \
    }
