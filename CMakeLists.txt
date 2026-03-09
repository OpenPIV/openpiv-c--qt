cmake_minimum_required(VERSION 3.8)

# Use vcpkg if toolchain is set or on Windows
if(DEFINED ENV{CMAKE_TOOLCHAIN_FILE} OR WIN32)
  set(vcpkg_root ${CMAKE_CURRENT_SOURCE_DIR}/external/vcpkg)
  include(cmake/utils.cmake)
  setup_vcpkg_before_project()
endif()

project(openpiv-c++)

set(CMAKE_CXX_STANDARD 17)

set(openpiv-c++_VERSION_MAJOR 0)
set(openpiv-c++_VERSION_MINOR 1)
set(openpiv-c++_VERSION_PATCH 0)

enable_testing()

option(OPENPIV_BUILD_TESTS "Build unit tests" ON)
option(OPENPIV_BUILD_EXAMPLES "Build example applications" ON)

if(WIN32)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif(WIN32)

if(MSVC)
  set(CMAKE_CXX_FLAGS "/EHsc /W3 /WX")
else()
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
  set(CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unknown-pragmas")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG -g -O3 -ffast-math")
endif()

add_subdirectory(openpiv)
if(OPENPIV_BUILD_TESTS)
  add_subdirectory(test)
endif()

if(OPENPIV_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
