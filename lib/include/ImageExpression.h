
#pragma once

// std
#include <type_traits>
#include <utility>

// local
#include "PixelTypes.h"
#include "Size.h"

// forwards
template < typename ContainedT > class ImageView;
template < typename ContainedT > class Image;
template < template <typename> class ImageT, typename ContainedT > class ImageInterface;


/// wrapper around a constant to adhere to requirements of
/// ImageExpression
template <typename T>
class ConstImageExpressionNode
{
public:
    using type = T;

    ConstImageExpressionNode() = default;
    ConstImageExpressionNode(const ConstImageExpressionNode&) = default;
    ConstImageExpressionNode(ConstImageExpressionNode&&) = default;
    explicit ConstImageExpressionNode(const T& t, const Size& s)
        : t_(t)
        , size_(s)
    {}

    explicit ConstImageExpressionNode(T&& t)
        : t_(std::move(t))
    {}

    inline constexpr T operator[](size_t) const
    {
        return t_;
    }

    inline constexpr Size size() const
    {
        return size_;
    }

private:
    T t_ {};
    Size size_;
};

/// wrapper around an image interface, allowing storage of
/// an expression node without copying the image data
template <template <typename> class ImageT,
          typename ContainedT,
          typename T = ImageInterface<ImageT, ContainedT>>
class ImageInterfaceExpressionNode
{
public:
    using type = T;

    ImageInterfaceExpressionNode() = default;
    ImageInterfaceExpressionNode(const ImageInterfaceExpressionNode&) = default;
    ImageInterfaceExpressionNode(ImageInterfaceExpressionNode&&) = default;
    explicit ImageInterfaceExpressionNode(const T& im)
        : im_(im)
    {}

    inline constexpr ContainedT operator[](size_t i) const
    {
        return im_[i];
    }

    inline constexpr Size size() const
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
class ImageExpression
{
public:
    using type = typename LeftExpr::type;

    ImageExpression() = delete;
    ImageExpression(const ImageExpression&) = default;
    ImageExpression& operator=(const ImageExpression&) = default;
    ImageExpression(ImageExpression&&) = default;
    ImageExpression& operator=(ImageExpression&&) = default;

    template < typename LE, typename RE >
    ImageExpression(LE&& le, RE&& re)
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

    inline constexpr Size size() const
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
class UnaryImageExpression
{
public:
    using type = typename Expr::type;

    UnaryImageExpression() = delete;
    UnaryImageExpression(const UnaryImageExpression&) = default;
    UnaryImageExpression& operator=(const UnaryImageExpression&) = default;
    UnaryImageExpression(UnaryImageExpression&&) = default;
    UnaryImageExpression& operator=(UnaryImageExpression&&) = default;

    template < typename E >
    UnaryImageExpression(E&& expr)
        : expr_(std::forward<E>(expr))
    {}

    inline const Expr& expr() const { return expr_; }

    inline auto operator[](size_t index) const ->
        decltype( Op::apply(this->expr()[index]) )
    {
        return Op::apply(expr()[index]);
    }

