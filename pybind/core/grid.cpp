
// openpiv
#include "core/grid.h"
#include "core/rect.h"
#include "core/size.h"

using namespace openpiv;
using namespace openpiv::core;

// pybind
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

using namespace pybind11::literals;

// std
#include <sstream>

namespace py = pybind11;

void add_grid(py::module& m)
{
    m.def("generate_cartesian_grid_with_relative_offset",
          static_cast<std::vector<rect>(*)(const size&, const core::size&, double)>(&generate_cartesian_grid),
          "image_size"_a,
          "interrogation_size"_a,
          "percentage_offset"_a);
    m.def("generate_cartesian_grid_with_fixed_offset",
          static_cast<std::vector<rect>(*)(const size&, const core::size&, std::array< uint32_t, 2 >)>(&generate_cartesian_grid),
          "image_size"_a,
          "interrogation_size"_a,
          "offsets"_a);
}
