
option(BUILD_PYBIND "build python bindings" OFF)

if (BUILD_PYBIND)
  message("building python bindings")

  find_package(pybind11)

  include_directories(${CMAKE_SOURCE_DIR}/openpiv)
  if (pybind11_FOUND)
    pybind11_add_module(
      pyopenpivcore
      core/module.cpp
      core/grid.cpp
      core/log.cpp
      core/point.cpp
      core/rect.cpp
      core/size.cpp
      core/vector.cpp
      core/pixel_types.cpp
      core/image.cpp
    )

    find_package(fmt CONFIG REQUIRED)
    target_link_libraries(
      pyopenpivcore
      PRIVATE fmt::fmt-header-only
      PRIVATE openpivcore)

    if (UNIX AND APPLE)
      set_target_properties(pyopenpivcore PROPERTIES SUFFIX ".so")
    endif()
  endif()
endif()
