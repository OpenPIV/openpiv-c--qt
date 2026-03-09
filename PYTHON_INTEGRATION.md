# Calling OpenPIV C++ from Python

There are **4 main approaches** to call the C++ PIV code from Python:

## Option 1: Subprocess (Easiest - No Compilation)

Call the existing command-line tools directly from Python.

```python
import subprocess
import numpy as np

def piv_process(image1, image2, size=32, overlap=0.5):
    """Run PIV processing using the C++ binary."""
    cmd = [
        './builddir/examples/process/process',
        '-s', str(size),
        '-o', str(overlap),
        '-i', image1, image2
    ]
    result = subprocess.run(cmd, capture_output=True, text=True)
    
    # Parse output
    data = []
    for line in result.stdout.strip().split('\n'):
        if line and not line.startswith('['):
            x, y, u, v, sn = map(float, line.split(','))
            data.append([x, y, u, v, sn])
    return np.array(data)

# Usage
vectors = piv_process('image1.tif', 'image2.tif')
```

**Pros:** No extra compilation, works immediately  
**Cons:** Slower (file I/O overhead), limited flexibility

---

## Option 2: pybind11 (Recommended - Fast & Clean)

Create Python bindings for the C++ library.

### Install pybind11
```bash
source .venv/bin/activate
uv pip install pybind11
```

### Create Python Module (`piv_bindings.cpp`)
```cpp
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include "openpiv/core/image.h"
#include "openpiv/algos/fft.h"
#include "openpiv/loaders/image_loader.h"

namespace py = pybind11;
using namespace openpiv;

py::dict cross_correlate(const std::string& file1, const std::string& file2, 
                         int size = 32, double overlap = 0.5) {
    // Load images
    std::ifstream is1(file1, std::ios::binary);
    std::ifstream is2(file2, std::ios::binary);
    
    auto loader = core::image_loader_registry::find(is1);
    core::gf_image img1, img2;
    loader->load(is1, img1);
    loader->load(is2, img2);
    
    // Create grid
    auto ia = core::size{size, size};
    auto grid = core::generate_cartesian_grid(img1.size(), ia, overlap);
    
    // Process
    algos::FFT fft{ia};
    std::vector<std::tuple<double, double, double, double, double>> results;
    
    for (const auto& rect : grid) {
        auto view1 = core::extract(img1, rect);
        auto view2 = core::extract(img2, rect);
        auto corr = fft.cross_correlate(view1, view2);
        
        auto peaks = core::find_peaks(corr, 2, 1);
        if (peaks.size() >= 2) {
            auto loc = core::fit_simple_gaussian(peaks[0]);
            double sn = peaks[0][{1,1}] / peaks[1][{1,1}];
            
            auto bl = rect.bottomLeft();
            auto mid = rect.midpoint();
            double u = mid[0] - (bl[0] + loc[0]);
            double v = mid[1] - (bl[1] + loc[1]);
            
            results.emplace_back(mid[0], img1.height() - mid[1], u, v, sn);
        }
    }
    
    // Convert to numpy arrays
    auto arr = py::array_t<double>(results.size() * 5);
    auto buf = arr.mutable_unchecked<1>();
    for (size_t i = 0; i < results.size(); i++) {
        buf[i*5 + 0] = std::get<0>(results[i]);
        buf[i*5 + 1] = std::get<1>(results[i]);
        buf[i*5 + 2] = std::get<2>(results[i]);
        buf[i*5 + 3] = std::get<3>(results[i]);
        buf[i*5 + 4] = std::get<4>(results[i]);
    }
    arr.resize({results.size(), 5});
    
    return py::dict("vectors"_a = arr, "count"_a = results.size());
}

PYBIND11_MODULE(openpiv_cpp, m) {
    m.doc() = "OpenPIV C++ bindings";
    m.def("cross_correlate", &cross_correlate, "Cross-correlate two images");
}
```

### Compile (`setup.py`)
```python
from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup

ext_modules = [
    Pybind11Extension(
        "openpiv_cpp",
        ["piv_bindings.cpp"],
        include_dirs=["openpiv", "external/pocketfft"],
        libraries=["openpivcore"],
        library_dirs=["builddir/openpiv"],
        cxx_std=17,
    ),
]

setup(
    name="openpiv_cpp",
    ext_modules=ext_modules,
    cmdclass={"build_ext": build_ext},
)
```

```bash
python setup.py build_ext --inplace
```

### Use from Python
```python
import openpiv_cpp

result = openpiv_cpp.cross_correlate('image1.tif', 'image2.tif')
vectors = result['vectors']  # numpy array: [x, y, u, v, sn]
```

**Pros:** Fast (direct memory access), clean API, type-safe  
**Cons:** Requires compilation, C++ knowledge

---

## Option 3: ctypes (No Extra Dependencies)

Load the shared library directly.

```python
import ctypes
import numpy as np

# Load library
lib = ctypes.CDLL('./builddir/openpiv/libopenpiv.so')

# Define function signatures (requires C wrapper in C++ code)
lib.piv_process.argtypes = [
    ctypes.c_char_p, ctypes.c_char_p,
    ctypes.c_int, ctypes.c_double,
    ctypes.POINTER(ctypes.c_double), ctypes.c_int
]
lib.piv_process.restype = ctypes.c_int

def piv_process(image1, image2, size=32, overlap=0.5):
    # Allocate output buffer
    max_vectors = 100000
    output = (ctypes.c_double * (max_vectors * 5))()
    
    count = lib.piv_process(
        image1.encode(), image2.encode(),
        size, overlap, output, max_vectors
    )
    
    # Convert to numpy
    arr = np.array(output[:count*5]).reshape(-1, 5)
    return arr
```

**Pros:** No extra dependencies  
**Cons:** Requires C wrapper, manual memory management

---

## Option 4: Cython (Python-like Syntax)

Write `.pyx` files that compile to C++.

```cython
# openpiv.pyx
# distutils: language = c++
# distutils: sources = openpiv/core/image.cpp openpiv/algos/fft.cpp

from libcpp.vector cimport vector
from libcpp.string cimport string

cdef extern from "openpiv/algos/fft.h" namespace "openpiv::algos":
    cdef cppclass FFT:
        FFT(core.size&)
        
cdef extern from "openpiv/core/image.h" namespace "openpiv::core":
    cdef cppclass gf_image:
        pass

def cross_correlate(str image1, str image2, int size=32):
    # Cython code calling C++
    pass
```

**Pros:** Python-like syntax, good for wrapping complex APIs  
**Cons:** Learning curve, additional build step

---

## Comparison Table

| Method | Speed | Difficulty | Flexibility | Best For |
|--------|-------|------------|-------------|----------|
| **Subprocess** | ⭐⭐ | Easy | Low | Quick scripts, prototyping |
| **pybind11** | ⭐⭐⭐⭐⭐ | Medium | High | Production, libraries |
| **ctypes** | ⭐⭐⭐⭐ | Medium | Medium | Simple APIs, no deps |
| **Cython** | ⭐⭐⭐⭐⭐ | Hard | High | Complex wrapping |

---

## Recommendation

**Start with Option 1 (Subprocess)** for quick testing, then move to **Option 2 (pybind11)** for production use.

Would you like me to implement any of these approaches?
