
// openpiv
#include "core/exception_builder.h"
#include "core/size.h"
#include "core/stream_utils.h"

using namespace openpiv;
using namespace openpiv::core;

// pybind
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>

// std
#include <sstream>

namespace py = pybind11;

void add_size(py::module& m)
{
    auto to_str = [](const size& s) {
        std::ostringstream ss;
        ss << s;
        return ss.str();
    };

    // size
    py::class_<size>(m, "size")
        .def(py::init())
        .def(py::init<size::component_t, size::component_t>())
        .def(py::init(
                 [](const std::vector<size::component_t>& components){
                     if ( components.size() != 2 )
                         exception_builder<std::runtime_error>() << "two components required to build size: " << components;

                     return new size( components[0], components[1] );
                 }))
        .def("width", &size::width)
        .def("height", &size::height)
        .def("area", &size::area)
        .def("components", &size::components)
        .def(py::self += py::self)
        .def(py::self + py::self)
        .def(py::self -= py::self)
        .def(py::self - py::self)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def("__repr__", to_str)
        .def("__str__", to_str);
    m.def("maximal_size", &maximal_size);
    m.def("minimal_size", &minimal_size);
    m.def("transpose", &transpose);
    py::implicitly_convertible<py::list, size>();
}
