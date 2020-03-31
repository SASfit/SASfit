#!/bin/sh

echo
echo "## Installing build tools ..."
echo

env

if [ "$COMPILER" = msys2 ];
then
    set -x
    export PATH="/c/$MSYS2_DIR/$MSYSTEM/bin:/c/$MSYS2_DIR/opt/bin:/c/$MSYS2_DIR/usr/local/bin:/c/$MSYS2_DIR/usr/bin:/c/$MSYS2_DIR/bin:$PATH"
    echo "PATH=$PATH"
    echo "CMD: '$0' argv: '$@'"
    shell="$(/bin/ps -p $$ | awk "/$$/ {print \$NF}")"
    echo $shell
    cygpath -w $shell
    echo "test: '$(which pacman)'"
    ls -la /usr/bin/pacman 2>&1
    ls -la /c/msys64/usr/bin/pacman 2>&1
exit 0
    sh -lc "pacman"

    # Ensure pacman mirrors and GIT are up to date:
    pacman -S --needed --noconfirm pacman-mirrors
    pacman -S --needed --noconfirm git

    # Updating packages first:
    pacman -Syu --noconfirm
    pacman -Syu --noconfirm

    # Installing required packages:
    pacman -S --noconfirm make diffutils patch mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake
fi

# vim: set ts=4 sw=4 sts=4 tw=0 et:
