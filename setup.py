"""
Setup script for OpenPIV C++ Python bindings.

Build wheels with:
    pip install build cibuildwheel
    python -m cibuildwheel --output-dir wheelhouse

Or build locally with:
    pip install build
    python -m build --wheel
"""

import shutil
import subprocess
import sys
from pathlib import Path

import pybind11
from setuptools import Extension, setup
from setuptools.command.build_ext import build_ext


ROOT_DIR = Path(__file__).parent.resolve()
BUILD_DIR = ROOT_DIR / "builddir"
LIBRARY_NAME = "openpivcore"


def build_cpp_library():
    """Build the C++ library using CMake."""
    print("Building C++ library with CMake...")
    BUILD_DIR.mkdir(parents=True, exist_ok=True)

    configure_command = [
        "cmake",
        "-S",
        str(ROOT_DIR),
        "-B",
        str(BUILD_DIR),
        "-DCMAKE_BUILD_TYPE=Release",
        "-DOPENPIV_BUILD_TESTS=OFF",
        "-DOPENPIV_BUILD_EXAMPLES=OFF",
        "-DVCPKG_MANIFEST_MODE=OFF",
    ]
    
    # Use vcpkg toolchain if explicitly set via environment variable
    vcpkg_toolchain = ROOT_DIR / "external" / "vcpkg" / "scripts" / "buildsystems" / "vcpkg.cmake"
    if vcpkg_toolchain.exists():
        toolchain_file = Path(__import__("os").environ.get("CMAKE_TOOLCHAIN_FILE", ""))
        if toolchain_file.exists() or sys.platform == "win32":
            configure_command.extend([
                "-DCMAKE_TOOLCHAIN_FILE=" + str(vcpkg_toolchain),
            ])
    
    if shutil.which("ninja") is not None:
        configure_command.extend(["-G", "Ninja"])

    subprocess.check_call(configure_command)
    subprocess.check_call(
        [
            "cmake",
            "--build",
            str(BUILD_DIR),
            "--config",
            "Release",
        ]
    )
    print("C++ library built successfully!")


def find_library_file(patterns):
    candidate_dirs = [
        BUILD_DIR / "openpiv" / "Release",
        BUILD_DIR / "openpiv",
        BUILD_DIR / "out" / "Release",
        BUILD_DIR / "out",
        BUILD_DIR / "Release",
        BUILD_DIR,
    ]

    for directory in candidate_dirs:
        if not directory.exists():
            continue
        for pattern in patterns:
            matches = sorted(directory.glob(pattern))
            if matches:
                return matches[0]
    return None


def get_library_info():
    """Locate the built shared library and import library."""
    if sys.platform == "win32":
        shared_patterns = [f"{LIBRARY_NAME}.dll"]
        link_patterns = [f"{LIBRARY_NAME}.lib"]
    elif sys.platform == "darwin":
        shared_patterns = [f"lib{LIBRARY_NAME}.dylib"]
        link_patterns = shared_patterns
    else:
        shared_patterns = [f"lib{LIBRARY_NAME}.so"]
        link_patterns = shared_patterns

    shared_library = find_library_file(shared_patterns)
    link_library = find_library_file(link_patterns)

    if shared_library is None and link_library is None:
        raise FileNotFoundError(f"Could not locate built {LIBRARY_NAME} library under {BUILD_DIR}")

    link_path = link_library or shared_library
    return {
        "shared_library": shared_library,
        "library_dir": str(link_path.parent),
    }


class CustomBuildExt(build_ext):
    """Custom build extension that builds the C++ library first."""

    def run(self):
        build_cpp_library()
        self.library_info = get_library_info()
        super().run()

    def build_extension(self, ext):
        extdir = Path(self.get_ext_fullpath(ext.name)).parent
        extdir.mkdir(parents=True, exist_ok=True)

        if getattr(self, "compiler", None) is not None and self.compiler.compiler_type == "msvc":
            extra_compile_args = ["/O2", "/std:c++17"]
            extra_link_args = []
        else:
            extra_compile_args = ["-O3", "-std=c++17", "-fvisibility=hidden"]
            if sys.platform == "darwin":
                extra_link_args = ["-Wl,-rpath,@loader_path"]
            else:
                extra_link_args = ["-Wl,-rpath,$ORIGIN"]

        ext.include_dirs.append(pybind11.get_include())
        ext.extra_compile_args = extra_compile_args
        ext.extra_link_args = extra_link_args
        ext.libraries = [LIBRARY_NAME]
        ext.library_dirs = [self.library_info["library_dir"]]

        super().build_extension(ext)

        shared_library = self.library_info["shared_library"]
        if shared_library is not None:
            # Copy to build output directory
            destination = extdir / shared_library.name
            if shared_library.resolve() != destination.resolve():
                shutil.copy2(shared_library, destination)
            
            # For editable installs, also copy to source directory
            source_pkg_dir = ROOT_DIR / "openpiv_cpp_pkg"
            if source_pkg_dir.exists():
                source_destination = source_pkg_dir / shared_library.name
                if shared_library.resolve() != source_destination.resolve():
                    shutil.copy2(shared_library, source_destination)


def get_extensions():
    """Get extension modules."""
    return [
        Extension(
            "openpiv_cpp_pkg.openpiv_cpp",
            ["piv_bindings.cpp"],
            include_dirs=[
                "openpiv",
                "external/pocketfft",
            ],
            language="c++",
        ),
    ]


readme_path = ROOT_DIR / "README.md"
long_description = ""
if readme_path.exists():
    long_description = readme_path.read_text(encoding="utf-8")


setup(
    ext_modules=get_extensions(),
    cmdclass={"build_ext": CustomBuildExt},
    include_package_data=True,
    package_data={"openpiv_cpp_pkg": ["*.dll", "*.dylib", "*.pyd", "*.so"]},
    long_description=long_description,
    long_description_content_type="text/markdown",
)
