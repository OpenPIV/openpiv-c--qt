
// openpiv
#include "core/vector.h"
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

template <typename T>
struct vector_trait
{
    static constexpr const char* name()
    {
        if constexpr (std::is_same_v<T, int32_t> )
            return "vector2_i";
        else if constexpr (std::is_same_v<T, uint32_t> )
            return "vector2_ui";
        else if constexpr (std::is_same_v<T, double> )
            return "vector2_d";

        return nullptr;
    }
};

template <typename T>
bool add_vector_type(py::module& m)
{
    using vector_t = vector2<T>;
    using value_t = typename vector_t::value_t;

    py::class_<vector_t>(m, vector_trait<T>::name())
        .def(py::init())
        .def(py::init<value_t, value_t>())
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def("data", &vector_t::data)
        .def("components", &vector_t::data)
        .def("__repr__",
             [](const vector_t& p) {
                 std::ostringstream ss;
                 ss << p;
                 return ss.str();
             })
        .def("__getitem__",
             [](const vector_t& p, size_t i) {
                 return p[i];
             })
        .def("__setitem__",
             [](vector_t& p, size_t i, value_t v) {
                 p[i] = v;
             })
        .def(py::self - py::self)
        .def(py::self + py::self)
        .def(py::self * value_t())
        .def(py::self / value_t());

    return true;
}


template <typename... Ts>
bool add_vector_types(py::module& m)
{
    bool result = (add_vector_type<Ts>(m) & ...);
    return result;
}

void add_vector(py::module& m)
{
    // add vector bindings for int32_t, uint32_t and double;
    // to add more types simply add to type list
    add_vector_types<int32_t, uint32_t, double>(m);
}
