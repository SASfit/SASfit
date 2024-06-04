#!/usr/bin/env dash

echo
echo "## Starting build on '$(uname -s)' ..."
echo

NUM_LOGICAL_CORES=1
CMAKE_GENERATOR='Unix Makefiles'

scriptdir="$(cd "$(dirname "$0")" && pwd -P)"
findCmdInPath="$scriptdir/../scripts/findCmdInPath.sh"

if uname -s | grep -qi '^mingw64'; # Windows
then
    # assumes to be run on Windows in a 'MSYS2 MinGW 64-bit' shell
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
        #export CC=/usr/local/bin/gcc-13
        #export CXX=/usr/local/bin/g++-13
    else # on Linux
        NUM_LOGICAL_CORES="$(awk '/processor/' /proc/cpuinfo | wc -l)"
    fi
    # find latest gcc and g++ compilers and set them as global variables
    [ -f "$CC" ]  || export CC="$( sh "$findCmdInPath" '/gcc(-(mp-)?[0-9]+)?$')"
    [ -f "$CXX" ] || export CXX="$(sh "$findCmdInPath" '/g\+\+(-(mp-)?[0-9]+)?$')"
fi

echo "Determined $NUM_LOGICAL_CORES logical cores."
echo
echo "Testing:"
$CC --version

cd "$APPVEYOR_BUILD_FOLDER" && \
mkdir -p build && cd build && cmake -G "$CMAKE_GENERATOR" -DCREATE_BINARY=TRUE "$scriptdir/.." && make -j$NUM_LOGICAL_CORES

#mkdir -p build && cd build && cmake -G "$CMAKE_GENERATOR" -DCREATE_BINARY=TRUE -DDEBUG=TRUE "$scriptdir/.." && make -j$NUM_LOGICAL_CORES

# vim: set ts=4 sw=4 sts=4 tw=0 et:
