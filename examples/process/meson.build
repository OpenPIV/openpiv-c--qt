avd_sub_exec = executable(
    'process',
    ['main.cpp', 'threadpool.hpp'],
    include_directories: [openpiv_include, pocketfft_include],
    dependencies: [
        cxxopts_dep,
        fmt_dep,
        thread_dep
    ],
    cpp_args: ['-D_USE_MATH_DEFINES', '-mavx2'],
    link_with: openpiv_lib,
    install: true
)