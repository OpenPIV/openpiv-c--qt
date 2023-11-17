test_source_files = [
  'enum_helper_test.cpp',
  'grid_test.cpp',
  'image_algos_test.cpp',
  'image_expression_test.cpp',
  'image_loader_test.cpp',
  'image_stats_test.cpp',
  'image_test.cpp',
  'image_utils_test.cpp',
  'image_view_test.cpp',
  'log_test.cpp',
  'pixel_types_test.cpp',
  'point_test.cpp',
  'range_test.cpp',
  'rect_test.cpp',
  'size_test.cpp',
  'stream_utils_test.cpp',
  'util_test.cpp',
  'vector_test.cpp'
]

foreach test_file: test_source_files
  source_name = test_file.split('.')[0]
  
  exec = executable(
    source_name,
    test_file,
    include_directories: [openpiv_include, libtiff_include],
    dependencies: openpiv_deps + [catch2_dep],
    cpp_args: ['-D_USE_MATH_DEFINES'],
    link_with: openpiv_lib
  )
  
  test(
    source_name,
    exec
  )
  
endforeach

if found_benchmark
  benchmark_source_files = [
    'image_algos_benchmark.cpp',
    'image_utils_benchmark.cpp'
  ]
  
  foreach test_file: test_source_files
    source_name = test_file.split('.')[0]
  
    exec = executable(
      source_name,
      test_file,
      include_directories: [openpiv_include, libtiff_include],
      dependencies: openpiv_deps + [catch2_dep],
      cpp_args: ['-D_USE_MATH_DEFINES'],
      link_with: openpiv_lib
    )
  
    test(
      source_name,
      exec
    )
  
  endforeach
endif

fs.copyfile(
  'corr_a.tiff'
)

fs.copyfile(
  'test-mono.tiff'
)

fs.copyfile(
  'test-rgb.tiff'
)