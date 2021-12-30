[![Build Status](https://travis-ci.org/timdewhirst/openpiv-c--qt.svg?branch=master)](https://travis-ci.org/timdewhirst/openpiv-c--qt)

OpenPIV (c++)
=============

An implementation of a PIV analysis engine in C++ using as few dependencies as possible;
the implementation requires a c++17 compliant compiler.

Build
-----

There are some external dependencies under external/, so when cloneing use:

```git clone --recursive-submodules <path to git repo>```

Building uses cmake; to build (linux example):

* cd to cloned source directory
* mkdir build; cd build
* cmake ..
* make

To run tests:

* make test

To enable a release build:

```cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..```

Dependencies
------------

* c++17 compiler e.g. clang++-5.0, gcc7
* [vcpkg](https://github.com/Microsoft/vcpkg)
  * catch2: unit test framework: vcpkg install catch2
  * libtiff: TIFF IO support: vcpkg install tiff
  * spdlog: logging framework: vcpkg install spdlog
  * benchmark: used to run performance benchmarks: vcpkg install benchmark
  * async++ (optional): implements c++17 parallel algorithms: vcpkg install asyncplusplus
  * cxxopts: nice command line parsing: vcpkg install cxxopts

Examples
--------

* under build/examples are two simple applications:
  * [process](examples/process/README.md): a straight-forward PIV cross correlator
  * [average_subtract](examples/average_subtract/README.md): a utility to read in n
    images, find the average and write out n new images with the mean subtracted

Logging
-------

Logging is done via [spdlog](https://github.com/gabime/spdlog), to change the
log level you can use the environmental variable `SPDLOG_LEVEL=<level>`.

TODO
----

* build
  * [x] travis/CI
  * [x] add clang/windows/OS X build
* core
  * [x] logging
  * [ ] iostream ops for ImageLoaders
  * [ ] image allocator support
    * [ ] shared memory
    * [ ] pooled memory
  * [ ] image loading
    * [x] load/save PNM files
      * [x] save double image
    * [x] support loading multiple images from files: modify loader interface to read header, report number of images, extract indexed images
    * [ ] memory map files - check performance for large files
    * [ ] PNG - lodepng
    * [ ] RAW - libraw looks less than ideal but no alternative?
    * [ ] b16/PCO
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
    * [ ] use SIMD?
    * [ ] real -> complex FFT/correlation of real data
  * [ ] direct correlation
  * [x] peak detection
  * [x] peak fitting
* image processing
  * [ ] change image_view to use array of pointers for each row?
  * [ ] low/high pass filter
* examples
  * [x] parallel cross-correlate
  * [x] image processing
* processing framework
  * [x] cartesian grid generator
  * [ ] further grid generators
  * [ ] median validation with secondary peak check and interpolation
  * [ ] store signal/noise value
  * [ ] processing
  * [ ] marking
  * [ ] iterative analysis
  * [ ] PIV guided PTV?
* data output
  * [ ] output registry
  * [ ] ASCII/CSV
  * [ ] gnuplot/pyplot?
  * [ ] tecplot
* GUI
  * [ ] Qt?
  * [ ] websockets/HTML5?
  * [ ] webassembly + JS/HTML5?