    inline constexpr Size size() const
    {
        return expr_.size();
    }

private:
    Expr expr_;
};

// fundamental pixel operators
template <typename T>
struct conjugate_op
{};

template <typename T>
struct conjugate_op< Complex<T> >
{
    constexpr static Complex<T> apply(const Complex<T>& v)
    {
        return v.conj();
    }
};

template <typename T>
struct abs_op
{};

template <typename T>
struct abs_op< Complex<T> >
{
    constexpr static Complex<T> apply(const Complex<T>& v)
    {
        return v.abs();
    }
};

template <typename T>
struct abs_sqr_op
{};

template <typename T>
struct abs_sqr_op< Complex<T> >
{
    constexpr static Complex<T> apply(const Complex<T>& v)
    {
        return v.abs_sqr();
    }
};

template <typename T>
struct real_op
{};

template <typename T>
struct real_op< Complex<T> >
{
    constexpr static T apply(const Complex<T>& v)
    {
        return v.real;
    }
};

template <typename T>
struct imag_op
{};

template <typename T>
struct imag_op< Complex<T> >
{
    constexpr static T apply(const Complex<T>& v)
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
struct is_imageexpression<ImageExpression<OP, LE, RE>> : std::true_type
{
    using type = typename LE::type;
    using node_type = ImageExpression<OP, LE, RE>;
};

template <typename T>
struct is_imageexpression<ConstImageExpressionNode<T>> : std::true_type
{
    using type = T;
    using node_type = ConstImageExpressionNode<T>;
};

template <typename T, typename OP>
struct is_imageexpression<UnaryImageExpression<OP, T>> : std::true_type
{
    using type = T;
    using node_type = UnaryImageExpression<OP, T>;
};

///
template <typename T>
struct is_ie_inputtype : std::false_type
{};

template <typename LE, typename RE, typename OP>
struct is_ie_inputtype<ImageExpression<OP, LE, RE>> : std::true_type
{
    using type = typename LE::type;
    using node_type = ImageExpression<OP, LE, RE>;
};

template <typename ContainedT>
struct is_ie_inputtype<Image<ContainedT>> : std::true_type
{
    using type = ContainedT;
    using node_type = ImageInterfaceExpressionNode<Image, ContainedT>;
};

template <typename ContainedT>
struct is_ie_inputtype<ImageView<ContainedT>> : std::true_type
{
    using type = ContainedT;
    using node_type = ImageInterfaceExpressionNode<ImageView, ContainedT>;
};

template <typename T>
struct is_ie_inputtype<ConstImageExpressionNode<T>> : std::true_type
{
    using type = T;
    using node_type = ConstImageExpressionNode<T>;
};

template <typename T, typename OP>
struct is_ie_inputtype<UnaryImageExpression<OP, T>> : std::true_type
{
    using type = T;
    using node_type = UnaryImageExpression<OP, T>;
};

///
template <
    typename LE,
    typename RE,
    typename T = typename std::enable_if<
        is_ie_inputtype<LE>::value && is_ie_inputtype<RE>::value,
        typename LE::type>::type,
    typename Op = plus_op<T>,
    typename LEWrapped = typename is_ie_inputtype<LE>::node_type,
    typename REWrapped = typename is_ie_inputtype<RE>::node_type>
ImageExpression<Op, LEWrapped, REWrapped>
operator+(const LE& le, const RE& re)
{
    return { LEWrapped{ le }, REWrapped{ re } };
}

template <
    typename LE,
    typename T,
    typename Op = plus_op<T>,
    typename E_ = typename std::enable_if<is_ie_inputtype<LE>::value && is_pixeltype<T>::value>::type,
    typename LEWrapped = typename is_ie_inputtype<LE>::node_type,
    typename REWrapped = ConstImageExpressionNode<T>>
ImageExpression<Op, LEWrapped, REWrapped>
operator+(const LE& le, T&& v)
{
    return { LEWrapped{ le }, REWrapped{ std::forward<T>(v), le.size() } };
}

template <
    typename T,
    typename RE,
    typename Op = plus_op<T>,
    typename E_ = typename std::enable_if<is_ie_inputtype<RE>::value && is_pixeltype<T>::value>::type,
    typename LEWrapped = ConstImageExpressionNode<T>,
    typename REWrapped = typename is_ie_inputtype<RE>::node_type>
ImageExpression<Op, LEWrapped, REWrapped>
operator+(T&& v, const RE& re)
{
    return { LEWrapped{ std::forward<T>(v), re.size() }, REWrapped{ re } };
}

///
template <
    typename LE,
    typename RE,
    typename T = typename std::enable_if<
        is_ie_inputtype<LE>::value && is_ie_inputtype<RE>::value,
        typename LE::type>::type,
    typename Op = minus_op<T>,
    typename LEWrapped = typename is_ie_inputtype<LE>::node_type,
    typename REWrapped = typename is_ie_inputtype<RE>::node_type>
ImageExpression<Op, LEWrapped, REWrapped>
operator-(const LE& le, const RE& re)
{
    return { LEWrapped{ le }, REWrapped{ re } };
}

template <
    typename LE,
    typename T,
    typename Op = minus_op<T>,
    typename E_ = typename std::enable_if<is_ie_inputtype<LE>::value && is_pixeltype<T>::value>::type,
    typename LEWrapped = typename is_ie_inputtype<LE>::node_type,
    typename REWrapped = ConstImageExpressionNode<T>>
ImageExpression<Op, LEWrapped, REWrapped>
operator-(const LE& le, T&& v)
{
    return { LEWrapped{ le }, REWrapped{ std::forward<T>(v), le.size() } };
}

template <
    typename T,
    typename RE,
    typename Op = minus_op<T>,
    typename E_ = typename std::enable_if<is_ie_inputtype<RE>::value && is_pixeltype<T>::value>::type,
    typename LEWrapped = ConstImageExpressionNode<T>,
    typename REWrapped = typename is_ie_inputtype<RE>::node_type>
ImageExpression<Op, LEWrapped, REWrapped>
operator-(T&& v, const RE& re)
{
    return { LEWrapped{ std::forward<T>(v), re.size() }, REWrapped{ re } };
}

template < typename ValueT >
UnaryImageExpression<conjugate_op<Complex<ValueT>>, ImageInterfaceExpressionNode<Image, Complex<ValueT>>>
conj(const Image<Complex<ValueT>>& im)
{
    return { im };
}

template < typename ValueT >
UnaryImageExpression<abs_op<Complex<ValueT>>, ImageInterfaceExpressionNode<Image, Complex<ValueT>>>
abs(const Image<Complex<ValueT>>& im)
{
    return { im };
}

template < typename ValueT >
UnaryImageExpression<abs_sqr_op<Complex<ValueT>>, ImageInterfaceExpressionNode<Image, Complex<ValueT>>>
abs_sqr(const Image<Complex<ValueT>>& im)
{
    return { im };
}

template < typename ValueT >
UnaryImageExpression<real_op<Complex<ValueT>>, ImageInterfaceExpressionNode<Image, Complex<ValueT>>>
real(const Image<Complex<ValueT>>& im)
{
    return { im };
}

template < typename ValueT >
UnaryImageExpression<imag_op<Complex<ValueT>>, ImageInterfaceExpressionNode<Image, Complex<ValueT>>>
imag(const Image<Complex<ValueT>>& im)
{
    return { im };
}

///
template <
    typename LE,
    typename RE,
    typename T = typename std::enable_if<
        is_ie_inputtype<LE>::value && is_ie_inputtype<RE>::value,
        typename LE::type>::type,
    typename Op = div_op<T>,
    typename LEWrapped = typename is_ie_inputtype<LE>::node_type,
    typename REWrapped = typename is_ie_inputtype<RE>::node_type>
ImageExpression<Op, LEWrapped, REWrapped>
operator/(const LE& le, const RE& re)
{
    return { LEWrapped{ le }, REWrapped{ re } };
}

template <
    typename LE,
    typename T,
    typename Op = div_op<T>,
    typename E_ = typename std::enable_if<is_ie_inputtype<LE>::value && is_pixeltype<T>::value>::type,
    typename LEWrapped = typename is_ie_inputtype<LE>::node_type,
    typename REWrapped = ConstImageExpressionNode<T>>
ImageExpression<Op, LEWrapped, REWrapped>
operator/(const LE& le, T&& v)
{
    return { LEWrapped{ le }, REWrapped{ std::forward<T>(v), le.size() } };
}

template <
    typename T,
    typename RE,
    typename Op = div_op<T>,
    typename E_ = typename std::enable_if<is_ie_inputtype<RE>::value && is_pixeltype<T>::value>::type,
    typename LEWrapped = ConstImageExpressionNode<T>,
    typename REWrapped = typename is_ie_inputtype<RE>::node_type>
ImageExpression<Op, LEWrapped, REWrapped>
operator/(T&& v, const RE& re)
{
    return { LEWrapped{ std::forward<T>(v), re.size() }, REWrapped{ re } };
}

///
template <
    typename LE,
    typename RE,
    typename T = typename std::enable_if<
        is_ie_inputtype<LE>::value && is_ie_inputtype<RE>::value,
        typename LE::type>::type,
    typename Op = mult_op<T>,
    typename LEWrapped = typename is_ie_inputtype<LE>::node_type,
    typename REWrapped = typename is_ie_inputtype<RE>::node_type>
ImageExpression<Op, LEWrapped, REWrapped>
operator*(const LE& le, const RE& re)
{
    return { LEWrapped{ le }, REWrapped{ re } };
}

template <
    typename LE,
    typename T,
    typename Op = mult_op<T>,
    typename E_ = typename std::enable_if<is_ie_inputtype<LE>::value && is_pixeltype<T>::value>::type,
    typename LEWrapped = typename is_ie_inputtype<LE>::node_type,
    typename REWrapped = ConstImageExpressionNode<T>>
ImageExpression<Op, LEWrapped, REWrapped>
operator*(const LE& le, T&& v)
{
    return { LEWrapped{ le }, REWrapped{ std::forward<T>(v), le.size() } };
}

template <
    typename T,
    typename RE,
    typename Op = mult_op<T>,
    typename E_ = typename std::enable_if<is_ie_inputtype<RE>::value && is_pixeltype<T>::value>::type,
    typename LEWrapped = ConstImageExpressionNode<T>,
    typename REWrapped = typename is_ie_inputtype<RE>::node_type>
ImageExpression<Op, LEWrapped, REWrapped>
operator*(T&& v, const RE& re)
{
    return { LEWrapped{ std::forward<T>(v), re.size() }, REWrapped{ re } };
}

///
template <
    typename LE,
    typename RE,
    typename T = typename std::enable_if<
        is_ie_inputtype<LE>::value && is_ie_inputtype<RE>::value,
        typename LE::type>::type,
    typename Op = mod_op<T>,
    typename LEWrapped = typename is_ie_inputtype<LE>::node_type,
    typename REWrapped = typename is_ie_inputtype<RE>::node_type>
ImageExpression<Op, LEWrapped, REWrapped>
operator%(const LE& le, const RE& re)
{
    return { LEWrapped{ le }, REWrapped{ re } };
}

template <
    typename LE,
    typename T,
    typename Op = mod_op<T>,
    typename E_ = typename std::enable_if<is_ie_inputtype<LE>::value && is_pixeltype<T>::value>::type,
    typename LEWrapped = typename is_ie_inputtype<LE>::node_type,
    typename REWrapped = ConstImageExpressionNode<T>>
ImageExpression<Op, LEWrapped, REWrapped>
operator%(const LE& le, T&& v)
{
    return { LEWrapped{ le }, REWrapped{ std::forward<T>(v), le.size() } };
}

template <
    typename T,
    typename RE,
    typename Op = mod_op<T>,
    typename E_ = typename std::enable_if<is_ie_inputtype<RE>::value && is_pixeltype<T>::value>::type,
    typename LEWrapped = ConstImageExpressionNode<T>,
    typename REWrapped = typename is_ie_inputtype<RE>::node_type>
ImageExpression<Op, LEWrapped, REWrapped>
operator%(T&& v, const RE& re)
{
    return { LEWrapped{ std::forward<T>(v), re.size() }, REWrapped{ re } };
}
