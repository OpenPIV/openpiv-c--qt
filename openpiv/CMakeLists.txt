
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# ensure we check local libs first
set(CMAKE_FIND_FRAMEWORK LAST)

set(SOURCE
  ${CMAKE_CURRENT_SOURCE_DIR}/core/size.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/core/rect.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/core/util.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/loaders/image_loader.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/loaders/pnm_image_loader.cpp)
set(LIBS)

set(LIBNAME openpivcore)
add_library(${LIBNAME} SHARED ${SOURCE})
target_compile_definitions(${LIBNAME} PUBLIC _USE_MATH_DEFINES)

# include path for pocket fft
set(POCKETFFT_EXTERNAL_LIBRARY_CXX ${PROJECT_SOURCE_DIR}/external/pocketfft)
include_directories(${POCKETFFT_EXTERNAL_LIBRARY_CXX})

# include packages
find_package(TIFF)
if(TIFF_FOUND)
  message("found libtiff")
  # no reliable way to get libtiffxx so build local copy of
  # tif_stream.cxx
  set(TIFF_EXTERNAL_LIBRARY_CXX ${PROJECT_SOURCE_DIR}/external/libtiff/4.0.10)
  target_sources(
    ${LIBNAME}
    PRIVATE
    ${TIFF_EXTERNAL_LIBRARY_CXX}/tif_stream.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/loaders/tiff_image_loader.cpp )

  include_directories(${TIFF_EXTERNAL_LIBRARY_CXX})
  include_directories(BEFORE ${TIFF_INCLUDE_DIR})

  target_link_libraries(${LIBNAME} PRIVATE ${TIFF_LIBRARIES})
endif()

find_package(mimalloc CONFIG)
if(mimalloc_FOUND)
  message("found mimalloc")
  target_link_libraries(
    ${LIBNAME}
    PRIVATE mimalloc mimalloc-static)
endif()

find_package(Threads REQUIRED)
find_package(fmt CONFIG REQUIRED)
target_link_libraries(
  ${LIBNAME}
  PRIVATE fmt::fmt-header-only
  PRIVATE Threads::Threads)

# install
#install(TARGETS ${LIBNAME} DESTINATION lib/${LIBNAME})

# Install library headers
#file(GLOB HEADERS include/*.h)
#install(FILES ${HEADERS} DESTINATION include/${LIBNAME})
