
#include "Size.h"

// std
#include <iostream>

// local
#include "Util.h"


Size& Size::operator+=( const Size& rhs )
{
    width() += rhs.width();
    height() += rhs.height();
    return *this;
}

Size& Size::operator-=( const Size& rhs )
{
    if ( rhs.width() > width() || rhs.height() > height() )
        Thrower<std::out_of_range>() << "sizes cannot be negative: " << *this << " - " << rhs;

    width() -= rhs.width();
    height() -= rhs.height();
    return *this;
}

/// ostream operator
std::ostream& operator<<( std::ostream& os, const Size& r )
{
    os << "[" << r.width() << "," << r.height() << "]";
    return os;
}

Size operator+(const Size& lhs, const Size& rhs)
{
    return Size(lhs)+=rhs;
}

Size operator-(const Size& lhs, const Size& rhs)
{
    return Size(lhs)-=rhs;
}

Size maximal_size( const Size& s )
{
    auto d{ std::max( s.width(), s.height() ) };
    return { d, d };
}

Size minimal_size( const Size& s )
{
    auto d{ std::min( s.width(), s.height() ) };
    return { d, d };
}

Size transpose( const Size& s )
{
    return { s.height(), s.width() };
}
