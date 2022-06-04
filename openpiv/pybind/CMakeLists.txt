

find_package(pybind11 CONFIG REQUIRED)
pybind11_add_module(
  pyopenpivcore
  core/module.cpp
  core/size.cpp
  core/point.cpp
  core/vector.cpp
)
target_link_libraries(
  pyopenpivcore
  PRIVATE openpivcore)

if (UNIX AND APPLE)
  set_target_properties(pyopenpivcore PROPERTIES SUFFIX ".so")
endif()
