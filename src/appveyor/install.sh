#!/bin/sh

echo
echo "## Installing build tools ..."
echo

env

if [ "$COMPILER" = msys2 ];
then
    export PATH="/$MSYSTEM/bin:/usr/bin:$PATH"
    echo "PATH=$PATH"
    cd / && pwd

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
