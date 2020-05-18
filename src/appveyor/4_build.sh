#!/bin/sh

echo
echo "## Starting build on '$(uname -s)' ..."
echo

NUM_LOGICAL_CORES=1
CMAKE_GENERATOR='Unix Makefiles'

if uname -s | grep -qi '^mingw64'; # Windows
then
    WMIC="$(command -v wmic)"
    if [ -x "$WMIC" ]; then
        NUM_LOGICAL_CORES="$($WMIC cpu get /format:list \
            | grep NumberOfLogicalProcessors \
            | cut -d= -f2 \
            | tr -d '[:space:]' )"
    fi
    CMAKE_GENERATOR='MSYS Makefiles'
else # macOS or Linux
    # determine number of computing cores
    if [ "$(uname -s)" = "Darwin" ]; then # on macOS
        NUM_LOGICAL_CORES="$(sysctl -n hw.ncpu)"
    else # on Linux
        NUM_LOGICAL_CORES="$(awk '/processor/' /proc/cpuinfo | wc -l)"
    fi
    # find latest gcc and g++ compilers and set them as global variables
    export CC=$(which $(echo $PATH | tr ':' '\n' | xargs -n 1 ls -1 | egrep '^gcc-(mp-)?[0-9]+' | tail -n1))
    export CXX=$(which $(echo $PATH | tr ':' '\n' | xargs -n 1 ls -1 | egrep '^g\+\+-(mp-)?[0-9]+' | tail -n1))

fi
export NUM_LOGICAL_CORES
echo "Determined $NUM_LOGICAL_CORES logical cores."
cmake --version
echo

cd "$APPVEYOR_BUILD_FOLDER" && \
mkdir build && cd build && cmake -G "$CMAKE_GENERATOR" ../src && make -j$NUM_LOGICAL_CORES

# vim: set ts=4 sw=4 sts=4 tw=0 et:
