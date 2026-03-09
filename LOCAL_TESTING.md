# GitHub Actions Local Testing Guide

## Summary

This document explains how to test GitHub Actions workflows locally before pushing to GitHub.

## Workflows

### 1. cmake.yml (CMake Build & Test)

**Status**: ✅ Works locally with `act`

**What it does**:
- Builds the C++ library using CMake + vcpkg
- Runs 18 unit tests with ctest

**How to run locally**:

```bash
# Using act (recommended - exact GitHub environment)
act pull_request -j build --container-architecture linux/amd64 \
  -P ubuntu-22.04=catthehacker/ubuntu:act-22.04

# Expected output: "100% tests passed, 0 tests failed out of 18"
```

**Alternative methods**:
```bash
# Using Docker (matches ubuntu-latest environment)
./scripts/test_cmake_docker.sh

# Direct on your system (requires dependencies)
./scripts/test_cmake_workflow.sh
```

---

### 2. wheels.yml (Python Wheel Build)

**Status**: ✅ Works locally with `cibuildwheel`

**What it does**:
- Builds Python wheels for multiple platforms (Linux, macOS, Windows)
- Uses auditwheel/delocate to bundle dependencies

**How to run locally**:

```bash
# Build a single wheel for testing (fastest)
uvx cibuildwheel --output-dir wheelhouse --only cp311-manylinux_x86_64

# Build all Linux wheels
uvx cibuildwheel --output-dir wheelhouse --platform linux

# Expected output: "1 wheel produced in X minutes"
```

**Important fixes applied**:
1. Changed `{plat}` to `manylinux2014_${{ matrix.arch }}` in `CIBW_REPAIR_WHEEL_COMMAND_LINUX`
2. This fix was discovered through local testing with `act` and `cibuildwheel`

---

## Tools

### act (GitHub Actions Runner)

Install: https://github.com/nektos/act

```bash
# Run specific job
act <event> -j <job-name>

# Example
act pull_request -j build

# Use specific Ubuntu image
act pull_request -j build -P ubuntu-22.04=catthehacker/ubuntu:act-22.04

# Verbose output for debugging
act pull_request -j build -v
```

### cibuildwheel (Python Wheel Builder)

Install: `pip install cibuildwheel` or use `uvx cibuildwheel`

```bash
# Build specific wheel
cibuildwheel --only cp311-manylinux_x86_64

# List available builds
cibuildwheel --print-build-identifiers
```

### Docker

For cmake.yml testing with exact GitHub environment:

```bash
# Build test image
docker build -f scripts/Dockerfile.cmake-test -t openpiv-cmake-test .

# Run tests
docker run --rm -v "$(pwd)":/workspace -w /workspace openpiv-cmake-test \
  ./scripts/test_cmake_workflow.sh
```

---

## Scripts

| Script | Purpose | Requirements |
|--------|---------|--------------|
| `scripts/test_cmake_workflow.sh` | Direct CMake test | cmake, ninja, libtiff-dev |
| `scripts/test_cmake_docker.sh` | Docker-based CMake test | Docker |
| `scripts/Dockerfile.cmake-test` | Docker image definition | Docker |

---

## Common Issues & Solutions

### cmake.yml fails in CI

**Run locally first**:
```bash
act pull_request -j build -P ubuntu-22.04=catthehacker/ubuntu:act-22.04
```

**Common fixes**:
- Missing vcpkg: `git submodule update --init --recursive`
- Missing dependencies: Install cmake, ninja, libtiff-dev
- CMake errors: Check `CMakeCache.txt` and clean build directory

### wheels.yml fails in CI

**Run locally first**:
```bash
uvx cibuildwheel --only cp311-manylinux_x86_64
```

**Common fixes**:
- `{plat}` not expanding: Use explicit platform name
- Build artifacts conflict: Clean `builddir/`, `build/`, `*.so` files
- auditwheel errors: Check library dependencies with `ldd`

---

## Quick Reference

```bash
# Test CMake workflow (3 minutes)
act pull_request -j build -P ubuntu-22.04=catthehacker/ubuntu:act-22.04

# Test wheel build (10 minutes for single wheel)
uvx cibuildwheel --output-dir wheelhouse --only cp311-manylinux_x86_64

# Clean build artifacts
rm -rf build/ builddir/ wheelhouse/ openpiv_cpp_pkg/*.so
```

---

## Files Modified for Local Testing

1. `.github/workflows/wheels.yml` - Fixed `{plat}` placeholder issue
2. `scripts/test_cmake_workflow.sh` - Local CMake test script
3. `scripts/test_cmake_docker.sh` - Docker wrapper
4. `scripts/Dockerfile.cmake-test` - Docker image definition
5. `scripts/README.md` - Scripts documentation
