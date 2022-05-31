
// openpiv
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

PYBIND11_MODULE(pyopenpivcore, m) {
    m.doc() = R"pbdoc(
        openpivcore python plugin
        -------------------------

        .. currentmodule:: pyopenpivcore

        .. autosummary::
           :toctree: _generate

        size
    )pbdoc";

    // size
    py::class_<size>(m, "size")
        .def(py::init())
        .def(py::init<size::component_t, size::component_t>())
        .def("width", &size::width)
        .def("height", &size::height)
        .def("area", &size::area)
        .def("components", &size::components)
        .def(py::self += py::self)
        .def(py::self + py::self)
        .def(py::self -= py::self)
        .def(py::self - py::self)
        .def("__repr__",
             [](const size& s) {
                 std::ostringstream ss;
                 ss << s;
                 return ss.str();
             });
    m.def("maximal_size", &maximal_size);
    m.def("minimal_size", &minimal_size);
    m.def("transpose", &transpose);

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
