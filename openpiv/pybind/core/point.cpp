
// openpiv
#include "core/point.h"

using namespace openpiv;
using namespace openpiv::core;

// pybind
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>

// std
#include <sstream>

namespace py = pybind11;

void add_point(py::module& m)
{
    // size
    using point2_i = point2<uint32_t>;
    py::class_<point2_i>(m, "point2_i")
        .def(py::init())
        .def(py::init<point2_i::value_t, point2_i::value_t>())
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def("data", &point2_i::data)
        .def("components", &point2_i::data)
        .def("__repr__",
             [](const point2_i& p) {
                 std::ostringstream ss;
                 ss << p;
                 return ss.str();
             })
        .def("__getitem__",
             [](const point2_i& p, size_t i) {
                 return p[i];
             })
        .def("__setitem__",
             [](point2_i& p, size_t i, point2_i::value_t v) {
                 p[i] = v;
             });
}
