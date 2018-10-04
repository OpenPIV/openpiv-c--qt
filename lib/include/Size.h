
#pragma once

// std
#include <array>
#include <cstdint>
#include <iosfwd>
#include <tuple>

/// basic 2D integer size i.e. (width, height)
class Size
{
public:
    using type = uint32_t;

    Size() = default;
    Size( const Size& ) = default;
    Size( Size&& ) = default;
    constexpr Size( type w, type h )
    {
        data_[0] = w;
        data_[1] = h;
    }

    Size& operator=( const Size& ) = default;
    Size& operator=( Size&& ) = default;
    inline bool operator==(const Size& rhs) const { return data_ == rhs.data_; }
    inline bool operator!=(const Size& rhs) const { return !operator==(rhs); }

    constexpr inline type width() const { return data_[0]; }
    constexpr inline type height() const { return data_[1]; }
    constexpr inline type area() const { return data_[0] * data_[1]; }
    constexpr inline std::tuple<uint32_t, uint32_t> components() const { return std::make_tuple( data_[0], data_[1] ); }

    Size& operator+=( const Size& rhs );
    Size& operator-=( const Size& rhs );

private:
    constexpr inline type& width_() { return data_[0]; }
    constexpr inline type& height_() { return data_[1]; }

    std::array<type, 2> data_ {};
};

/// ostream operator
std::ostream& operator<<( std::ostream& os, const Size& r );

// basic arithmetic operators
Size operator+(const Size& lhs, const Size& rhs);
Size operator-(const Size& lhs, const Size& rhs);

/// find maximal dimension i.e. (1, 2) -> (2, 2)
Size maximal_size( const Size& s );

/// find minimal dimension i.e. (1, 2) -> (1, 1)
Size minimal_size( const Size& s );

/// transpose a size i.e. (1, 2) -> (2, 1)
Size transpose( const Size& s );

