#!/usr/bin/bash

if [ "${TRAVIS_OS_NAME}" = "windows" ]; then
  choco install visualstudio2019buildtools --package-parameters \
    "--add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 --add Microsoft.VisualStudio.Component.Windows10SDK"
  choco upgrade cmake
fi
