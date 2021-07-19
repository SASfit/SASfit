#!/bin/sh

echo
echo "## Starting build on '$(uname -s)' ..."
echo

NUM_LOGICAL_CORES=1
CMAKE_GENERATOR='Unix Makefiles'

# Returns the latest version sorted command found in PATH, e.g. latest gcc, g++
get_latest_in_path()
{
    local pattern; pattern="$1"
    echo "Searching for pattern '$pattern':" >&2
    pattern="$(echo "$PATH" \
        | tr ':' '\n' | xargs -n 1 ls -1 2> /dev/null \
	| egrep "$pattern" | sort -uV)"
    local select; select="$(printf "%s\n" "$pattern" | tail -n1)"
    printf "%s\n\n" "$pattern" | sed 's/\(\b'$select'\b\)\(\s\)*$/\1 (selected)\2/' >&2
    echo $select
}

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
    CC=$(get_latest_in_path '^gcc(-(mp-)?[0-9]+)?$')
    CXX=$(get_latest_in_path '^g\+\+(-(mp-)?[0-9]+)?$')
    export CC=$(which $CC)
    export CXX=$(which $CXX)

fi

echo "Determined $NUM_LOGICAL_CORES logical cores."
echo

cd "$APPVEYOR_BUILD_FOLDER" && \
mkdir -p build && cd build && cmake -G "$CMAKE_GENERATOR" ../src && make -j$NUM_LOGICAL_CORES

# vim: set ts=4 sw=4 sts=4 tw=0 et:
