openpiv_src = [
    'core/size.cpp',
    'core/rect.cpp',
    'core/util.cpp',
    'loaders/image_loader.cpp',
    'loaders/pnm_image_loader.cpp',
    'loaders/tiff_image_loader.cpp',
    libtiff_cpp_include
]

openpiv_lib = shared_library(
    'openpiv',
    openpiv_src,
    include_directories: [openpiv_include, libtiff_include, pocketfft_include],
    dependencies: openpivcore_deps,
    cpp_args: ['-D_USE_MATH_DEFINES'],
    install: true
)