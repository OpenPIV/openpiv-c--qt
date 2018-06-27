OpenPIV (C++)
=============

An implementation of a PIV analysis engine in C++ using as few dependencies as possible;
the implementation depends on a modern C++17 compliant compiler.

Build
-----

Building currently uses TUP (https://github.com/gittup/tup): a cross-platform rules
based build system. TUP has been chosen for it's speed and simplicity.

Dependencies
------------

* C++17 compiler e.g. clang++-5.0
* libtiff


TODO
----

* logging
* core
  * load/save PNM files - done
  * iostream ops for ImageLoaders
  * shared memory -> Image allocator support
  * utilities
    * split RGB to 4xgreyscale
* algorithms
  * normalize
  * FFT
  * correlation
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
