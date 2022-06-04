
// pybind
#include <pybind11/pybind11.h>

namespace py = pybind11;

void add_size(py::module &);
void add_point(py::module &);
void add_vector(py::module &);

PYBIND11_MODULE(pyopenpivcore, m) {
    m.doc() = R"pbdoc(
        openpivcore python plugin
        -------------------------

        .. currentmodule:: pyopenpivcore

        .. autosummary::
           :toctree: _generate

        size
    )pbdoc";

    // add each binding chunk here
    add_size(m);
    add_point(m);
    add_vector(m);

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
