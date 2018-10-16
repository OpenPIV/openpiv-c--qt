
# include catch
include_directories(${CMAKE_SOURCE_DIR}/3rd-party/catch/single_include/catch2)

# create an archive for catch-main
add_library(catch-main STATIC main.cpp)

file(GLOB SOURCES *.cpp)
list(REMOVE_ITEM SOURCES "main.cpp")
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
