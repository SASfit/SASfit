#!/bin/sh

echo Compiler: $COMPILER
echo Architecture: $MSYS2_ARCH
echo Platform: $PLATFORM
echo MSYS2 directory: $MSYS2_DIR
echo MSYS2 system: $MSYSTEM
echo MINGW_PACKAGE_PREFIX: $MINGW_PACKAGE_PREFIX
echo "PATH=$PATH"

echo ## Installing required packages:
## sh -lc "pacman -S --noconfirm make diffutils patch mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-python-pip mingw-w64-x86_64-fftw mingw-w64-x86_64-gsl mingw-w64-x86_64-zlib mingw-w64-x86_64-boost"

# Remove existing package with wrong versions first
pacman -Rns --noconfirm \
    mingw-w64-x86_64-gcc \
    mingw-w64-x86_64-libwinpthread \
    mingw-w64-x86_64-winpthreads \
    mingw-w64-x86_64-binutils \
    mingw-w64-x86_64-gcc-libs \
    mingw-w64-x86_64-bzip2 \
    mingw-w64-x86_64-gdb \
    mingw-w64-x86_64-gdb-multiarch \
    mingw-w64-x86_64-make \
    mingw-w64-x86_64-gettext-runtime \
    mingw-w64-x86_64-mpdecimal \
    mingw-w64-x86_64-mpfr \
    mingw-w64-x86_64-ncurses \
    mingw-w64-x86_64-python \
    mingw-w64-x86_64-readline \
    mingw-w64-x86_64-tcl \
    mingw-w64-x86_64-termcap \
    mingw-w64-x86_64-tools-git \
    mingw-w64-x86_64-zstd

# Install specific versions of some build packages
cd "$APPVEYOR_BUILD_FOLDER"
mkdir packages
cd packages
pacman -S --noconfirm curl gawk
sh -x "../src/appveyor/citools/msys2_get_pckg+deps.sh" gcc-13
sh "../src/appveyor/citools/msys2_get_pckg+deps.sh" cmake-3.29.2
sh "../src/appveyor/citools/msys2_get_pckg+deps.sh" python-pip-24.0
pacman -U --noconfirm *.zst

# Install more general build tools
pacman -S --noconfirm make diffutils coreutils patch mingw-w64-x86_64-fftw mingw-w64-x86_64-gsl

