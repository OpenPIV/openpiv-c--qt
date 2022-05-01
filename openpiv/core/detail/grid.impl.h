
#pragma once

// openpiv
#include "core/util.h"

namespace openpiv::core {

    std::vector<core::rect>
    generate_cartesian_grid( const core::size& image_size,
                             const core::size& interrogation_size,
                             double percentage_offset )
    {
        if ( percentage_offset < 0.0 || percentage_offset > 1.0 )
            core::exception_builder<std::runtime_error>() << "offsets must be between 0.0 and 1.0";

        return generate_cartesian_grid(
            image_size,
            interrogation_size,
            { (uint32_t)(interrogation_size.width()  * percentage_offset),
              (uint32_t)(interrogation_size.height() * percentage_offset) } );
    }

    std::vector<core::rect>
    generate_cartesian_grid( const core::size& image_size,
                             const core::size& interrogation_size,
                             std::array<uint32_t, 2> offsets )
    {
        if ( image_size.area() == 0 )
            core::exception_builder<std::runtime_error>() << "image size must be non-zero";

        if ( interrogation_size.area() == 0 )
            core::exception_builder<std::runtime_error>() << "interrogation size must be non-zero";

        if ( offsets[0] == 0 || offsets[1] == 0 )
            core::exception_builder<std::runtime_error>() << "offsets must be non-zero";

        if ( interrogation_size.width() > image_size.width() ||
             interrogation_size.height() > image_size.height() )
            core::exception_builder<std::runtime_error>() << "interrogation size is bigger than image";

        auto [x_offset, y_offset] = offsets;

        auto x_count = 1 + (image_size.width()  - interrogation_size.width())  / x_offset;
        auto y_count = 1 + (image_size.height() - interrogation_size.height()) / y_offset;

        auto x_start_offset = (image_size.width()  - interrogation_size.width()  - (x_offset * (x_count-1)))/2;
        auto y_start_offset = (image_size.height() - interrogation_size.height() - (y_offset * (y_count-1)))/2;

        std::vector<core::rect> result;
        for ( size_t y=0; y < y_count; ++y )
        {
            for ( size_t x=0; x < x_count; ++x )
            {
                rect r{ {x_start_offset + x * x_offset, y_start_offset + y * y_offset}, interrogation_size };
                result.emplace_back( std::move(r) );
            }
        }

        return result;
    }

}
