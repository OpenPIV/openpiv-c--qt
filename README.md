Installation instructions on Mac OS X (Snow Leopard)
====================================================

1. Download and install Qt SDK (full SDK with Qt Creator)
2. Download and install libtiff library. The steps are:
 * download and unzip the tiff-4.0.0.tar.gz
 * ./configure CPPFLAGS='-I/usr/X11/include -I/opt/local/include' LIBS='-lGL -lGLU' LDFLAGS='-L/usr/X11/lib -L/opt/local/lib'
 * make
 * sudo make install
3. Download and install fftw3 library, follow their great instructions in INSTALL file (I had to use --disable-fortran to make it working)
