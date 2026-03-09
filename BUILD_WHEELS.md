# Building Binary Wheels with cibuildwheel and Docker

This guide explains how to build manylinux wheels that work on all Linux distributions using Docker and cibuildwheel.

## Quick Start

### Build wheels locally (Linux)

```bash
# Install cibuildwheel
pip install cibuildwheel

# Build all wheels (requires Docker)
cibuildwheel --output-dir wheelhouse

# Build specific platform only
cibuildwheel --platform linux --output-dir wheelhouse
cibuildwheel --platform macos --output-dir wheelhouse
cibuildwheel --platform windows --output-dir wheelhouse
```

### Build with uv (recommended)

```bash
# Create virtual environment
uv venv
source .venv/bin/activate

# Install dependencies
uv pip install cibuildwheel setuptools wheel pybind11

# Build wheels
cibuildwheel --output-dir wheelhouse
```

## How It Works

### manylinux and Docker

cibuildwheel uses Docker containers with [manylinux](https://github.com/pypa/manylinux) images to build wheels that are compatible with most Linux distributions:

| Image | Compatibility | Size |
|-------|--------------|------|
| manylinux2014 | CentOS 7+ (glibc 2.17) | ~500MB |
| manylinux_2_24 | Debian 9+ (glibc 2.24) | ~600MB |
| manylinux_2_28 | RHEL 8+ (glibc 2.28) | ~700MB |

### Build Process

```
┌─────────────────────────────────────────────────────────────┐
│  1. Pull manylinux Docker image                             │
│  2. Install build dependencies (cmake, libtiff, etc.)       │
│  3. Build C++ library inside container                      │
│  4. Compile Python extension with pybind11                  │
│  5. Run auditwheel to fix library paths                     │
│  6. Output: .whl file works on all Linux distros            │
└─────────────────────────────────────────────────────────────┘
```

## Configuration

### pyproject.toml

Key cibuildwheel settings:

```toml
[tool.cibuildwheel]
# Python versions to build
build = ["cp38-*", "cp39-*", "cp310-*", "cp311-*", "cp312-*"]

# Skip incompatible platforms
skip = ["*-win32", "*-manylinux_i686", "pp*"]

[tool.cibuildwheel.linux]
# Use manylinux2014 for broad compatibility
manylinux-x86_64-image = "manylinux2014"
manylinux-aarch64-image = "manylinux2014"

# Install system dependencies
before-all = """
yum install -y cmake ninja-build pkgconfig libtiff-devel libjpeg-devel
"""

# Fix library paths for portability
repair-wheel-command = "auditwheel repair --plat {plat} -w {dest_dir} {wheel}"
```

## Building for Multiple Architectures

### x86_64 (Intel/AMD)

```bash
# Native build (fastest)
cibuildwheel --platform linux --archs x86_64
```

### aarch64 (ARM64)

```bash
# Requires QEMU emulation (slower)
docker run --rm --privileged multiarch/qemu-user-static --reset -p yes
cibuildwheel --platform linux --archs aarch64
```

### universal2 (macOS)

```bash
# Build for both Intel and Apple Silicon
cibuildwheel --platform macos --archs universal2
```

## Testing Wheels

### Test installation

```bash
# Create clean virtual environment
uv venv test_env
source test_env/bin/activate

# Install wheel
uv pip install wheelhouse/openpiv_cpp-0.2.0-*.whl

# Run tests
python -c "import openpiv_cpp_pkg; print('OK')"
python -m pytest tests/
```

### Check wheel compatibility

```bash
# Linux: check manylinux tag
auditwheel show openpiv_cpp-0.2.0-*.whl

# macOS: check minimum version
delocate-listdeps openpiv_cpp-0.2.0-*.whl

# All platforms: inspect wheel
unzip -l openpiv_cpp-0.2.0-*.whl
```

## CI/CD with GitHub Actions

The workflow automatically builds wheels for:

| Platform | Architectures | Python Versions |
|----------|--------------|-----------------|
| Linux (manylinux2014) | x86_64, aarch64 | 3.8 - 3.13 |
| macOS | x86_64, arm64, universal2 | 3.8 - 3.13 |
| Windows | AMD64 | 3.8 - 3.13 |

### Trigger a build

```bash
# Tag a release
git tag v0.2.0
git push origin v0.2.0

# This triggers the workflow which:
# 1. Builds wheels on all platforms
# 2. Uploads to PyPI automatically
# 3. Creates GitHub release with wheel attachments
```

## Troubleshooting

### Build fails with "command not found: cmake"

Add to `before-all`:
```toml
[tool.cibuildwheel.linux]
before-all = "yum install -y cmake"
```

### Wheel not portable (auditwheel fails)

Check for non-portable library links:
```bash
auditwheel show your_wheel.whl
```

Fix by ensuring all dependencies are:
- Included in manylinux image, OR
- Vendored in your wheel

### ARM64 build too slow

Use native hardware or cross-compilation:
```bash
# On ARM64 machine (fast)
cibuildwheel --archs aarch64

# Or skip ARM64 builds
CIBW_SKIP="*aarch64"
```

### macOS delocate fails

Ensure dependencies are installed via brew:
```toml
[tool.cibuildwheel.macos]
before-all = "brew install cmake ninja libtiff"
```

## Wheel Size Optimization

```toml
# Strip debug symbols
[tool.cibuildwheel]
environment = { CMAKE_BUILD_TYPE = "Release" }

# Use LTO for smaller binaries
environment = { CFLAGS = "-flto", CXXFLAGS = "-flto" }
```

## Example: Full Build Command

```bash
# Complete build with all options
CIBW_BUILD="cp38-* cp39-* cp310-* cp311-* cp312-*" \
CIBW_SKIP="*-win32 *-manylinux_i686 pp*" \
CIBW_MANYLINUX_X86_64_IMAGE="manylinux2014" \
CIBW_MANYLINUX_AARCH64_IMAGE="manylinux2014" \
CIBW_BEFORE_ALL_LINUX="yum install -y cmake ninja-build pkgconfig libtiff-devel libjpeg-devel zlib-devel" \
CIBW_TEST_REQUIRES="pytest numpy" \
CIBW_TEST_COMMAND="python -c \"import openpiv_cpp_pkg; print('OK')\"" \
cibuildwheel --output-dir wheelhouse
```

## Resources

- [cibuildwheel documentation](https://cibuildwheel.pypa.io/)
- [manylinux images](https://github.com/pypa/manylinux)
- [auditwheel](https://github.com/pypa/auditwheel)
- [delocate (macOS)](https://github.com/matthew-brett/delocate)
