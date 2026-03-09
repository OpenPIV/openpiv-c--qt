# OpenPIV C++ Python Bindings

Fast Particle Image Velocimetry (PIV) analysis using C++ backend with Python bindings.

## Installation

### From PyPI (recommended)

```bash
# Using pip
pip install openpiv-cpp

# Using uv (faster)
uv pip install openpiv-cpp
```

### From wheel file

```bash
# Using pip
pip install openpiv_cpp-0.2.0-*.whl

# Using uv
uv pip install openpiv_cpp-0.2.0-*.whl
```

### From source

```bash
# Clone repository
git clone --recursive https://github.com/OpenPIV/openpiv-c--qt.git
cd openpiv-c--qt

# Create virtual environment and install with uv
uv venv
source .venv/bin/activate  # On Windows: .venv\Scripts\activate

# Install build dependencies
uv pip install setuptools wheel pybind11

# Build and install
uv pip install .
```

## Quick Start

```python
import openpiv_cpp_pkg as piv

# Process two PIV images
result = piv.piv_process(
    "image1.tif",
    "image2.tif",
    size=32,        # Interrogation window size (pixels)
    overlap=0.5     # 50% overlap
)

# Access results
vectors = result['vectors']  # numpy array: [x, y, u, v, sn]
stats = result['stats']

print(f"Found {stats['vector_count']} vectors")
print(f"Mean speed: {stats['mean_speed']:.2f} pixels")
```

## API Reference

### `piv_process(image1, image2, size=32, overlap=0.5)`

Process a pair of PIV images and return velocity field with statistics.

**Parameters:**
- `image1` (str): Path to first image (TIFF or PNM format)
- `image2` (str): Path to second image
- `size` (int): Interrogation window size in pixels (default: 32)
- `overlap` (float): Overlap fraction between windows (default: 0.5)

**Returns:** dict with keys:
- `vectors`: numpy array of shape (n, 5) with columns [x, y, u, v, sn]
- `count`: number of vectors
- `image_size`: [width, height] of input images
- `ia_size`: interrogation window size
- `overlap`: overlap fraction
- `stats`: dictionary with velocity statistics

### `cross_correlate(image1, image2, size=32, overlap=0.5, fft_type="complex")`

Low-level cross-correlation function with FFT method selection.

**Parameters:**
- `fft_type` (str): FFT algorithm to use:
  - `"complex"`: Complex FFT (default)
  - `"real"`: Real-valued FFT (faster for real data)
  - `"pocket"`: PocketFFT complex (alternative implementation)
  - `"pocket_real"`: PocketFFT real (fastest)

## Example with Visualization

```python
import openpiv_cpp_pkg as piv
import numpy as np
import matplotlib.pyplot as plt

# Process images
result = piv.piv_process("img1.tif", "img2.tif", size=32)

# Extract data
vectors = result['vectors']
x, y, u, v, sn = vectors.T
speed = np.sqrt(u**2 + v**2)

# Create quiver plot
fig, ax = plt.subplots(figsize=(10, 6))
q = ax.quiver(x, y, u, v, speed, cmap='viridis', scale=50)
ax.set_xlabel('X (pixels)')
ax.set_ylabel('Y (pixels)')
ax.set_title(f'Velocity Field (speed: {speed.min():.2f}-{speed.max():.2f} px)')
ax.set_aspect('equal')
plt.colorbar(q, label='Speed (px)')
plt.savefig('piv_result.png', dpi=150)
plt.show()
```

## Performance Comparison

Typical processing times for 1008×1012 pixel images (32×32 windows, 50% overlap):

| FFT Method | Time | Vectors |
|------------|------|---------|
| complex | ~380 ms | 3837 |
| real | ~330 ms | 3843 |
| pocket | ~300 ms | 3837 |
| pocket_real | ~240 ms | 3837 |

## Output Format

The `vectors` array contains columns:
- `x`: X position (pixels)
- `y`: Y position (pixels, image coordinates)
- `u`: Horizontal velocity component (pixels)
- `v`: Vertical velocity component (pixels)
- `sn`: Signal-to-noise ratio (peak1/peak2)

## Building Wheels

For developers wanting to build distribution wheels:

```bash
# Using uv (recommended - faster)
uv venv
source .venv/bin/activate
uv pip install build setuptools wheel pybind11
python -m build --wheel

# Or using pip only
pip install build
python -m build --wheel

# Build for multiple platforms (requires Docker)
uv pip install cibuildwheel
python -m cibuildwheel --output-dir wheelhouse
```

## Requirements

- Python 3.8+
- NumPy 1.20+
- C++17 compiler (for building from source)

## License

GPL-3.0-or-later

## Citation

If you use this software in your research, please cite:
- OpenPIV: https://www.openpiv.net/
