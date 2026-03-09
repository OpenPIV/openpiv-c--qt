#!/usr/bin/env bash
# Local test script that mimics the GitHub Actions cmake.yml workflow
# Usage: ./scripts/test_cmake_workflow.sh

set -e  # Exit on error

echo "========================================="
echo "Testing CMake workflow locally"
echo "========================================="

# Configuration (matches cmake.yml)
BUILD_TYPE="${BUILD_TYPE:-Release}"
export VCPKG_DISABLE_METRICS=1

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$ROOT_DIR/build"
VCPKG_DIR="$ROOT_DIR/external/vcpkg"

echo ""
echo "Configuration:"
echo "  ROOT_DIR: $ROOT_DIR"
echo "  BUILD_DIR: $BUILD_DIR"
echo "  BUILD_TYPE: $BUILD_TYPE"
echo "  VCPKG_DIR: $VCPKG_DIR"
echo ""

# Step 1: Clean previous build (optional)
echo "========================================="
echo "Step 0: Clean previous build"
echo "========================================="
if [ -d "$BUILD_DIR" ]; then
    echo "Removing existing build directory..."
    rm -rf "$BUILD_DIR"
fi
echo "Done"

# Step 2: Install Linux dependencies (if needed)
echo ""
echo "========================================="
echo "Step 1: Check dependencies"
echo "========================================="
check_command() {
    if command -v "$1" &> /dev/null; then
        echo "✓ $1 found: $(command -v "$1")"
    else
        echo "✗ $1 NOT found"
        MISSING_DEPS="$MISSING_DEPS $1"
    fi
}

check_command cmake
check_command ninja
check_command git
check_command pkg-config

# Check for libtiff development files
if dpkg -l | grep -q libtiff-dev; then
    echo "✓ libtiff found: $(dpkg -l libtiff-dev | grep libtiff-dev | awk '{print $3}')"
elif pkg-config --exists tiff-4; then
    echo "✓ libtiff found: $(pkg-config --modversion tiff-4)"
elif pkg-config --exists tiff; then
    echo "✓ libtiff found: $(pkg-config --modversion tiff)"
else
    echo "✗ libtiff NOT found (install libtiff-dev)"
    MISSING_DEPS="$MISSING_DEPS libtiff-dev"
fi

if [ -n "$MISSING_DEPS" ]; then
    echo ""
    echo "Missing dependencies:$MISSING_DEPS"
    echo ""
    echo "Install with:"
    echo "  sudo apt-get update && sudo apt-get install -y cmake ninja-build libtiff-dev"
    exit 1
fi

# Step 3: Bootstrap vcpkg (if needed)
echo ""
echo "========================================="
echo "Step 2: Bootstrap vcpkg"
echo "========================================="
if [ ! -f "$VCPKG_DIR/vcpkg" ]; then
    echo "Bootstrapping vcpkg..."
    cd "$ROOT_DIR/external/vcpkg"
    ./bootstrap-vcpkg.sh -disableMetrics
    echo "vcpkg bootstrapped successfully"
else
    echo "✓ vcpkg already bootstrapped"
fi

# Step 4: Configure CMake
echo ""
echo "========================================="
echo "Step 3: Configure CMake"
echo "========================================="
cmake -S "$ROOT_DIR" \
      -B "$BUILD_DIR" \
      -G Ninja \
      -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
      "-DCMAKE_TOOLCHAIN_FILE=$VCPKG_DIR/scripts/buildsystems/vcpkg.cmake" \
      "-DVCPKG_MANIFEST_FEATURES=tests;examples"

echo "CMake configuration complete"

# Step 5: Build
echo ""
echo "========================================="
echo "Step 4: Build"
echo "========================================="
cmake --build "$BUILD_DIR" --config "$BUILD_TYPE" --parallel

echo "Build complete"

# Step 6: Test
echo ""
echo "========================================="
echo "Step 5: Test"
echo "========================================="
ctest --test-dir "$BUILD_DIR" -C "$BUILD_TYPE" --output-on-failure

echo ""
echo "========================================="
echo "All tests passed!"
echo "========================================="
