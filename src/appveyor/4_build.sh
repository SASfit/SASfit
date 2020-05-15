#!/bin/sh

echo
echo "## Starting build on '$(uname -s)' ..."
echo

NUM_LOGICAL_CORES=1
CMAKE_GENERATOR='Unix Makefiles'
if [ "$(uname -s)" = "Darwin" ];
then
    # macOS
    # determine number of computing cores
    NUM_LOGICAL_CORES=$(sysctl -n hw.ncpu)
    # find latest gcc and g++ compilers and set them as global variables
    export CC=$(which $(echo $PATH | tr ':' '\n' | xargs -n 1 ls -1 | egrep '^gcc-(mp-)?[0-9]+' | tail -n1))
    export CXX=$(which $(echo $PATH | tr ':' '\n' | xargs -n 1 ls -1 | egrep '^g\+\+-(mp-)?[0-9]+' | tail -n1))

elif uname -s | grep -qi '^mingw64';
then
    # Windows

    WMIC="$(command -v wmic)"
    if [ -x "$WMIC" ]; then
        NUM_LOGICAL_CORES="$($WMIC cpu get /format:list \
            | grep NumberOfLogicalProcessors \
            | cut -d= -f2 \
            | tr -d '[:space:]' )"
    fi
    CMAKE_GENERATOR='MSYS Makefiles'
fi
export NUM_LOGICAL_CORES
echo "Determined $NUM_LOGICAL_CORES logical cores."
which cmake
cmake --version
echo

cd "$APPVEYOR_BUILD_FOLDER" && \
mkdir build && cd build && cmake -G "$CMAKE_GENERATOR" ../src && make -j$NUM_LOGICAL_CORES

# vim: set ts=4 sw=4 sts=4 tw=0 et:
