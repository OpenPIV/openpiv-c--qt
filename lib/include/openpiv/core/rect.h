
#pragma once

// std
#include <iosfwd>

// local
#include "core/point.h"
#include "core/size.h"

namespace openpiv::core {

/// basic 2D integer rectangle defined in terms of an origin (bottom right) and size
class rect
{
public:
    using point_t = point2<int32_t>;

    rect() = default;
    rect( const rect& ) = default;
    rect( rect&& ) = default;
    rect( const point_t& bl, const core::size& size );

    /// construct a rect from size with default origin
    static rect from_size( const core::size& s );

    rect& operator=( const rect& ) = default;
    rect& operator=( rect&& ) = default;
    bool operator==(const rect& rhs) const;
    bool operator!=(const rect& rhs) const;

    point_t bottomLeft() const;
    point_t topLeft() const;
    point_t bottomRight() const;
    point_t topRight() const;
    point_t midpoint() const;

    inline const core::size& size() const { return size_; }
    inline core::size::component_t width() const { return size_.width(); }
    inline core::size::component_t height() const { return size_.height(); }
    inline core::size::component_t area() const { return size_.area(); }

    /// is this rectangle wholly within \a r1
    bool within( const rect& r1 ) const;

    /// is \a r1 wholly contained within this rectangle
    bool contains( const rect& r1 ) const;

    /// construct a dilated rectangle; positive values of d will grow
    /// the rectangle, negative will shrink:
    ///
    /// { {0, 0}, {10, 10} }.dilate(2) -> { {-2, -2}, {14, 14} }
    rect dilate( int32_t d ) const;

private:
    point_t bottomLeft_;
    core::size size_;
};

/// ostream operator
std::ostream& operator<<( std::ostream& os, const rect& r );

}
