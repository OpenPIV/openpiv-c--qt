
// openpiv
#include "core/log.h"

using namespace openpiv;
using namespace openpiv::core;
using namespace openpiv::core::logger;

// pybind
#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>

// std
#include <functional>
#include <sstream>
#include <string>

namespace py = pybind11;

Logger::sink_id_t add_sink_(Logger::sink_t sink)
{
    // wrap the passed function in a GIL lock as the sink is called
    // from a logging thread in C++
    auto gil_sink = [sink](Level level, const std::string& str) -> bool {
        pybind11::gil_scoped_acquire lock{};
        return sink(level, str);
    };

    auto result = Logger::instance().add_sink(gil_sink);
    warn("python logging sink added: ensure this is removed before this module is unloaded by calling remove_sink");

    return result;
}

void add_log(py::module& m)
{
    py::enum_<Level>(m, "Level")
        .value("NONE",  Level::NONE)
        .value("FATAL", Level::FATAL)
        .value("ERROR", Level::ERROR)
        .value("WARN",  Level::WARN)
        .value("INFO",  Level::INFO)
        .value("DEBUG", Level::DEBUG)
        .value("TEST",  Level::TEST)
   .export_values();

    m.def("add_sink", &add_sink_);
    m.def("remove_sink",
          [](Logger::sink_id_t id) -> bool
          {
              return Logger::instance().remove_sink(id);
          });
    m.def("test_log", [](){ debug("hello, world! {}", 3.141); });
}
