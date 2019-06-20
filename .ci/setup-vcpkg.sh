#!/usr/bin/env bash

# exit on non-zero return, echo all lines
set -ev

# setup vcpkg and install required packages
required="catch2 tiff spdlog benchmark"

pushd ./external/vcpkg
./bootstrap-vcpkg.sh
./vcpkg install ${required}
