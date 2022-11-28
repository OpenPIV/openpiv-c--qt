![Build Status](https://github.com/OpenPIV/openpiv-c--qt/actions/workflows/cmake.yml/badge.svg)

# OpenPIV (c++)

An implementation of a PIV analysis engine in C++ using as few dependencies as possible;
the implementation requires a c++17 compliant compiler.

## Build

There are some external dependencies under external/, so when cloning use:

```git clone --recursive <path to git repo>```

Building uses cmake and vcpkg, and is simplified by using a vcpkg manifest to specify
the dependent packages. Vcpkg has some pre-requisites:

* a compiler (e.g. `apt install build-essentials`)
* cmake
* git
* (linux) pkg-config (e.g. `apt install pkg-config`)
* curl, zip, unzip, tar (e.g. `apt install curl zip unzip tar`)
* ninja (e.g. `apt install ninja-build`)

To build:

* `cmake -B build -S .`
* `cmake --build build`

To run tests:

* `cd build`
* `ctest`

To change the build type, add `-DCMAKE_BUILD_TYPE` e.g.
`cmake -DCMAKE_BUILD_TYPE=RelWithDebugInfo -B build -S .`.

The binaries are located in the build directory:

* build
  * test -> *_test
  * examples
    * process
    * average_subtract
  * openpiv -> libopenpivcore.so

### Python Bindings

Python bindings are made through pybind11. Building python modules appears to be a non-trivial
subject, however as openpivcore is based on vcpkg and cmake, a setup.py is provided to
build the core library as well as the python bindings using cmake.

Although vcpkg provides pybind11, this is not used as it has a dependency on vcpkg's python; the
aim is to allow the python bindings to work with the user's installed version of python. As such,
the supplied `pyproject.toml` specifies pybind11 as a required dependency.

To build using pip/setuptools:

* create a venv at the same level as the openpivcore directory
* activate the venv
* run pip against the name of the directory containing the openpiv code e.g. `pip install <path to source>`
  * a clean step should happen as part of the build, this can also be run manually with `python setup.py clean`

To build using cmake:

* install pybind11 using pip
* specify the location of the installed pybind11 libs/cmake modules using `-DCMAKE_PREFIX_PATH=<path to pybind11>`
  * you can use pip & sed to find this: `pip show pybind11 | sed -n '/^Location/ s/.* \(.*\)/\1/ p'`
* specify that the python bindings should be built: `-DBUILD_PYBIND=ON`

e.g.

```
cmake -B build -S . -DBUILD_PYBIND=ON -DCMAKE_PREFIX_PATH=$(pip show pybind11 | sed -n '/^Location/ s/.* \(.*\)/\1/ p')
```

### Raspberry Pi

Build times are, as expected, much slower than on a modern Intel CPU, but the code
will compile. Some observations:

* `VCPKG_FORCE_SYSTEM_BINARIES=1` must be set
* `VCPKG_MAX_CONCURRENCY=<num cores>` can help if you find you get kernel panics
  due to poor cooling

Performance isn't terrible: on a raspberry pi model 3B:

```
$ lscpu
Architecture:                    aarch64
CPU op-mode(s):                  32-bit, 64-bit
Byte Order:                      Little Endian
CPU(s):                          4
On-line CPU(s) list:             0-3
Thread(s) per core:              1
Core(s) per socket:              4
Socket(s):                       1
Vendor ID:                       ARM
Model:                           4
Model name:                      Cortex-A53
Stepping:                        r0p4
CPU max MHz:                     1200.0000
CPU min MHz:                     1200.0000
BogoMIPS:                        38.40
Vulnerability Itlb multihit:     Not affected
Vulnerability L1tf:              Not affected
Vulnerability Mds:               Not affected
Vulnerability Meltdown:          Not affected
Vulnerability Spec store bypass: Not affected
Vulnerability Spectre v1:        Mitigation; __user pointer sanitization
Vulnerability Spectre v2:        Not affected
Vulnerability Srbds:             Not affected
Vulnerability Tsx async abort:   Not affected
Flags:                           fp asimd evtstrm crc32 cpuid

$ time ./process -s 32 -i ../../../examples/images/F_00001.tif ../../../examples/images/F_00002.tif > out.piv
[15:59:23 +00:00] [info] [thread 40843] size: 32 x 32
[15:59:23 +00:00] [info] [thread 40843] overlap: 0.5
[15:59:23 +00:00] [info] [thread 40843] input files: [../../../examples/images/F_00001.tif, ../../../examples/images/F_00002.tif]
[15:59:23 +00:00] [info] [thread 40843] execution: pool
[15:59:23 +00:00] [info] [thread 40843] loaded images: [1024,1024]
[15:59:23 +00:00] [info] [thread 40843] generated grid for image size: [1024,1024], ia: [32,32] (50% overlap)
[15:59:23 +00:00] [info] [thread 40843] grid count: 3969
[15:59:23 +00:00] [info] [thread 40843] processing using thread pool

real    0m1.248s
user    0m3.249s
sys     0m0.109s
```

This is about 1ms per interrogation area (3 cores, 3969 interrogation areas, 1.248s).
For comparison, on my laptop:

```
$ lscpu
Architecture:                    x86_64
CPU op-mode(s):                  32-bit, 64-bit
Byte Order:                      Little Endian
Address sizes:                   39 bits physical, 48 bits virtual
CPU(s):                          8
On-line CPU(s) list:             0-7
Thread(s) per core:              2
Core(s) per socket:              4
Socket(s):                       1
NUMA node(s):                    1
Vendor ID:                       GenuineIntel
CPU family:                      6
Model:                           126
Model name:                      Intel(R) Core(TM) i7-1065G7 CPU @ 1.30GHz
Stepping:                        5
CPU MHz:                         1500.000
CPU max MHz:                     3900.0000
CPU min MHz:                     400.0000
BogoMIPS:                        2995.20
Virtualisation:                  VT-x
L1d cache:                       192 KiB
L1i cache:                       128 KiB
L2 cache:                        2 MiB
L3 cache:                        8 MiB
NUMA node0 CPU(s):               0-7
Vulnerability Itlb multihit:     KVM: Mitigation: Split huge pages
Vulnerability L1tf:              Not affected
Vulnerability Mds:               Not affected
Vulnerability Meltdown:          Not affected
Vulnerability Spec store bypass: Mitigation; Speculative Store Bypass disabled via prctl and seccomp
Vulnerability Spectre v1:        Mitigation; usercopy/swapgs barriers and __user pointer sanitization
Vulnerability Spectre v2:        Mitigation; Enhanced IBRS, IBPB conditional, RSB filling
Vulnerability Srbds:             Not affected
Vulnerability Tsx async abort:   Not affected
Flags:                           fpu vme de pse tsc msr pae mce cx8 apic sep mtrr pge mca cmov pat pse36 clflush dts acpi mmx fxsr sse sse2 ss
                                 ht tm pbe syscall nx pdpe1gb rdtscp lm constant_tsc art arch_perfmon pebs bts rep_good nopl xtopology nonstop_
                                 tsc cpuid aperfmperf tsc_known_freq pni pclmulqdq dtes64 monitor ds_cpl vmx est tm2 ssse3 sdbg fma cx16 xtpr p
                                 dcm pcid sse4_1 sse4_2 x2apic movbe popcnt tsc_deadline_timer aes xsave avx f16c rdrand lahf_lm abm 3dnowprefe
                                 tch cpuid_fault epb invpcid_single ssbd ibrs ibpb stibp ibrs_enhanced tpr_shadow vnmi flexpriority ept vpid ep
                                 t_ad fsgsbase tsc_adjust sgx bmi1 avx2 smep bmi2 erms invpcid avx512f avx512dq rdseed adx smap avx512ifma clfl
                                 ushopt intel_pt avx512cd sha_ni avx512bw avx512vl xsaveopt xsavec xgetbv1 xsaves split_lock_detect dtherm ida
                                 arat pln pts hwp hwp_notify hwp_act_window hwp_epp hwp_pkg_req avx512vbmi umip pku ospke avx512_vbmi2 gfni vae
                                 s vpclmulqdq avx512_vnni avx512_bitalg avx512_vpopcntdq rdpid sgx_lc fsrm md_clear flush_l1d arch_capabilities

$ time ./process -s 32 -i ../../../examples/images/F_00001.tif ../../../examples/images/F_00002.tif > out.piv
[16:06:56 +00:00] [info] [thread 378898] size: 32 x 32
[16:06:56 +00:00] [info] [thread 378898] overlap: 0.5
[16:06:56 +00:00] [info] [thread 378898] input files: [../../../examples/images/F_00001.tif, ../../../examples/images/F_00002.tif]
[16:06:56 +00:00] [info] [thread 378898] execution: pool
[16:06:56 +00:00] [info] [thread 378898] loaded images: [1024,1024]
[16:06:56 +00:00] [info] [thread 378898] generated grid for image size: [1024,1024], ia: [32,32] (50% overlap)
[16:06:56 +00:00] [info] [thread 378898] grid count: 3969
[16:06:56 +00:00] [info] [thread 378898] processing using thread pool

real    0m0.122s
user    0m0.649s
sys     0m0.020s
```

This is about 230us per interrogation area (7 cores, 3696 interrogation areas, 0.122s)

## Dependencies

These are captured in `vcpkg.json`:

* c++17 compiler e.g. clang++-5.0, gcc7
* [vcpkg](https://github.com/Microsoft/vcpkg)
  * catch2: unit test framework
  * libtiff: TIFF IO support
  * benchmark: used to run performance benchmarks
  * async++ (optional): implements c++17 parallel algorithms
  * cxxopts: nice command line parsing
  * pybind11

## Examples

* under build/examples are two simple applications:
  * [process](examples/process/README.md): a straight-forward PIV cross correlator
  * [average_subtract](examples/average_subtract/README.md): a utility to read in n
    images, find the average and write out n new images with the mean subtracted

# TODO

* build
  * [x] travis/github actions/CI
  * [x] add clang/windows/OS X build
  * [ ] python bindings
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
    * [x] optimize
    * [ ] openCL
    * [ ] apply kernel in Fourier space
    * [x] use SIMD?
    * [x] real -> complex FFT/correlation of real data
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
