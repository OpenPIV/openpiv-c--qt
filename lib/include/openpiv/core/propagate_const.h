
#pragma once

// std
#include <type_traits>

namespace openpiv::core {

/// Const-propagating wrapper for pointers without the restrictions of
/// std::experimental::propagate_const. It treats the wrapped pointer as a
/// pointer to const when accessed through a const access path, hence the name.
template <typename T>
struct propagate_const
{
    using type = typename std::remove_pointer_t<T>;
    using type_ptr = typename std::add_pointer_t<type>;
    using type_ref = typename std::add_lvalue_reference_t<type>;
    type_ptr _ptr{ nullptr };

    propagate_const() = default;
    propagate_const(const propagate_const&) = default;
    propagate_const(propagate_const&&) = default;
    propagate_const& operator=(const propagate_const&) = default;
    propagate_const& operator=(propagate_const&&) = default;

    propagate_const(type_ptr ptr) : _ptr( ptr ) {}
    propagate_const(std::add_const<type_ptr> ptr) : _ptr( const_cast<type_ptr>(ptr) ) {}

    constexpr operator type_ptr() { return _ptr; }
    constexpr operator const type_ptr() const { return _ptr; }
    constexpr type_ptr operator->() { return _ptr; }
    constexpr const type_ptr operator->() const { return _ptr; }
    constexpr type_ref operator*() { return *_ptr; }
    constexpr const type_ref operator*() const { return *_ptr; }

    bool operator==( const propagate_const& rhs ) const { return _ptr == rhs._ptr; }
    bool operator!=( const propagate_const& rhs ) const { return !operator==(rhs); }
};


}
