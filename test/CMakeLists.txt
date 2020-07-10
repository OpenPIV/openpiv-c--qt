#
# unit tests:
#

# include catch
find_package(Catch2 REQUIRED)

# create an archive for catch-main
add_library(catch-main STATIC main.cpp)

file(GLOB SOURCES *_test.cpp)
foreach(SOURCE ${SOURCES})
  get_filename_component(OUTPUT ${SOURCE} NAME_WE)
  message("building ${OUTPUT} from ${SOURCE}")

  # for each file, build a binary
  add_executable(${OUTPUT} ${SOURCE})

  # include openpivcore, catch2
  get_target_property(CATCH2_INCLUDE_DIRECTORIES Catch2::Catch2 INTERFACE_INCLUDE_DIRECTORIES)
  include_directories(${CMAKE_SOURCE_DIR}/openpiv ${CATCH2_INCLUDE_DIRECTORIES})
  target_link_libraries(${OUTPUT} catch-main openpivcore)

  # add as a test
  add_test(
    NAME ${OUTPUT}
    COMMAND ${OUTPUT}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/test)
endforeach()

#
# benchmark tests:
#

# include google benchmark
find_package(benchmark CONFIG REQUIRED)

file(GLOB BM_SOURCES *_benchmark.cpp)
foreach(BM_SOURCE ${BM_SOURCES})
  get_filename_component(OUTPUT ${BM_SOURCE} NAME_WE)
  message("building ${OUTPUT} from ${BM_SOURCE}")

  # for each file, build a binary
  add_executable(${OUTPUT} ${BM_SOURCE})

  # include openpivcore
  include_directories(${CMAKE_SOURCE_DIR})
  target_link_libraries(${OUTPUT} openpivcore benchmark::benchmark benchmark::benchmark_main)
endforeach()
