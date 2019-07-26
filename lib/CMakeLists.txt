
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/openpiv)

set( SOURCE
  ${CMAKE_CURRENT_SOURCE_DIR}/src/image_loader.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/pnm_image_loader.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/size.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/rect.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/util.cpp )
set( LIBS )

find_package(TIFF)
if(TIFF_FOUND)
  # no reliable way to get libtiffxx so build local copy of
  # tif_stream.cxx
  set(TIFF_EXTERNAL_LIBRARY_CXX ${PROJECT_SOURCE_DIR}/external/libtiff/4.0.10)
  set(SOURCE
    ${SOURCE}
    ${TIFF_EXTERNAL_LIBRARY_CXX}/tif_stream.cxx
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tiff_image_loader.cpp )

  set(LIBS
    ${LIBS}
    ${TIFF_LIBRARIES})

  include_directories(${TIFF_EXTERNAL_LIBRARY_CXX})
  include_directories(${TIFF_INCLUDE_DIR})
endif()

find_package(Threads REQUIRED)
set(LIBS ${LIBS} Threads::Threads)

set(LIBNAME openpivcore)
add_compile_options(-ffast-math)
add_library(${LIBNAME} SHARED ${SOURCE})
target_compile_definitions(${LIBNAME} PUBLIC _USE_MATH_DEFINES)
target_link_libraries(${LIBNAME} ${LIBS})

# install
#install(TARGETS ${LIBNAME} DESTINATION lib/${LIBNAME})

# Install library headers
#file(GLOB HEADERS include/*.h)
#install(FILES ${HEADERS} DESTINATION include/${LIBNAME})
