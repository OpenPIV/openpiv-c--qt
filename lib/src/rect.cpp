
#include "core/rect.h"

// std
#include <iostream>

// local
#include "core/exception_builder.h"

namespace openpiv::core {

rect::rect( const point_t& bl, const core::size& size )
    : bottomLeft_( bl )
    , size_( size )
{}

/// construct a rect from size with default origin
rect rect::from_size( const core::size& s )
{
    return { point_t(), s };
}

bool rect::operator==(const rect& rhs) const
{
    return
        bottomLeft_ == rhs.bottomLeft_ &&
        size_ == rhs.size_;
}

bool rect::operator!=(const rect& rhs) const
{
    return !operator==(rhs);
}

rect::point_t rect::bottomLeft() const
{
    return bottomLeft_;
}

rect::point_t rect::topLeft() const
{
    return {
        bottomLeft_[0],
        bottomLeft_[1]+checked_unsigned_conversion<point_t::value_t>(size_.height()) };
}

rect::point_t rect::bottomRight() const
{
    return {
        bottomLeft_[0]+checked_unsigned_conversion<point_t::value_t>(size_.width()),
        bottomLeft_[1] };
}

rect::point_t rect::topRight() const
{
    return {
        bottomLeft_[0]+checked_unsigned_conversion<point_t::value_t>(size_.width()),
        bottomLeft_[1]+checked_unsigned_conversion<point_t::value_t>(size_.height()) };
}

rect::point_t rect::midpoint() const
{
    return { bottomLeft_[0] + width()/2, bottomLeft_[1] + height()/2 };
}

/// is this rectangle wholly within \a r1
bool rect::within( const rect& r1 ) const
{
    return
        bottomLeft()[0] >= r1.bottomLeft()[0] &&
        bottomLeft()[1] >= r1.bottomLeft()[1] &&
        topRight()[0] <= r1.topRight()[0] &&
        topRight()[1] <= r1.topRight()[1];
}

/// is \a r1 wholly contained within this rectangle
bool rect::contains( const rect& r1 ) const
{
    return r1.within( *this );
}

/// construct a dilated rectangle; positive values of d will grow
/// the rectangle, negative will shrink:
///
/// { {0, 0}, {10, 10} }.dilate(2) -> { {-2, -2}, {14, 14} }
rect rect::dilate( int32_t d ) const
{
    if ( -d >= width()/2 || -d >= height()/2 )
        core::exception_builder<std::runtime_error>()
            << "unable to dilate rect " << *this << " as dilation is too large";

    return {
        {bottomLeft_[0]-d, bottomLeft_[1]-d},
        {size_.width() + 2*d, size_.height() + 2*d}
    };
}

/// ostream operator
std::ostream& operator<<( std::ostream& os, const rect& r )
{
    os << r.bottomLeft() << " -> " << r.size();
    return os;
}

}
