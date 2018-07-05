
#pragma once

// std
#include <array>
#include <cstdint>
#include <iosfwd>

/// basic 2D integer size
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
    constexpr inline type& width() { return data_[0]; }
    constexpr inline type height() const { return data_[1]; }
    constexpr inline type& height() { return data_[1]; }
    constexpr inline type area() const { return data_[0] * data_[1]; }

    Size& operator+=( const Size& rhs );
    Size& operator-=( const Size& rhs );

private:
    std::array<type, 2> data_ {};
};

/// ostream operator
std::ostream& operator<<( std::ostream& os, const Size& r );

Size operator+(const Size& lhs, const Size& rhs);
Size operator-(const Size& lhs, const Size& rhs);
