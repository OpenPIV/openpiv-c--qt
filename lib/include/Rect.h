
#pragma once

// std
#include <array>
#include <cstdint>
#include <iostream>

// local
#include "Point.h"
#include "Size.h"
#include "Util.h"

/// basic 2D integer rectangle defined in terms of an origin (bottom right) and size
class Rect
{
public:
    using PointT = Point2<int32_t>;
    using SizeT = Size;

    Rect() = default;
    Rect( const Rect& ) = default;
    Rect( Rect&& ) = default;
    Rect( const PointT& bl, const Size& size )
        : bottomLeft_( bl ),
          size_( size )
    {}

    /// construct a rect from size with default origin
    static Rect from_size( const Size& s )
    {
        return Rect( PointT(), s );
    }

    Rect& operator=( const Rect& ) = default;
    Rect& operator=( Rect&& ) = default;
    constexpr inline bool operator==(const Rect& rhs) const
    {
        return
            bottomLeft_ == rhs.bottomLeft_ &&
            size_ == rhs.size_;
    }
    constexpr inline bool operator!=(const Rect& rhs) const { return !operator==(rhs); }

    constexpr inline PointT bottomLeft() const
    {
        return bottomLeft_;
    }
    constexpr inline PointT topLeft() const
    {
        return PointT(
            bottomLeft_[0],
            bottomLeft_[1]+checked_unsigned_conversion<decltype(bottomLeft_)::type>(size_.height()) );
    }
    constexpr inline PointT bottomRight() const
    {
        return PointT(
            bottomLeft_[0]+checked_unsigned_conversion<decltype(bottomLeft_)::type>(size_.width()),
            bottomLeft_[1] );
    }
    constexpr inline PointT topRight() const
    {
        return PointT(
            bottomLeft_[0]+checked_unsigned_conversion<decltype(bottomLeft_)::type>(size_.width()),
            bottomLeft_[1]+checked_unsigned_conversion<decltype(bottomLeft_)::type>(size_.height()) );
    }
    constexpr inline PointT midpoint() const
    {
        return { bottomLeft_[0] + width()/2, bottomLeft_[1] + height()/2 };
    }

    constexpr inline const Size& size() const { return size_; }
    constexpr inline Size::type width() const { return size_.width(); }
    constexpr inline Size::type height() const { return size_.height(); }
    constexpr inline Size::type area() const { return size_.area(); }

    /// is this rectangle wholly within \a r1
    constexpr inline const bool within( const Rect& r1 ) const
    {
        return
            bottomLeft()[0] >= r1.bottomLeft()[0] &&
            bottomLeft()[1] >= r1.bottomLeft()[1] &&
            topRight()[0] <= r1.topRight()[0] &&
            topRight()[1] <= r1.topRight()[1];
    }

    /// is \a r1 wholly contained within this rectangle
    constexpr inline const bool contains( const Rect& r1 ) const
    {
        return r1.within( *this );
    }

private:
    PointT bottomLeft_;
    Size size_;
};

/// ostream operator
inline std::ostream& operator<<( std::ostream& os, const Rect& r )
{
    os << r.bottomLeft() << " -> " << r.size();
    return os;
}
