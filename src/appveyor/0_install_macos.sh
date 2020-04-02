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
    which brew
    brew -h
    brew install git cmake gcc
    brew cask install xquartz
#fi

# vim: set ts=4 sw=4 sts=4 tw=0 et:
