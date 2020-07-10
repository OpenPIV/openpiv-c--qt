
#pragma once

// std
#include <type_traits>
#include <utility>

// local
#include "core/pixel_types.h"
#include "core/image_type_traits.h"
#include "core/size.h"

namespace openpiv::core {

// forwards
template < typename ContainedT > class image_view;
template < typename ContainedT > class image;


/// wrapper around a constant to adhere to requirements of
/// ImageExpression
template <typename T>
class const_image_expression_node
{
public:
    using type = T;

    const_image_expression_node() = default;
    const_image_expression_node(const const_image_expression_node&) = default;
    const_image_expression_node(const_image_expression_node&&) = default;
    explicit const_image_expression_node(const T& t, const size& s)
        : t_(t)
        , size_(s)
    {}

    explicit const_image_expression_node(T&& t)
        : t_(std::move(t))
    {}

    inline constexpr T operator[](size_t) const
    {
        return t_;
    }

    inline constexpr core::size size() const
    {
        return size_;
    }

private:
    T t_ {};
    core::size size_;
};

/// wrapper around an image interface, allowing storage of
/// an expression node without copying the image data
template <template <typename> class ImageT,
          typename ContainedT,
          typename T = ImageT<ContainedT>>
class image_interface_expression_node
{
public:
    using type = T;

    image_interface_expression_node() = default;
    image_interface_expression_node(const image_interface_expression_node&) = default;
    image_interface_expression_node(image_interface_expression_node&&) = default;
    explicit image_interface_expression_node(const T& im)
        : im_(im)
    {}

    inline constexpr ContainedT operator[](size_t i) const
    {
        return im_[i];
    }

    inline constexpr core::size size() const
    {
        return im_.size();
    }

private:
    const T& im_;
};


/// template expression to construct chains of binary operations
/// to perform on a per-pixel level for images;
/// \ta Op is always passed as a template parameter to allow
/// full expansion without indirection
template <typename Op, typename LeftExpr, typename RightExpr>
class image_expression
{
public:
    using type = typename LeftExpr::type;

    image_expression() = delete;
    image_expression(const image_expression&) = default;
    image_expression& operator=(const image_expression&) = default;
    image_expression(image_expression&&) = default;
    image_expression& operator=(image_expression&&) = default;

    template < typename LE, typename RE >
    image_expression(LE&& le, RE&& re)
        : le_(std::forward<LE>(le))
        , re_(std::forward<RE>(re))
    {}

    inline const LeftExpr& le() const { return le_; }
    inline const RightExpr& re() const { return re_; }

    inline auto operator[](size_t index) const ->
        decltype( Op::apply(this->le()[index], this->re()[index]) )
    {
        return Op::apply(le()[index], re()[index]);
    }

    inline constexpr core::size size() const
    {
        return le_.size();
    }

private:
    LeftExpr le_;
    RightExpr re_;
};


/// template expression to construct chains of unary operations
/// to perform on a per-pixel level for images;
/// \ta Op is always passed as a template parameter to allow
/// full expansion without indirection
template <typename Op, typename Expr>
class unary_image_expression
{
public:
    using type = typename Expr::type;

    unary_image_expression() = delete;
    unary_image_expression(const unary_image_expression&) = default;
    unary_image_expression& operator=(const unary_image_expression&) = default;
    unary_image_expression(unary_image_expression&&) = default;
    unary_image_expression& operator=(unary_image_expression&&) = default;

    template < typename E >
    unary_image_expression(E&& expr)
        : expr_(std::forward<E>(expr))
    {}

    inline const Expr& expr() const { return expr_; }

    inline auto operator[](size_t index) const ->
        decltype( Op::apply(this->expr()[index]) )
    {
        return Op::apply(expr()[index]);
    }

