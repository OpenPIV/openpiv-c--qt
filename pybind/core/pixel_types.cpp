
// openpiv
#include "core/pixel_types.h"

using namespace openpiv;
using namespace openpiv::core;

// local
#include "pyutils.h"

// pybind
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>

// std
#include <sstream>


namespace py = pybind11;


template <typename T>
bool add_rgba_type(py::module& m)
{
    using pixel_t = rgba<T>;
    using value_t = typename pixel_t::value_t;

    auto to_str = [](const pixel_t& p) {
        std::ostringstream ss;
        ss << p;
        return ss.str();
    };

    py::class_<pixel_t>(m, &pixel_type_trait<rgba, T>::name()[0])
        .def(py::init())
        .def(py::init<value_t>())
        .def(py::init<value_t, value_t, value_t, value_t>())
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def_readwrite("r", &pixel_t::r)
        .def_readwrite("g", &pixel_t::g)
        .def_readwrite("b", &pixel_t::b)
        .def_readwrite("a", &pixel_t::a)
        .def("__repr__", to_str)
        .def("__str__", to_str);

    return true;
}

template <typename... Ts>
bool add_rgba_types(py::module& m)
{
    bool result = (add_rgba_type<Ts>(m) & ...);
    return result;
}


template <typename T>
bool add_yuva_type(py::module& m)
{
    using pixel_t = yuva<T>;
    using value_t = typename pixel_t::value_t;

    auto to_str = [](const pixel_t& p) {
        std::ostringstream ss;
        ss << p;
        return ss.str();
    };

    py::class_<pixel_t>(m, &pixel_type_trait<yuva, T>::name()[0])
        .def(py::init())
        .def(py::init<value_t>())
        .def(py::init<value_t, value_t, value_t, value_t>())
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def_readwrite("y", &pixel_t::y)
        .def_readwrite("u", &pixel_t::u)
        .def_readwrite("v", &pixel_t::v)
        .def_readwrite("a", &pixel_t::a)
        .def("__repr__", to_str)
        .def("__str__", to_str);

    return true;
}

template <typename... Ts>
bool add_yuva_types(py::module& m)
{
    bool result = (add_yuva_type<Ts>(m) & ...);
    return result;
}


template <typename T>
bool add_complex_type(py::module& m)
{
    using pixel_t = complex<T>;
    using value_t = typename pixel_t::value_t;

    auto to_str = [](const pixel_t& p) {
        std::ostringstream ss;
        ss << p;
        return ss.str();
    };

    py::class_<pixel_t>(m, &pixel_type_trait<complex, T>::name()[0])
        .def(py::init())
        .def(py::init<value_t, value_t>())
        .def(py::init<pixel_t>())
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def(py::self - py::self)
        .def(py::self -= py::self)
        .def(py::self + py::self)
        .def(py::self += py::self)
        .def(py::self * py::self)
        .def(py::self *= py::self)
        .def(py::self / py::self)
        .def(py::self /= py::self)
        .def("conj", &pixel_t::conj)
        .def("abs", &pixel_t::abs)
        .def("abs_sqr", &pixel_t::abs_sqr)
        .def_readwrite("real", &pixel_t::real)
        .def_readwrite("imag", &pixel_t::imag)
        .def("__repr__", to_str)
        .def("__str__", to_str)
        .def(T{} - py::self)
        .def(T{} + py::self)
        .def(T{} * py::self)
        .def(T{} / py::self);
    m.def("exp", &openpiv::core::exp<T>);

    return true;
}

template <typename... Ts>
bool add_complex_types(py::module& m)
{
    bool result = (add_complex_type<Ts>(m) & ...);
    return result;
}



template <typename T>
bool add_g_type(py::module& m)
{
    using pixel_t = g<T>;
    using value_t = typename pixel_t::value_t;

    auto to_str = [](const pixel_t& p) {
        std::ostringstream ss;
        ss << p;
        return ss.str();
    };

    py::class_<pixel_t>(m, &pixel_type_trait<g, T>::name()[0])
        .def(py::init())
        .def(py::init<value_t>())
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def_readwrite("v", &pixel_t::v)
        .def("__repr__", to_str)
        .def("__str__", to_str);

    return true;
}

template <typename... Ts>
bool add_g_types(py::module& m)
{
    bool result = (add_g_type<Ts>(m) & ...);
    return result;
}

void add_pixel_types(py::module& m)
{
    add_rgba_types<uint8_t, uint16_t, uint32_t>(m);
    add_yuva_types<uint8_t, uint16_t, uint32_t>(m);
    add_g_types<uint8_t, uint16_t, uint32_t, double>(m);
    add_complex_types<int8_t, int16_t, int32_t, double>(m);
}

