cmake_minimum_required(VERSION 3.8)

project(openpiv-c++)

#SET(CMAKE_C_COMPILER clang)
#SET(CMAKE_CXX_COMPILER clang++)

set(CMAKE_CXX_STANDARD 17)

set(openpiv-c++_VERSION_MAJOR 0)
set(openpiv-c++_VERSION_MINOR 1)
set(openpiv-c++_VERSION_PATCH 0)

enable_testing()

if(WIN32)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif(WIN32)

add_subdirectory(openpiv)
add_subdirectory(test)
add_subdirectory(examples)

#get_cmake_property(_variableNames VARIABLES)
#foreach (_variableName ${_variableNames})
#  message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()
