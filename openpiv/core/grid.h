
#pragma once

// std
#include <array>
#include <vector>

// openpiv
#include "core/rect.h"
#include "core/size.h"

namespace openpiv::core {

    /// generate a centred cartesian grid of rectangles with
    /// dimensions \a size and a specified \a offset
    ///
    /// generate_cartesian_grid( images_size, size( 32, 32 ), 0.5 )
    ///
    /// image_size = (100, 50)
    ///
    /// top-left: [(2,  1), (18,  1), (34,  1), (50,  1), (66,  1),
    ///            (2, 17), (18, 17), (34, 17), (50, 17), (66, 17)]
    /// (66, 1) -> rectangle from (66, 1) -> (98, 33)
    /// (2, 17) -> rectangle from (2, 17) -> (34, 49)
    std::vector<core::rect>
    generate_cartesian_grid( const core::size& image_size,
                             const core::size& interrogation_size,
                             double percentage_offset );

    /// generate a centred cartesian grid of rectangles with
    /// dimensions \a size and a specified \a offset in x and y
    ///
    /// generate_cartesian_grid( images_size, size( 32, 32 ), { 16, 16 } )
    ///
    /// image_size = (100, 50)
    ///
    /// top-left: [(2,  1), (18,  1), (34,  1), (50,  1), (66,  1),
    ///            (2, 17), (18, 17), (34, 17), (50, 17), (66, 17)]
    /// (66, 1) -> rectangle from (66, 1) -> (98, 33)
    /// (2, 17) -> rectangle from (2, 17) -> (34, 49)
    std::vector<core::rect>
    generate_cartesian_grid( const core::size& image_size,
                             const core::size& interrogation_size,
                             std::array< uint32_t, 2 > offsets );

} // end of namespace


#include "core/detail/grid.impl.h"
