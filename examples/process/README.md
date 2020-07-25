Example PIV Processor
=====================

`process` is a simple PIV processing application i.e. it will take an image (or pair of images),
generate an appropriate rectangular grid, perform correlations at each location, find a sub-pixel
peak and then write the data out.

To use, only image pairs are required:

```sh
./process -i exp1_001_a.tiff exp1_002_b.tiff

registered image/x-portable-anymap
registered image/tiff
size: 32x32
overlap: 0.5
input files: [../../../examples/process/exp1_001_a.tiff, ../../../examples/process/exp1_001_b.tiff]
execution: async++
loaded images: [511,369]
23, 353, -0.180496, -5.23265
263, 353, 0.130522, -4.99688
439, 257, -0.673298, -5.62078
471, 305, -0.24143, -6.81816
...
```

Some things to note:

* all data is written to standard out; you can redirect this into a file if required
* the default processing parameters are a 32x32 window with 50% overlap
* to get a list of options: `./process --help`
* procesing is by default multi-threaded; there are two options:
  * `async++`: this uses the async++ library to get c++20 like parallel processing
  * `pool`: this uses a thread pool that uses (#cores - 1) threads
  * `pool` is slightly faster
* you can plot the data in gnuplot by capturing to `out.piv` and `gnuplot> plot "out.piv" using 1:2:3:4 with vectors head filled lt 2`
  * gnuplot is pretty tolerant of the leading comments!

