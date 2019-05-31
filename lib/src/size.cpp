
#include "size.h"

// std
#include <iostream>

// local
#include "exception_builder.h"
#include "util.h"

namespace openpiv::core {

size& size::operator+=( const size& rhs )
{
    width_() += rhs.width();
    height_() += rhs.height();
    return *this;
}

size& size::operator-=( const size& rhs )
{
    if ( rhs.width() > width() || rhs.height() > height() )
        core::exception_builder<std::out_of_range>()
            << "sizes cannot be negative: " << *this << " - " << rhs;

    width_() -= rhs.width();
    height_() -= rhs.height();
    return *this;
}

/// ostream operator
std::ostream& operator<<( std::ostream& os, const size& r )
{
    os << "[" << r.width() << "," << r.height() << "]";
    return os;
}

size operator+(const size& lhs, const size& rhs)
{
    return size(lhs)+=rhs;
}

size operator-(const size& lhs, const size& rhs)
{
    return size(lhs)-=rhs;
}

size maximal_size( const size& s )
{
    auto d{ std::max( s.width(), s.height() ) };
    return { d, d };
}

size minimal_size( const size& s )
{
    auto d{ std::min( s.width(), s.height() ) };
    return { d, d };
}

size transpose( const size& s )
{
    return { s.height(), s.width() };
}

}
