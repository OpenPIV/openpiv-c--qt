
#pragma once

// std
#include <array>
#include <cstdint>
#include <iosfwd>

// local
#include "core/dll_export.h"

namespace openpiv::core {

/// basic 2D integer size i.e. (width, height)
class DLL_EXPORT size
{
public:
    using component_t = uint32_t;
    using components_t = std::array<component_t, 2>;

    size() = default;

    constexpr size( const size& rhs )
    {
        data_ = rhs.components();
    }

    constexpr size( size&& rhs )
    {
        data_ = std::move( rhs.data_ );
    }

    constexpr size( component_t w, component_t h )
    {
        data_[0] = w;
        data_[1] = h;
    }

    size& operator=( const size& rhs )
    {
        size s( rhs );
        *this = std::move( s );

        return *this;
    }

    size& operator=( size&& rhs )
    {
        data_ = std::move( rhs.data_ );

        return *this;
    }

    inline bool operator==(const size& rhs) const { return data_ == rhs.data_; }
    inline bool operator!=(const size& rhs) const { return !operator==(rhs); }

    constexpr inline component_t width() const { return width_(); }
    constexpr inline component_t height() const { return height_(); }
    constexpr inline component_t area() const { return width_() * height_(); }
    constexpr inline components_t components() const { return data_; }

    size& operator+=( const size& rhs );
    size& operator-=( const size& rhs );
    
private:
    constexpr inline const component_t& width_() const { return data_[0]; }
    constexpr inline const component_t& height_() const { return data_[1]; }
    constexpr inline component_t& width_() { return data_[0]; }
    constexpr inline component_t& height_() { return data_[1]; }

    components_t data_ {};
};

/// ostream operator
DLL_EXPORT std::ostream& operator<<( std::ostream& os, const size& r );

// basic arithmetic operators
DLL_EXPORT size operator+(const size& lhs, const size& rhs);
DLL_EXPORT size operator-(const size& lhs, const size& rhs);

/// find maximal dimension i.e. (1, 2) -> (2, 2)
DLL_EXPORT size maximal_size( const size& s );

/// find minimal dimension i.e. (1, 2) -> (1, 1)
DLL_EXPORT size minimal_size( const size& s );

/// transpose a size i.e. (1, 2) -> (2, 1)
DLL_EXPORT size transpose( const size& s );

}
