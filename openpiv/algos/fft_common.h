
#pragma once

// local
#include "core/enum_helper.h"

namespace openpiv::algos {

    enum class direction {
        FORWARD,
        REVERSE
    };

    DECLARE_ENUM_HELPER( direction, {
            { direction::FORWARD, "forward" },
            { direction::REVERSE, "reverse" }
        } )

}
