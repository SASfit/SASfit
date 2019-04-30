#!/bin/sh

echo
echo "## Starting build ..."
echo

eval "$(wmic cpu get /format:list | grep NumberOfLogicalProcessors)"

cd "$APPVEYOR_BUILD_FOLDER" && \
mkdir build && cd build && cmake ../src
echo
echo "## Setting AppVeyor build version to '$SASFIT_VERSION'"
echo
appveyor UpdateBuild -Version "$SASFIT_VERSION"
echo "NumberOfLogicalProcessors: $NumberOfLogicalProcessors"
make -j$NumberOfLogicalProcessors

# vim: set ts=2 sw=2 sts=2 tw=0:
