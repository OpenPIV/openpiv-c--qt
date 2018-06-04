
#include "Size.h"

// std
#include <iostream>

/// ostream operator
std::ostream& operator<<( std::ostream& os, const Size& r )
{
    os << "[" << r.width() << "," << r.height() << "]";
    return os;
}
