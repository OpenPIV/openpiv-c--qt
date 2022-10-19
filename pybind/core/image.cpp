
// openpiv
#include "core/image.h"

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


template <template <typename> class I,
          template <typename> class P,
          typename U>
struct image_type_trait
{
    template <size_t N=64>
    static constexpr char_array<N> name()
    {
        return cat<N>(to_array<8>("image"), pixel_type_trait<P, U>::name());
    }
};

// test
static_assert(image_type_trait<image, g, uint8_t>::name() == "image_g_u8");
static_assert(image_type_trait<image, rgba, uint16_t>::name() == "image_rgba_u16");
static_assert(image_type_trait<image, yuva, uint32_t>::name() == "image_yuva_u32");
static_assert(image_type_trait<image, complex, double>::name() == "image_c_f");


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

    py::class_<image_t>(m, &image_type_trait<I, P, U>::name()[0])
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
    bool result = (add_image_<image, P, Ts>(m) & ...);
    return result;
}

void add_image(py::module& m)
{
    add_images<g, uint8_t, uint16_t, double>(m);
    add_images<rgba, uint8_t, uint16_t>(m);
    add_images<complex, double>(m);
}

