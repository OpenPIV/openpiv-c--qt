# Local Testing Scripts for GitHub Actions

This directory contains scripts to test GitHub Actions workflows locally before pushing.

## cmake.yml Workflow Testing

### Option 1: Run directly on your system (requires dependencies)

```bash
# Install dependencies (Ubuntu/Debian)
sudo apt-get update && sudo apt-get install -y cmake ninja-build libtiff-dev

# Run the test script
./scripts/test_cmake_workflow.sh
```

### Option 2: Run in Docker (recommended - matches GitHub Actions environment)

```bash
# Run in Docker (no need to install dependencies locally)
./scripts/test_cmake_docker.sh
```

This uses `ubuntu:22.04` which matches GitHub's `ubuntu-latest` runner.

### Option 3: Use `act` (GitHub Actions runner emulator)

```bash
# Install act: https://github.com/nektos/act
# Run the CMake workflow
act pull_request -j build --container-architecture linux/amd64

# Or run with verbose output
act pull_request -j build -v
```

## What the scripts do

Both scripts perform the same steps as the GitHub Actions workflow:

1. **Clean** - Remove previous build artifacts
2. **Dependencies** - Install cmake, ninja, libtiff-dev
3. **Bootstrap vcpkg** - Initialize vcpkg package manager
4. **Configure CMake** - Set up build with vcpkg toolchain
5. **Build** - Compile the project
6. **Test** - Run ctest

## Troubleshooting

### CMake configuration fails

Make sure vcpkg submodules are initialized:
```bash
git submodule update --init --recursive
```

### Tests fail

Check the build output for compiler errors. Common issues:
- Missing dependencies (libtiff-dev)
- C++17 compiler support
- vcpkg package installation failures

### Docker permission denied

Add your user to the docker group:
```bash
sudo usermod -aG docker $USER
# Then log out and back in
```

## Files

- `test_cmake_workflow.sh` - Direct system test script
- `test_cmake_docker.sh` - Docker wrapper script
- `Dockerfile.cmake-test` - Docker image definition (ubuntu:22.04 + dependencies)
