

find_package(pybind11 CONFIG REQUIRED)
pybind11_add_module(
  pyopenpivcore
  core/bindings.cpp
)
target_link_libraries(
  pyopenpivcore
  PRIVATE openpivcore)