    inline constexpr core::size size() const
    {
        return expr_.size();
    }

private:
    Expr expr_;
};

//
//
// fundamental pixel operators
template <typename T>
struct conjugate_op
{};

template <typename T>
struct conjugate_op< complex<T> >
{
    constexpr static complex<T> apply(const complex<T>& v)
    {
        return v.conj();
    }
};

template <typename T>
struct abs_op
{};

template <typename T>
struct abs_op< complex<T> >
{
    constexpr static complex<T> apply(const complex<T>& v)
    {
        return v.abs();
    }
};

template <typename T>
struct abs_sqr_op
{};

template <typename T>
struct abs_sqr_op< complex<T> >
{
    constexpr static complex<T> apply(const complex<T>& v)
    {
        return v.abs_sqr();
    }
};

template <typename T>
struct real_op
{};

template <typename T>
struct real_op< complex<T> >
{
    constexpr static T apply(const complex<T>& v)
    {
        return v.real;
    }
};

template <typename T>
struct imag_op
{};

template <typename T>
struct imag_op< complex<T> >
{
    constexpr static T apply(const complex<T>& v)
    {
        return v.imag;
    }
};

template <typename T>
struct plus_op
{
    constexpr static T apply(const T& lhs, const T& rhs)
    {
        return lhs + rhs;
    }
};

template <typename T>
struct minus_op
{
    constexpr static T apply(const T& lhs, const T& rhs)
    {
        return lhs - rhs;
    }

