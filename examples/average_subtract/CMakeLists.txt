
# include packages
find_package(cxxopts CONFIG REQUIRED)

add_executable(average_subtract main.cpp)

# include openpivcore
include_directories(${CMAKE_SOURCE_DIR}/openpiv)
target_link_libraries(
  average_subtract
  PRIVATE cxxopts::cxxopts
  openpivcore)
