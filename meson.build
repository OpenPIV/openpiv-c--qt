project(
  'OpenPIV-c++',
  'cpp',
  version: '0.2.0',
  license: 'GPL-3',
  meson_version: '>= 1.1.0',
  default_options: [
    'buildtype=release',
    'cpp_std=c++17',
    'warning_level=2'
  ]
)

fs = import('fs')

cpp = meson.get_compiler('cpp')

cxx_compiler = cpp.get_id() 

if cxx_compiler == 'gcc'
  if not cpp.version().version_compare('>=8.4')
    error('OpenPIV-c++ requires GCC >= 8.4')
  endif
endif

if cxx_compiler != 'msvc'
  _gnu_args = cpp.get_supported_arguments(
    '-ffast-math',
    '-Wno-unknown-pragmas',
  )
  
  add_project_arguments(
    _gnu_args, 
    language: 'cpp'
  )
endif

pocketfft_include = include_directories(
  'external/pocketfft'
)

libtiff_include = include_directories(
  'external/libtiff/4.0.10'
)

libtiff_cpp_include = files('external/libtiff/4.0.10/tif_stream.cxx')

openpiv_include = include_directories(
  'openpiv'
)

cxxopts_dep = dependency(
  'cxxopts', 
  include_type: get_option('deps_include_type'), 
  required: true
)

thread_dep = dependency(
  'threads', 
  required: true
)

catch2_dep = dependency(
  'catch2', 
  fallback: ['catch2', 'catch2_with_main_dep'], 
  include_type: get_option('deps_include_type'),
  required: true
)

# meson overrides the header-only dep
fmt_dep = dependency(
  'fmt',
  include_type: get_option('deps_include_type'),
  required: false
)

if not fmt_dep.found()
  fmt = subproject(
    'fmt',
    default_options: 'default_library=static' # meson still installs dynamic library if not static
  )

  fmt_dep = fmt.get_variable('fmt_header_only_dep')
endif

jpeg_dep = dependency(
  'libjpeg', 
  fallback: ['libjpeg-turbo', 'jpeg_dep'], 
  include_type: get_option('deps_include_type'),
  required: true
#  static: true
)

zlib_dep = dependency(
  'zlib', 
  fallback: ['zlib', 'zlib_dep'],
  include_type: get_option('deps_include_type'),
  required: true
#  static: true
)

tiff_dep = dependency(
  'libtiff', 
  fallback: ['libtiff', 'libtiff4_dep'], 
  include_type: get_option('deps_include_type'),
  default_options: ['jpeg=enabled', 'zlib=enabled'],
  required: true
#  static: true
)

found_benchmark = false
if get_option('benchmarks').enabled()
    benchmark_dep = dependency(
      'benchmark', 
      include_type: get_option('deps_include_type')
    )
    found_benchmark = benchmark_dep.found()
endif

libopenpivcore_deps = [
  fmt_dep,
#  jpeg_dep, # jpeg image loaders are not currently implemented
#  png_dep, # png image loaders are not currently implemented
#  raw_dep, # raw image loaders are not currently implemented
  tiff_dep,
  thread_dep
]

openpiv_deps = [
  cxxopts_dep,
  fmt_dep,
  thread_dep
]

subdir('openpiv')

openpiv_dep = declare_dependency(
    include_directories: openpiv_include,
    dependencies: libopenpivcore_deps,
    link_with: openpiv_lib
)

subdir('examples')

if not meson.is_subproject()
  subdir('tests')
endif

pkgconfig = import('pkgconfig')

pkgconfig.generate(
  name: meson.project_name(),
  version: meson.project_version(),
  description: 'Open-source Particle Image Velocimetry in c++',
  libraries: [openpiv_lib]
)