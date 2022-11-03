#
# unit tests:
#

# include catch
find_package(Catch2 REQUIRED)
find_package(Threads REQUIRED)
find_package(fmt CONFIG REQUIRED)

file(GLOB SOURCES *_test.cpp)
foreach(SOURCE ${SOURCES})
  get_filename_component(OUTPUT ${SOURCE} NAME_WE)
  message("building ${OUTPUT} from ${SOURCE}")

  # for each file, build a binary
  add_executable(${OUTPUT} ${SOURCE})

  # include openpivcore
  include_directories(${CMAKE_SOURCE_DIR}/openpiv)

  target_link_libraries(
    ${OUTPUT}
    PRIVATE Catch2::Catch2WithMain
    PRIVATE openpivcore
    PRIVATE fmt::fmt-header-only
    PRIVATE Threads::Threads)

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
  target_link_libraries(
    ${OUTPUT}
    PRIVATE openpivcore
    PRIVATE fmt::fmt-header-only
    PRIVATE benchmark::benchmark
    PRIVATE benchmark::benchmark_main)
endforeach()
