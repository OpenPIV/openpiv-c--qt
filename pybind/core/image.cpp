
// openpiv
#include "core/image.h"
#include "core/util.h"

using namespace openpiv;
using namespace openpiv::core;

// local
#include "pyutils.h"

// std
#include <sstream>
#include <vector>

// pybind
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>

namespace py = pybind11;


template <template <typename> class I,
          template <typename> class P,
          typename U>
bool add_image_(py::module& m)
{
    using pixel_t = P<U>;
    using image_t = I<pixel_t>;

    auto to_str = [](const image_t& im) {
        std::ostringstream ss;
        ss << im;
        return ss.str();
    };

    py::class_<image_t>(m, &image_type_trait<I, P, U>::name()[0], py::buffer_protocol())
        .def_buffer([](image_t& image) -> py::buffer_info {
            const auto dimensions = image_type_trait<I, P, U>::dimensions(image);
            const auto strides = image_type_trait<I, P, U>::strides(image);
            return py::buffer_info(
                image.data(),                       // pointer to raw data
                sizeof( U ),                        // size of a single underlying data element
                py::format_descriptor<U>::format(), // mapping to python char code for underlying
                dimensions.size(),                  // number of dimensions
                dimensions,                         // dimensions in matrix convention
                strides);                           // strides in matrix convention
        })
        .def(py::init())
        .def(py::init<image_t>())
        .def(py::init<uint32_t, uint32_t>())
        .def(py::init<size>())
        .def(py::init<rect>())
        .def(py::init<uint32_t, uint32_t, pixel_t>())
        .def(py::init<size, pixel_t>())
        .def(py::init<rect, pixel_t>())
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def("data", static_cast<const pixel_t* (image_t::*)() const>(&image_t::data))
        .def("line", static_cast<const pixel_t* (image_t::*)(size_t) const>(&image_t::line))
        .def("width", &image_t::width)
        .def("height", &image_t::height)
        .def("size", &image_t::size)
        .def("pixel_count", &image_t::pixel_count)
        .def("rect", &image_t::rect)
        .def("__getitem__",
             [](const image_t& p, size_t i) {
                 return p[i];
             })
        .def("__setitem__",
             [](image_t& p, size_t i, pixel_t v) {
                 p[i] = v;
             })
        .def("__getitem__",
             [](const image_t& p, const point2<uint32_t>& i) {
                 return p[i];
             })
        .def("__setitem__",
             [](image_t& p, const point2<uint32_t>& i, pixel_t v) {
                 p[i] = v;
             })
        .def("__repr__", to_str)
        .def("__str__", to_str);

    return true;
}


template <template <typename> class P, typename... Ts>
bool add_images(py::module& m)
{
    bool result = (add_image_<image, P, Ts>(m) && ...);
    return result;
}

void add_image(py::module& m)
{
    add_images<g, uint8_t, uint16_t, double>(m);
    add_images<rgba, uint8_t, uint16_t>(m);
    add_images<complex, double>(m);
}

