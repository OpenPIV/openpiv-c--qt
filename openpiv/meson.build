openpiv_src = [
  'core/size.cpp',
  'core/rect.cpp',
  'core/util.cpp',
  'loaders/image_loader.cpp',
  'loaders/pnm_image_loader.cpp',
  'loaders/tiff_image_loader.cpp',
  libtiff_cpp_include
]

cpp_args = [
  '-D_USE_MATH_DEFINES'
]

if cxx_compiler == 'gcc'
  cpp_args += cpp.get_supported_arguments(
    '-export-symbols'
  )
elif cxx_compiler == 'msvc'
  cpp_args += ['-DEXPORT_DLL_SYMBOLS']
endif

openpiv_lib = shared_library(
  'openpivcore',
  openpiv_src,
  include_directories: [openpiv_include, pocketfft_include],
  dependencies: libopenpivcore_deps,
  cpp_args: cpp_args,
  install: true
)