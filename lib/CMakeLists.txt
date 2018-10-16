
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set( SOURCE
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ImageLoader.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/PNMImageLoader.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Size.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Util.cpp )
set( LIBS )

find_package(TIFF)
if(TIFF_FOUND)
  set(SOURCE
    ${SOURCE}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/TIFFImageLoader.cpp )

  # workaround missing libtiffxx
  get_filename_component(TIFF_DIR ${TIFF_LIBRARIES} DIRECTORY)
  get_filename_component(TIFF_SO_EXT ${TIFF_LIBRARIES} EXT)
  get_filename_component(TIFF_LIB ${TIFF_LIBRARIES} NAME_WE)
  set(TIFF_LIBRARIES ${TIFF_LIBRARIES} "${TIFF_DIR}/${TIFF_LIB}xx${TIFF_SO_EXT}")

  set(LIBS
    ${LIBS}
    ${TIFF_LIBRARIES})
  include_directories(TIFF_INCLUDE_DIR)
endif()

find_package(Threads REQUIRED)
set(LIBS ${LIBS} Threads::Threads)

set(LIBNAME openpivcore)
add_library(${LIBNAME} SHARED ${SOURCE})
target_link_libraries(${LIBNAME} ${LIBS})

# install
#install(TARGETS ${LIBNAME} DESTINATION lib/${LIBNAME})

# Install library headers
#file(GLOB HEADERS include/*.h)
#install(FILES ${HEADERS} DESTINATION include/${LIBNAME})
