
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set( SOURCE
  ${CMAKE_CURRENT_SOURCE_DIR}/src/image_loader.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/pnm_image_loader.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/size.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/rect.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/util.cpp )
set( LIBS )

find_package(TIFF)
if(TIFF_FOUND)
  set(SOURCE
    ${SOURCE}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/tiff_image_loader.cpp )

  # workaround missing libtiffxx
  get_filename_component(TIFF_DIR ${TIFF_LIBRARY} DIRECTORY)
  get_filename_component(TIFF_SO_EXT ${TIFF_LIBRARY} EXT)
  get_filename_component(TIFF_LIB ${TIFF_LIBRARY} NAME_WE)
  set(TIFF_LIBRARIES ${TIFF_LIBRARIES} "${TIFF_DIR}/${TIFF_LIB}xx${TIFF_SO_EXT}")

  set(LIBS
    ${LIBS}
    ${TIFF_LIBRARIES})
  include_directories(TIFF_INCLUDE_DIR)
endif()

find_package(Threads REQUIRED)
set(LIBS ${LIBS} Threads::Threads)

set(LIBNAME openpivcore)
add_compile_options(-ffast-math)
add_library(${LIBNAME} SHARED ${SOURCE})
target_link_libraries(${LIBNAME} ${LIBS})

# install
#install(TARGETS ${LIBNAME} DESTINATION lib/${LIBNAME})

# Install library headers
#file(GLOB HEADERS include/*.h)
#install(FILES ${HEADERS} DESTINATION include/${LIBNAME})
