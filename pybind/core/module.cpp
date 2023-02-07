
// pybind
#include <pybind11/pybind11.h>

namespace py = pybind11;

void add_grid(py::module &);
void add_log(py::module &);
void add_point(py::module &);
void add_rect(py::module &);
void add_size(py::module &);
void add_vector(py::module &);
void add_pixel_types(py::module &);
void add_image(py::module &);
void add_loader(py::module &);

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
    add_grid(m);
    add_log(m);
    add_point(m);
    add_rect(m);
    add_size(m);
    add_vector(m);
    add_pixel_types(m);
    add_image(m);
    add_loader(m);

#ifdef VERSION_INFO
    m.attr("__version__") = MACRO_STRINGIFY(VERSION_INFO);
#else
    m.attr("__version__") = "dev";
#endif
}
