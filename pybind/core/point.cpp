
// openpiv
#include "core/exception_builder.h"
#include "core/log.h"
#include "core/point.h"
#include "core/stream_utils.h"
#include "core/vector.h"

using namespace openpiv;
using namespace openpiv::core;
namespace logger = openpiv::core::logger;

// pybind
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>

// std
#include <exception>
#include <sstream>


namespace py = pybind11;

template <typename T>
struct point_trait
{
    static constexpr const char* name()
    {
        if constexpr (std::is_same_v<T, int32_t> )
            return "point2_i";
        else if constexpr (std::is_same_v<T, uint32_t> )
            return "point2_ui";
        else if constexpr (std::is_same_v<T, double> )
            return "point2_d";

        return nullptr;
    }
};

template <typename T>
bool add_point_type(py::module& m)
{
    using point_t = point2<T>;
    using vector_t = vector2<T>;
    using value_t = typename point_t::value_t;

    auto to_str = [](const point_t& p) {
        std::ostringstream ss;
        ss << p;
        return ss.str();
    };

    py::class_<point_t>(m, point_trait<T>::name())
        .def(py::init())
        .def(py::init<value_t, value_t>())
        .def(py::init(
                 [](const std::vector<value_t>& components){
                     if ( components.size() != 2 )
                         exception_builder<std::runtime_error>() << "two components required to build point: " << components;

                     return new point_t(components[0], components[1]);
                 }))
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def("data", &point_t::data)
        .def("components", &point_t::data)
        .def("__repr__", to_str)
        .def("__str__", to_str)
        .def("__getitem__",
             [](const point_t& p, size_t i) {
                 return p[i];
             })
        .def("__setitem__",
             [](point_t& p, size_t i, value_t v) {
                 p[i] = v;
             })
        .def(py::self - py::self)
        .def(py::self + vector_t())
        .def(py::self - vector_t())
        .def(py::self < py::self)
        .def(py::self <= py::self)
        .def(py::self > py::self)
        .def(py::self >= py::self);
    py::implicitly_convertible<py::list, point_t>();

    return true;
}


template <typename... Ts>
bool add_point_types(py::module& m)
{
    bool result = (add_point_type<Ts>(m) && ...);
    return result;
}

void add_point(py::module& m)
{
    // add point bindings for int32_t, uint32_t and double;
    // to add more types simply add to type list
    add_point_types<int32_t, uint32_t, double>(m);
}
