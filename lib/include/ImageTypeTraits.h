
#pragma once

// std
#include <type_traits>

// forwards
template < typename ContainedT > class ImageView;
template < typename ContainedT > class Image;

template <typename T>
struct is_imagetype : std::false_type
{};

template <typename ContainedT>
struct is_imagetype<Image<ContainedT>> : std::true_type
{};

template <typename ContainedT>
struct is_imagetype<ImageView<ContainedT>> : std::true_type
{};
