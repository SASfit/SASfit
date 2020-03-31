#!/bin/sh

echo
echo "## Installing build tools ..."
echo

env

if [ "$COMPILER" = msys2 ];
then
    set -x
    export PATH="/$MSYSTEM/bin:/opt/bin:/usr/local/bin:/usr/bin:/bin:$PATH"
    echo "Showing PATH:"
    echo "PATH=$PATH"
    echo "CMD: '$0' argv: '$@'"
    echo "shell: '$(/bin/ps -p $$)'"
    echo "test: '$(which pacman)'"
    ls -la /usr/bin/pacman
    which sh
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
