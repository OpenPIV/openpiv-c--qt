
// openpiv
#include "core/size.h"

using namespace openpiv;
using namespace openpiv::core;

// pybiond
#include <pybind11/pybind11.h>

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
        .def(py::init<size::component_t, size::component_t>())
        .def("width", &size::width)
        .def("height", &size::height);

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
