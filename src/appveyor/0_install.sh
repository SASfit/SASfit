#!/bin/sh

echo
echo "## Installing build tools ..."
echo

env

#if [ "$COMPILER" = msys2 ];
#then # on Windows
    set -x
#    export PATH="/c/$MSYS2_DIR/$MSYSTEM/bin:/c/$MSYS2_DIR/opt/bin:/c/$MSYS2_DIR/usr/local/bin:/c/$MSYS2_DIR/usr/bin:/c/$MSYS2_DIR/bin:$PATH"
#    ROOT_DIR=/c/$MSYS2_DIR
#    export PATH="$ROOT_DIR/$MSYSTEM/bin:$ROOT_DIR/usr/bin:$ROOT_DIR/bin"
    echo "CMD: '$0' argv: '$@'"
    ls -la
    pid=$$
    /bin/ps -p $pid
    echo "test"
    ps -p $pid
    which ps
    eval "/bin/ps -p $pid | awk '/$pid/ {print \$NF}'"
    ls -la /bin/
    ls -la /usr/bin/
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
#fi

# vim: set ts=4 sw=4 sts=4 tw=0 et:
