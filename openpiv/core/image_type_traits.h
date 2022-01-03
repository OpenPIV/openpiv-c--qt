
#pragma once

// std
#include <type_traits>

namespace openpiv::core {

// forwards
template < typename ContainedT > class image_view;
template < typename ContainedT > class image;

template <typename T>
struct is_imagetype : std::false_type
{};

template <typename ContainedT>
struct is_imagetype<image<ContainedT>> : std::true_type
{};

template <typename ContainedT>
struct is_imagetype<image_view<ContainedT>> : std::true_type
{};

template <typename T>
inline constexpr bool is_imagetype_v = is_imagetype<T>::value;

}
