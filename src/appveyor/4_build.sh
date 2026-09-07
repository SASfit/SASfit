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
        # gcc/gcc@N formulae are keg-only on macOS (Homebrew deliberately does
        # not symlink their bin/ dir into the general prefix, to avoid
        # shadowing Apple's own /usr/bin/gcc clang wrapper), so a bare `which
        # gcc`/`which g++` here -- or the findCmdInPath fallback below, which
        # only searches directories already on $PATH -- would silently find
        # nothing of ours (or Apple's clang) unless that bin/ dir is already
        # on $PATH. It also cannot rely on 0_install_macos.sh having put it
        # there, since that runs as a separate `install:` script in
        # appveyor.yml and does not share environment with this one. Resolve
        # the keg-only prefix directly instead, so this is self-contained
        # regardless of PATH state or exactly which script installed it.
        if command -v brew >/dev/null 2>&1; then
            GCC_KEG_PREFIX="$(brew --prefix gcc@15 2>/dev/null || brew --prefix gcc 2>/dev/null)"
            if [ -n "$GCC_KEG_PREFIX" ] && [ -d "$GCC_KEG_PREFIX/bin" ]; then
                PATH="$GCC_KEG_PREFIX/bin:$PATH"
            fi
        fi
        # Deliberately do NOT do `export CC=$(which gcc)` here as the
        # Linux branch below does: Homebrew's gcc formulae only ever
        # provide version-suffixed binaries (gcc-15, not gcc), so a bare
        # "gcc" found on PATH at this point can only be Apple's
        # /usr/bin/gcc clang wrapper -- which IS a real, executable file,
        # so it would silently satisfy the "[ -f "$CC" ]" check below and
        # get used instead of real GNU gcc, with no error or warning.
        # Leaving CC/CXX unset here means the versioned findCmdInPath
        # lookup below always runs for macOS instead.
    else # on Linux
        NUM_LOGICAL_CORES="$(awk '/processor/' /proc/cpuinfo | wc -l)"
        export CC=$(which gcc)
        export CXX=$(which g++)
    fi
    # find latest gcc and g++ compilers and set them as global variables
    [ -f "$CC" ]  || export CC="$( sh "$findCmdInPath" '/gcc(-(mp-)?[0-9]+)?$')"
    [ -f "$CXX" ] || export CXX="$(sh "$findCmdInPath" '/g\+\+(-(mp-)?[0-9]+)?$')"
fi

echo "Determined $NUM_LOGICAL_CORES logical cores."

([ -z "$APPVEYOR_BUILD_FOLDER" ] || cd "$APPVEYOR_BUILD_FOLDER") && \
mkdir -p build && cd build && cmake -G "$CMAKE_GENERATOR" -DCREATE_BINARY=TRUE "$scriptdir/.." && make -j$NUM_LOGICAL_CORES

#mkdir -p build && cd build && cmake -G "$CMAKE_GENERATOR" -DCREATE_BINARY=TRUE -DDEBUG=TRUE "$scriptdir/.." && make -j$NUM_LOGICAL_CORES

# vim: set ts=4 sw=4 sts=4 tw=0 et:
