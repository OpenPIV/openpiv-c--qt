#
# unit tests:
#

# create an archive for catch-main
add_library(catch-main STATIC main.cpp)

# include catch
find_package(Catch2 CONFIG REQUIRED)

file(GLOB SOURCES *Test.cpp)
foreach(SOURCE ${SOURCES})
  get_filename_component(OUTPUT ${SOURCE} NAME_WE)
  message("building ${OUTPUT} from ${SOURCE}")

  # for each file, build a binary
  add_executable(${OUTPUT} ${SOURCE})

  # include openpivcore
  include_directories(${CMAKE_SOURCE_DIR}/lib/include)
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

file(GLOB BM_SOURCES *Benchmark.cpp)
foreach(BM_SOURCE ${BM_SOURCES})
  get_filename_component(OUTPUT ${BM_SOURCE} NAME_WE)
  message("building ${OUTPUT} from ${BM_SOURCE}")

  # for each file, build a binary
  add_executable(${OUTPUT} ${BM_SOURCE})

  # include openpivcore
  include_directories(${CMAKE_SOURCE_DIR}/lib/include)
  target_link_libraries(${OUTPUT} openpivcore benchmark::benchmark benchmark::benchmark_main)
endforeach()