
// openpiv
#include "core/rect.h"
#include "core/size.h"

using namespace openpiv;
using namespace openpiv::core;

// pybind
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>

// std
#include <sstream>

namespace py = pybind11;

void add_rect(py::module& m)
{
    // size
    auto to_str = [](const rect& r) {
        std::ostringstream ss;
        ss << r;
        return ss.str();
    };

    py::class_<rect>(m, "rect")
        .def(py::init())
        .def(py::init<rect::point_t, size>())
        .def_static("from_size", &rect::from_size)
        .def("bottomLeft", &rect::bottomLeft)
        .def("topLeft", &rect::topLeft)
        .def("bottomRight", &rect::bottomRight)
        .def("topRight", &rect::topRight)
        .def("midpoint", &rect::midpoint)
        .def("bottom", &rect::bottom)
        .def("top", &rect::top)
        .def("left", &rect::left)
        .def("right", &rect::right)
        .def("size", &rect::size)
        .def("width", &rect::width)
        .def("height", &rect::height)
        .def("area", &rect::area)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def("__repr__", to_str)
        .def("__str__", to_str);
}
