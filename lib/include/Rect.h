
#pragma once

// std
#include <array>
#include <cstdint>
#include <iostream>

// local
#include "Point.h"

/// basic 2D integer rectangle defined in terms of an origin (bottom right) and size
class Rect
{
public:
    Rect() = default;
    Rect( const Rect& ) = default;
    Rect( Rect&& ) = default;
    Rect( const Int2DPoint& bl, const Size& size )
        : bottomLeft_( bl ),
          size_( size )
    {}

    Rect& operator=( const Rect& ) = default;
    Rect& operator=( Rect&& ) = default;
    inline bool operator==(const Rect& rhs) const
    {
        return
            bottomLeft_ == rhs.bottomLeft_ &&
            size_ == rhs.size_;
    }
    inline bool operator!=(const Rect& rhs) const { return !operator==(rhs); }

    inline Int2DPoint bottomLeft() const
    {
        return bottomLeft_;
    }
    inline Int2DPoint topLeft() const
    {
        return Int2DPoint( bottomLeft_[0], bottomLeft_[1]+size_.height() );
    }
    inline Int2DPoint bottomRight() const
    {
        return Int2DPoint( bottomLeft_[0]+size_.width(), bottomLeft_[1] );
    }
    inline Int2DPoint topRight() const
    {
        return Int2DPoint( bottomLeft_[0]+size_.width(), bottomLeft_[1]+size_.height() );
    }

    inline const Size& size() const { return size_; }
    inline Size::type width() const { return size_.width(); }
    inline Size::type height() const { return size_.height(); }
    inline Size::type area() const { return size_.area(); }

    /// is this rectangle wholly within \a r1
    inline const bool within( const Rect& r1 ) const
    {
        return
            bottomLeft()[0] >= r1.bottomLeft()[0] &&
            bottomLeft()[1] >= r1.bottomLeft()[1] &&
            topRight()[0] <= r1.topRight()[0] &&
            topRight()[1] <= r1.topRight()[1];
    }

    /// is \a r1 wholly contained within this rectangle
    inline const bool contains( const Rect& r1 ) const
    {
        return r1.within( *this );
    }

private:
    Int2DPoint bottomLeft_;
    Size size_;
};

/// ostream operator
std::ostream& operator<<( std::ostream& os, const Rect& r )
{
    os << r.bottomLeft() << " -> " << r.size();
    return os;
}
