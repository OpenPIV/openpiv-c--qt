OpenPIV (C++)
=============

An implementation of a PIV analysis engine in C++ using as few dependencies as possible;
the implementation depends on a modern C++17 compliant compiler.

Build
-----

Building uses cmake; to build (linux example):

* mkdir build
* cd build
* cmake ..
* make

Dependencies
------------

* C++17 compiler e.g. clang++-5.0
* libtiff (http://libtiff.org/)
* Catch2 (https://github.com/catchorg/Catch2)


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
  * FFT - done
    * spectral correlation
    * optimize
  * direct correlation
  * peak detection
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
