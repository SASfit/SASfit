#!/bin/sh

echo
echo "## Starting build ..."
echo

eval "export $(wmic cpu get /format:list | grep NumberOfLogicalProcessors)"
[ -z "$NumberOfLogicalProcessors" ] || export NUM_LOGICAL_CORES=$NumberOfLogicalProcessors
echo "Using $NUM_LOGICAL_CORES logical cores."

cd "$APPVEYOR_BUILD_FOLDER" && \
mkdir build && cd build && cmake ../src

env | sort

echo
echo "## Setting AppVeyor build version to '$SASFIT_VERSION'"
echo
appveyor UpdateBuild -Version "$SASFIT_VERSION"
make -j$NUM_LOGICAL_CORES

# vim: set ts=2 sw=2 sts=2 tw=0:
