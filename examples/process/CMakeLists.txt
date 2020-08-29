
# include packages
find_package(cxxopts CONFIG REQUIRED)
find_package(Async++ CONFIG)

add_executable(process main.cpp)

# include openpivcore
include_directories(${CMAKE_SOURCE_DIR}/openpiv)
target_link_libraries(
  process
  PRIVATE cxxopts::cxxopts
  openpivcore)

if (Async++_FOUND)
  message("async++ found; including")
  target_compile_definitions(
    process
    PRIVATE ASYNCPLUSPLUS)
  target_link_libraries(
    process
    PRIVATE Async++)
endif()
