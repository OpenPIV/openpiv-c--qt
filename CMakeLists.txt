cmake_minimum_required(VERSION 3.8)

project(openpiv-c++)

set(CMAKE_CXX_STANDARD 17)

set(openpiv-c++_VERSION_MAJOR 0)
set(openpiv-c++_VERSION_MINOR 1)
set(openpiv-c++_VERSION_PATCH 0)

enable_testing()

add_subdirectory(lib)
add_subdirectory(test)
add_subdirectory(examples)

#get_cmake_property(_variableNames VARIABLES)
#foreach (_variableName ${_variableNames})
#  message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()
