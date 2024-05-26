#!/bin/sh

echo
echo "## Installing build tools on:"
echo
sw_vers
echo

# env | sort

set -x

export HOMEBREW_NO_INSTALL_CLEANUP=1
brew update
brew install --overwrite cmake gcc@13 gsl fftw zlib # GCC for xquartz libstc++ compat.
brew install --cask xquartz

# vim: set ts=4 sw=4 sts=4 tw=0 et:
