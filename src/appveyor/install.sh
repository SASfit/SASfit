#!/bin/sh

echo
echo "## Installing build tools ..."
echo

env

if [ "$COMPILER" = msys2 ];
then # on Windows
    set -x
    ROOT_DIR=/c/$MSYS2_DIR
    export PATH="$ROOT_DIR/$MSYSTEM/bin:$ROOT_DIR/usr/bin:$ROOT_DIR/bin"
    echo "CMD: '$0' argv: '$@'"
    ls -la
    ls /c/$MSYS2_DIR/
    pid=$$
    /bin/ps -p $pid
    echo "test"
    ps -p $pid
    which ps
    which cygpath
    eval "/bin/ps -p $pid | awk '/$pid/ {print \$NF}'"
    cygpath -w /usr/bin/sh
    ls -la /bin/
    ls -la /usr/bin/
    ls -la /c/msys64/usr/bin
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
