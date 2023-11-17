avd_sub_exec = executable(
    'average_subtract',
    'main.cpp',
    include_directories: openpiv_include,
    dependencies: [
        cxxopts_dep,
        fmt_dep,
        thread_dep
    ],
    link_with: openpiv_lib,
    install: true
)