    constexpr static T apply(const T& lhs)
    {
        return T{-1} * lhs;
    }
};

template <typename T>
struct mult_op
{
    constexpr static T apply(const T& lhs, const T& rhs)
    {
        return lhs * rhs;
    }
};

template <typename T>
struct div_op
{
    constexpr static T apply(const T& lhs, const T& rhs)
    {
        return lhs / rhs;
    }
};

template <typename T>
struct mod_op
{
    constexpr static T apply(const T& lhs, const T& rhs)
    {
        return lhs % rhs;
    }
};


///
template <typename T>
struct is_imageexpression : std::false_type
{};

template <typename LE, typename RE, typename OP>
struct is_imageexpression<image_expression<OP, LE, RE>> : std::true_type
{
    using type = typename LE::type;
    using node_type = image_expression<OP, LE, RE>;
};

template <typename T>
struct is_imageexpression<const_image_expression_node<T>> : std::true_type
{
    using type = T;
    using node_type = const_image_expression_node<T>;
};

template <typename T, typename OP>
struct is_imageexpression<unary_image_expression<OP, T>> : std::true_type
{
    using type = T;
    using node_type = unary_image_expression<OP, T>;
};

template <typename T>
inline constexpr bool is_imageexpression_v = is_imageexpression<T>::value;

///
template <typename T>
struct is_ie_inputtype : std::false_type
{};

template <typename LE, typename RE, typename OP>
struct is_ie_inputtype<image_expression<OP, LE, RE>> : std::true_type
{
    using type = typename LE::type;
    using node_type = image_expression<OP, LE, RE>;
};

template <typename ContainedT>
struct is_ie_inputtype<image<ContainedT>> : std::true_type
{
    using type = ContainedT;
    using node_type = image_interface_expression_node<image, ContainedT>;
};

template <typename ContainedT>
struct is_ie_inputtype<image_view<ContainedT>> : std::true_type
{
    using type = ContainedT;
    using node_type = image_interface_expression_node<image_view, ContainedT>;
};

template <typename T>
struct is_ie_inputtype<const_image_expression_node<T>> : std::true_type
{
    using type = T;
    using node_type = const_image_expression_node<T>;
};

template <typename T, typename OP>
struct is_ie_inputtype<unary_image_expression<OP, T>> : std::true_type
{
    using type = typename T::type;
    using node_type = unary_image_expression<OP, T>;
};

template <typename T>
inline constexpr bool is_ie_inputtype_v = is_ie_inputtype<T>::value;

///
template <
    typename LE,
    typename RE,
    typename T = typename std::enable_if_t<
        is_ie_inputtype_v<LE> && is_ie_inputtype_v<RE>,
        typename is_ie_inputtype<LE>::type>,
    typename Op = plus_op<T>,
    typename LEWrapped = typename is_ie_inputtype<LE>::node_type,
    typename REWrapped = typename is_ie_inputtype<RE>::node_type>
image_expression<Op, LEWrapped, REWrapped>
operator+(const LE& le, const RE& re)
{
    return { LEWrapped{ le }, REWrapped{ re } };
}

template <
    typename LE,
    typename T,
    typename Op = plus_op<T>,
    typename = typename std::enable_if_t<is_ie_inputtype_v<LE> && is_pixeltype_v<T>>,
    typename LEWrapped = typename is_ie_inputtype<LE>::node_type,
    typename REWrapped = const_image_expression_node<T>>
image_expression<Op, LEWrapped, REWrapped>
operator+(const LE& le, T&& v)
{
    return { LEWrapped{ le }, REWrapped{ std::forward<T>(v), le.size() } };
}

template <
    typename T,
    typename RE,
    typename Op = plus_op<T>,
    typename = typename std::enable_if_t<is_ie_inputtype_v<RE> && is_pixeltype_v<T>>,
    typename LEWrapped = const_image_expression_node<T>,
    typename REWrapped = typename is_ie_inputtype<RE>::node_type>
image_expression<Op, LEWrapped, REWrapped>
operator+(T&& v, const RE& re)
{
    return { LEWrapped{ std::forward<T>(v), re.size() }, REWrapped{ re } };
}

///
template <
    typename LE,
    typename RE,
    typename T = typename std::enable_if_t<
        is_ie_inputtype_v<LE> && is_ie_inputtype_v<RE>,
        typename is_ie_inputtype<LE>::type>,
    typename Op = minus_op<T>,
    typename LEWrapped = typename is_ie_inputtype<LE>::node_type,
    typename REWrapped = typename is_ie_inputtype<RE>::node_type>
image_expression<Op, LEWrapped, REWrapped>
operator-(const LE& le, const RE& re)
{
    return { LEWrapped{ le }, REWrapped{ re } };
}

template <
    typename LE,
    typename T,
    typename Op = minus_op<T>,
    typename = typename std::enable_if_t<is_ie_inputtype_v<LE> && is_pixeltype_v<T>>,
    typename LEWrapped = typename is_ie_inputtype<LE>::node_type,
    typename REWrapped = const_image_expression_node<T>>
image_expression<Op, LEWrapped, REWrapped>
operator-(const LE& le, T&& v)
{
    return { LEWrapped{ le }, REWrapped{ std::forward<T>(v), le.size() } };
}

template <
    typename T,
    typename RE,
    typename Op = minus_op<T>,
    typename = typename std::enable_if_t<is_ie_inputtype_v<RE> && is_pixeltype_v<T>>,
    typename LEWrapped = const_image_expression_node<T>,
    typename REWrapped = typename is_ie_inputtype<RE>::node_type>
image_expression<Op, LEWrapped, REWrapped>
operator-(T&& v, const RE& re)
{
    return { LEWrapped{ std::forward<T>(v), re.size() }, REWrapped{ re } };
}

template < typename ValueT >
unary_image_expression<conjugate_op<complex<ValueT>>, image_interface_expression_node<image, complex<ValueT>>>
conj(const image<complex<ValueT>>& im)
{
    return { im };
}

template < typename ValueT >
unary_image_expression<abs_op<complex<ValueT>>, image_interface_expression_node<image, complex<ValueT>>>
abs(const image<complex<ValueT>>& im)
{
    return { im };
}

template < typename ValueT >
unary_image_expression<abs_sqr_op<complex<ValueT>>, image_interface_expression_node<image, complex<ValueT>>>
abs_sqr(const image<complex<ValueT>>& im)
{
    return { im };
}

template < typename ValueT >
unary_image_expression<real_op<complex<ValueT>>, image_interface_expression_node<image, complex<ValueT>>>
real(const image<complex<ValueT>>& im)
{
    return { im };
}

template < typename ValueT >
unary_image_expression<imag_op<complex<ValueT>>, image_interface_expression_node<image, complex<ValueT>>>
imag(const image<complex<ValueT>>& im)
{
    return { im };
}

///
template <
    typename LE,
    typename RE,
    typename T = typename std::enable_if_t<
        is_ie_inputtype_v<LE> && is_ie_inputtype_v<RE>,
        typename is_ie_inputtype<LE>::type>,
    typename Op = div_op<T>,
    typename LEWrapped = typename is_ie_inputtype<LE>::node_type,
    typename REWrapped = typename is_ie_inputtype<RE>::node_type>
image_expression<Op, LEWrapped, REWrapped>
operator/(const LE& le, const RE& re)
{
    return { LEWrapped{ le }, REWrapped{ re } };
}

template <
    typename LE,
    typename T,
    typename Op = div_op<T>,
    typename = typename std::enable_if_t<is_ie_inputtype_v<LE> && is_pixeltype_v<T>>,
    typename LEWrapped = typename is_ie_inputtype<LE>::node_type,
    typename REWrapped = const_image_expression_node<T>>
image_expression<Op, LEWrapped, REWrapped>
operator/(const LE& le, T&& v)
{
    return { LEWrapped{ le }, REWrapped{ std::forward<T>(v), le.size() } };
}

template <
    typename T,
    typename RE,
    typename Op = div_op<T>,
    typename = typename std::enable_if_t<is_ie_inputtype_v<RE> && is_pixeltype_v<T>>,
    typename LEWrapped = const_image_expression_node<T>,
    typename REWrapped = typename is_ie_inputtype<RE>::node_type>
image_expression<Op, LEWrapped, REWrapped>
operator/(T&& v, const RE& re)
{
    return { LEWrapped{ std::forward<T>(v), re.size() }, REWrapped{ re } };
}

///
template <
    typename LE,
    typename RE,
    typename T = typename std::enable_if_t<
        is_ie_inputtype_v<LE> && is_ie_inputtype_v<RE>,
        typename is_ie_inputtype<LE>::type>,
    typename Op = mult_op<T>,
    typename LEWrapped = typename is_ie_inputtype<LE>::node_type,
    typename REWrapped = typename is_ie_inputtype<RE>::node_type>
image_expression<Op, LEWrapped, REWrapped>
operator*(const LE& le, const RE& re)
{
    return { LEWrapped{ le }, REWrapped{ re } };
}

template <
    typename LE,
    typename T,
    typename Op = mult_op<T>,
    typename = typename std::enable_if_t<is_ie_inputtype_v<LE> && is_pixeltype_v<T>>,
    typename LEWrapped = typename is_ie_inputtype<LE>::node_type,
    typename REWrapped = const_image_expression_node<T>>
image_expression<Op, LEWrapped, REWrapped>
operator*(const LE& le, T&& v)
{
    return { LEWrapped{ le }, REWrapped{ std::forward<T>(v), le.size() } };
}

template <
    typename T,
    typename RE,
    typename Op = mult_op<T>,
    typename = typename std::enable_if_t<is_ie_inputtype_v<RE> && is_pixeltype_v<T>>,
    typename LEWrapped = const_image_expression_node<T>,
    typename REWrapped = typename is_ie_inputtype<RE>::node_type>
image_expression<Op, LEWrapped, REWrapped>
operator*(T&& v, const RE& re)
{
    return { LEWrapped{ std::forward<T>(v), re.size() }, REWrapped{ re } };
}

///
template <
    typename LE,
    typename RE,
    typename T = typename std::enable_if_t<
        is_ie_inputtype_v<LE> && is_ie_inputtype_v<RE>,
        typename is_ie_inputtype<LE>::type>,
    typename Op = mod_op<T>,
    typename LEWrapped = typename is_ie_inputtype<LE>::node_type,
    typename REWrapped = typename is_ie_inputtype<RE>::node_type>
image_expression<Op, LEWrapped, REWrapped>
operator%(const LE& le, const RE& re)
{
    return { LEWrapped{ le }, REWrapped{ re } };
}

template <
    typename LE,
    typename T,
    typename Op = mod_op<T>,
    typename = typename std::enable_if_t<is_ie_inputtype_v<LE> && is_pixeltype_v<T>>,
    typename LEWrapped = typename is_ie_inputtype<LE>::node_type,
    typename REWrapped = const_image_expression_node<T>>
image_expression<Op, LEWrapped, REWrapped>
operator%(const LE& le, T&& v)
{
    return { LEWrapped{ le }, REWrapped{ std::forward<T>(v), le.size() } };
}

template <
    typename T,
    typename RE,
    typename Op = mod_op<T>,
    typename = typename std::enable_if_t<is_ie_inputtype_v<RE> && is_pixeltype_v<T>>,
    typename LEWrapped = const_image_expression_node<T>,
    typename REWrapped = typename is_ie_inputtype<RE>::node_type>
image_expression<Op, LEWrapped, REWrapped>
operator%(T&& v, const RE& re)
{
    return { LEWrapped{ std::forward<T>(v), re.size() }, REWrapped{ re } };
}


}
