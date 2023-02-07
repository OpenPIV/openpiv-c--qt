
#pragma once

// std
#include <cmath>
#include <limits>
#include <istream>
#include <numeric>
#include <optional>
#include <sstream>
#include <thread>
#include <typeinfo>
#include <type_traits>
#include <utility>

// openpiv
#include "core/exception_builder.h"

// define POSIX-style endianness constants for MSVC
// (to be replaced w/ C++20 std::endian)
#ifdef _MSC_VER
  #define __ORDER_LITTLE_ENDIAN__ 1
  #define __ORDER_BIG_ENDIAN__ 0
  #define __BYTE_ORDER__ 1
#endif

namespace openpiv::core {

/// strongly typed memcpy with ability to stride over data
template <typename T,
          typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
void* typed_memcpy( void* dest, const T* src, size_t count, size_t stride = 1 );

/// strongly typed memset
template <typename T>
void* typed_memset( void* dest, const T& src, size_t count );

/// strongly typed memcmp
template <typename T>
int typed_memcmp( const T* s1, const T* s2, size_t count );

/// simple entry/exit logger with indent
class entry_exit_logger
{
private:
    static thread_local uint8_t indent;

    std::ostream& os_;
    std::string s_;
    std::thread::id id_;

public:
    entry_exit_logger( std::ostream& os, const std::string& s );
    ~entry_exit_logger();
};

#if defined(DEBUG)
# define DECLARE_ENTRY_EXIT entry_exit_logger __func__##entry_exit_logger( std::cout, __func__ );
# define DECLARE_ENTRY_EXIT_STREAM(s) entry_exit_logger __func__##entry_exit_logger( s, __func__ );
#else
# define DECLARE_ENTRY_EXIT
# define DECLARE_ENTRY_EXIT_STREAM(s)
#endif

#pragma warning(disable: 4146) // unary minus operator applied to unsigned type, result still unsigned
/// determine if \a v is a power of two
inline constexpr bool is_pow2( uint64_t v )
{
    return v && (v == (v&-v));
}
#pragma warning(default: 4146)

/// simple RAII for std::istream multi-char peek; allows caller
/// to "peek" at multiple bytes and will restore stream upon destruction:
///
///
class peeker
{
public:
    peeker( std::istream& is );
    ~peeker();

private:
    std::istream& is_;
    std::istream::pos_type pos_ {-1};
};

/// allow safe conversion from unsigned to signed
template <typename To, typename From>
constexpr
To checked_unsigned_conversion(const From& v);

template <typename To, typename... From>
struct are_all_convertible
{
    constexpr static bool value = (std::is_convertible_v<From,To> && ...);
};


template <typename To, typename... From>
inline constexpr bool are_all_convertible_v = are_all_convertible<To, From...>::value;

template <typename TypeA, typename... TypeB>
struct are_all_equal
{
    constexpr static bool value = (std::is_same_v<TypeA, TypeB> && ...);
};

template <typename To, typename... From>
inline constexpr bool are_all_equal_v = are_all_equal<To, From...>::value;


template <typename TypeA, typename... TypeB>
struct are_any_equal
{
    constexpr static bool value = (std::is_same_v<TypeA, TypeB> || ...);
};


template <typename To, typename... From>
inline constexpr bool are_any_equal_v = are_any_equal<To, From...>::value;


template<typename Dest, typename Src, std::size_t N>
auto convert_array_to(const std::array<Src, N> &src) -> std::array<Dest, N>;


}

#include "core/detail/util.impl.h"
