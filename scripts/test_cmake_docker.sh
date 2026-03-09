#!/usr/bin/env bash
# Run cmake.yml workflow test in Docker (no sudo required)
# Mimics GitHub Actions ubuntu-latest environment
#
# Usage: ./scripts/test_cmake_docker.sh

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"

echo "========================================="
echo "Testing CMake workflow in Docker"
echo "========================================="
echo ""
echo "This mimics the GitHub Actions ubuntu-latest environment"
echo ""

# Check if Docker is available
if ! command -v docker &> /dev/null; then
    echo "ERROR: Docker is not installed or not in PATH"
    echo "Install Docker: https://docs.docker.com/get-docker/"
    exit 1
fi

echo "Docker version: $(docker --version)"
echo ""

# Build the test image
echo "Building Docker image..."
docker build -f "$SCRIPT_DIR/Dockerfile.cmake-test" \
             -t openpiv-cmake-test:latest \
             "$ROOT_DIR"

echo ""
echo "Running tests in container..."
echo ""

# Run the test script in the container
# Mount the project directory as a volume
docker run --rm \
    -v "$ROOT_DIR":/workspace:rw \
    -w /workspace \
    openpiv-cmake-test:latest \
    ./scripts/test_cmake_workflow.sh

echo ""
echo "========================================="
echo "Docker test complete!"
echo "========================================="
