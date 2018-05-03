* Note that these are guidelines that worked for us to install the
  software on various platforms.  Please do not hesitate to contact us
  at: if you require assistance on your specific platform.  Thanks!

Installation instructions on Ubuntu based Linux
===============================================

1. Get pre-requisites
 * Download and install Qt4 for developers: apt-get install qt4-default
 * Download and install libtiff for developers: apt-get install libtiff5-dev
 * Download and install fftw3 for developers: apt-get install libfftw3-*
2. Compile OpenPIV:
 * qmake; make
3. Run OpenPIV

Installations instructions for general Linux
============================================

1. Get pre-requisites
 * Download and install Qt4 for developers:
 * Download and install libtiff4 for developers:
 * Download and install fftw3 for developers:
2. Compile OpenPIV:
 * in the source directory you need to run 'qmake' (or something like 'qmake-qt4', depending on how your distro works), followed by 'make'. 
3. Run OpenPIV

Note
----

Our C++ version is currently under development, but we hope to release an Alpha version of the software soon.  
You can access our pre-Alpha version here: https://github.com/OpenPIV/openpiv-c--qt.  

If somebody is interested in contributing to the development, we would be more than happy to find a project.

Installation instructions on Mac OS X (Snow Leopard)
====================================================

1. Download and install Qt SDK
2. Download and install libtiff library. The steps are:
 * download and unzip the tiff-4.0.0.tar.gz
 * ./configure CPPFLAGS='-I/usr/X11/include -I/opt/local/include' LIBS='-lGL -lGLU' LDFLAGS='-L/usr/X11/lib -L/opt/local/lib'
 * make
 * sudo make install
3. Download and install fftw3 library, follow their great instructions in INSTALL file (I had to use --disable-fortran to make it working)


Installation instructions for Windows
=====================================

1. Get pre-requisites
 * Download and install Qt4 for developers:
 * Download and install libtiff4 for developers:
 * Download and install fftw3 for developers:
2. Compile OpenPIV
3. Run OpenPIV
