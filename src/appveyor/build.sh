#!/bin/sh

echo
echo "## Starting build ..."
echo

NUM_LOGICAL_CORES=1
# determine number of computing cores on Windows
WMIC="$(command -v wmic)"
if [ -x "$WMIC" ]; then
    NUM_LOGICAL_CORES="$($WMIC cpu get /format:list | grep NumberOfLogicalProcessors | cut -d= -f2)"
fi
export NUM_LOGICAL_CORES
echo "$0 determined $NUM_LOGICAL_CORES logical cores."

cd "$APPVEYOR_BUILD_FOLDER" && \
mkdir build && cd build && cmake ../src && make -j"$NUM_LOGICAL_CORES"

# vim: set ts=2 sw=2 sts=2 tw=0:
