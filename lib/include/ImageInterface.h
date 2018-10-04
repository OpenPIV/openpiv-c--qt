
#pragma once

// std
#include <iosfwd>
#include <type_traits>

// local
#include "ImageExpression.h"
#include "Point.h"
#include "Size.h"

/// basic 2-dimensional image interface; designed to
/// be inherited using CRTP
template < template<typename> class ImageT, typename ContainedT >
class ImageInterface
{
public:
    using ContainedType = ContainedT;
    using DerivedType = ImageT<ContainedT>;

    inline const DerivedType* derived() const { return static_cast<const DerivedType*>( this ); }
    inline DerivedType* derived() { return static_cast<DerivedType*>( this ); }

    inline void resize( uint32_t w, uint32_t h )
    {
        derived()->resize( w, h );
    }

    inline void resize( const Size& s )
    {
        derived()->resize( s.width(), s.height() );
    }

    /// equality
    inline bool operator==(const ImageInterface& rhs) const
    {
        return derived()->operator==(rhs);
    }
    inline bool operator!=(const ImageInterface& rhs) const
    {
        return derived()->operator!=(rhs);
    }

    /// pixel accessor
    constexpr inline ContainedT& operator[](size_t i)
    {
        return derived()->operator[](i);
    }
    constexpr inline const ContainedT& operator[](size_t i) const
    {
        return derived()->operator[](i);
    }

    /// pixel accessor by point
    constexpr inline ContainedT& operator[]( const Point2<uint32_t>& xy )
    {
        return derived()->operator[](xy);
    }
    constexpr inline const ContainedT& operator[]( const Point2<uint32_t>& xy ) const
    {
        return derived()->operator[](xy);
    }

    /// raw data by line
    constexpr inline ContainedT* line( size_t i )
    {
        return derived()->line( i );
    }
    constexpr inline const ContainedT* line( size_t i ) const
    {
        return derived()->line( i );
    }

    /// geometry accessors
    inline const uint32_t width() const { return derived()->width(); }
    inline const uint32_t height() const { return derived()->height(); }
    inline const Size size() const { return derived()->size(); }
    inline const uint32_t pixel_count() const { return derived()->pixel_count(); }

    /// apply an operation to all pixels
    template <typename Op>
    DerivedType& apply( Op op )
    {
        for ( decltype(pixel_count()) i=0; i<pixel_count(); ++i )
            operator[](i) = op(i, operator[](i));

        return *derived();
    }

    template <typename E>
    typename std::enable_if< is_imageexpression<E>::value, DerivedType& >::type
    operator=(const E& e)
    {
        resize( e.size() );

        for ( decltype(pixel_count()) i=0; i<pixel_count(); ++i )
        {
            operator[](i) = e[i];
        }

        return *derived();
    }

    /// iterators
    auto begin() const { return derived()->begin(); }
    auto begin() { return derived()->begin(); }
    auto end() const { return derived()->end(); }
    auto end() { return derived()->end(); }
};

/// ostream operator
template < template<typename> class ImageT, typename ContainedT >
std::ostream& operator<<( std::ostream& os, const ImageInterface<ImageT, ContainedT>& p )
{
    return operator<<(os, *p.derived());
}

