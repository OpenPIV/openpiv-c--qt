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

Dependencies
------------

* c++17 compiler e.g. clang++-5.0, gcc7
* [vcpkg](https://github.com/Microsoft/vcpkg)
  * vcpkg install catch2
  * vcpkg install tiff
  * vcpkg install spdlog
  * vcpkg install benchmark

TODO
----

* logging
* core
  * load/save PNM files - done
    * save double images - done
  * iostream ops for ImageLoaders
  * shared memory -> Image allocator support
  * utilities
    * split RGB to 4xgreyscale - done
    * join 4xgreyscale to RGB - done
    * split complex to planes - done
    * create complex from planes - done
  * add ImageInterface data iterators - done
  * remove data() method from ImageInterface, ImageView - done
  * allow ImageView to be non-const
* algorithms
  * transpose - done
  * normalize - done
  * FFT
    * spectral correlation - done
    * optimize
    * apply kernel in Fourier space
    * use SIMD
  * direct correlation
  * peak detection - done
  * peak fitting
* image processing
  * low/high pass filter
* processing framework
  * grid generator
  * processing
  * marking
* data output
  * ASCII/CSV
  * gnuplot/pyplot?
  * tecplot
* GUI
  * Qt?
  * websockets/HTML5?
  * webassembly + JS/HTML5?
