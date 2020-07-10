
# include packages
find_package(cxxopts CONFIG REQUIRED)
find_package(Async++ CONFIG REQUIRED)

add_executable(process main.cpp)

# include openpivcore
include_directories(${CMAKE_SOURCE_DIR}/openpiv)
target_link_libraries(
  process
  PRIVATE Async++
  PRIVATE cxxopts::cxxopts
  openpivcore)
