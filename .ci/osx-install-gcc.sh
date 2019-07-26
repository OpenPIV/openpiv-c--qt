#!/usr/bin/env bash

# exit on non-zero return, echo all lines
set -ev

if [ "${TRAVIS_OS_NAME}" = "osx" ]; then
  # Update command line tool to avoid an error:
  # "_stdio.h: No such file or directory", when building samtools.
  softwareupdate --list
  softwareupdate --install "Command Line Tools (macOS High Sierra version 10.13) for Xcode-10.1"
  brew install gcc@7
fi
