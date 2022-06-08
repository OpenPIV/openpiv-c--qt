
#include "core/rect.h"

// std
#include <iostream>
#include <cmath>

// local
#include "core/exception_builder.h"
#include "core/util.h"

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
    if ( -d >= checked_unsigned_conversion<int32_t>( width()/2 ) ||
         -d >= checked_unsigned_conversion<int32_t>( height()/2 ) )
        core::exception_builder<std::runtime_error>()
            << "unable to dilate rect " << *this << " as dilation is too large";

    return {
        {bottomLeft_[0]-d, bottomLeft_[1]-d},
        {size_.width() + 2*d, size_.height() + 2*d}
    };
}

/// construct a dilated rectangle; positive values of d will grow
/// the rectangle, negative will shrink:
///
/// - value is an absolute percentage of the width, height
/// - 0 is the largest contraction
/// - 1 is a nullop
///
/// { {0, 0}, {10, 10} }.dilate(1.2) -> { {-1, -1}, {12, 12} }
rect rect::dilate( double d ) const
{
    if ( d < 0.0 )
        core::exception_builder<std::runtime_error>()
            << "unable to dilate rect " << *this << " as dilation is negative";

    const point_t::value_t w = width();
    const point_t::value_t h = height();
    const point_t::value_t rw = lround( d * w );
    const point_t::value_t rh = lround( d * h );
    const point_t::value_t dw = (w - rw)/2;
    const point_t::value_t dh = (h - rh)/2;

    return {
        {bottomLeft_[0] + dw, bottomLeft_[1] + dh},
        {static_cast<size::component_t>(rw), static_cast<size::component_t>(rh)}
    };
}

/// ostream operator
std::ostream& operator<<( std::ostream& os, const rect& r )
{
    os << r.bottomLeft() << " -> " << r.size();
    return os;
}

}
