#!/usr/bin/env bash

# exit on non-zero return, echo all lines
set -ev

# setup vcpkg and install required packages
required="catch2 tiff spdlog benchmark asyncplusplus cxxopts"

cd ./external/vcpkg

if [ "${TRAVIS_OS_NAME}" = "linux" ] || [ "${TRAVIS_OS_NAME}" = "osx" ]; then
  ./bootstrap-vcpkg.sh
else
  cmd "/C bootstrap-vcpkg.bat"
fi

./vcpkg install ${required}
