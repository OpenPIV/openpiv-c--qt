[![Build Status](https://travis-ci.org/timdewhirst/openpiv-c--qt.svg?branch=master)](https://travis-ci.org/timdewhirst/openpiv-c--qt)

OpenPIV (c++)
=============

An implementation of a PIV analysis engine in C++ using as few dependencies as possible;
the implementation requires a c++17 compliant compiler.

Build
-----

Building uses cmake; to build (linux example):

* mkdir build
* cd build
* cmake -DCMAKE_TOOLCHAIN_FILE=[vcpkg root]/scripts/buildsystems/vcpkg.cmake ..
* make

To run tests:

* make test

Dependencies
------------

* c++17 compiler e.g. clang++-5.0, gcc7
* [vcpkg](https://github.com/Microsoft/vcpkg)
  * catch2: unit test framework: vcpkg install catch2
  * libtiff: TIFF IO support: vcpkg install tiff
  * spdlog: logging framework (not yet required): vcpkg install spdlog
  * benchmark: used to run performance benchmarks: vcpkg install benchmark
  * async++: implements c++17 parallel algoriths: vcpkg install asyncplusplus
  * cxxopts: nice command line parsing: vcpkg install cxxopts

TODO
----

* build
  * [x] travis/CI
  * [ ] add clang/windows/OS X build
* core
  * [ ] logging
  * [x] load/save PNM files
    * [x] save double image
  * [ ] iostream ops for ImageLoaders
  * [ ] image allocator support
    * [ ] shared memory
    * [ ] pooled memory
  * utilities
    * [x] split RGB to 4xgreyscale
    * [x] join 4xgreyscale to RGB
    * [x] split complex to planes
    * [x] create complex from planes
  * [x] add ImageInterface data iterators
  * [x] remove data() method from ImageInterface, ImageView
  * [x] allow ImageView to be non-const
* algorithms
  * [x] transpose
  * [x] normalize
  * FFT
    * [x] spectral correlation
    * [ ] optimize
    * [ ] openCL
    * [ ] apply kernel in Fourier space
    * [ ] use SIMD
    * [ ] real -> complex FFT
  * [ ] direct correlation
  * [x] peak detection
  * [x] peak fitting
* image processing
  * [ ] change image_view to use array of pointers for each row
  * [ ] low/high pass filter
* examples
  * [x] parallel cross-correlate
* processing framework
  * [x] cartesian grid generator
  * [ ] further grid generators
  * [ ] processing
  * [ ] marking
* data output
  * [ ] ASCII/CSV
  * [ ] gnuplot/pyplot?
  * [ ] tecplot
* GUI
  * [ ] Qt?
  * [ ] websockets/HTML5?
  * [ ] webassembly + JS/HTML5?
