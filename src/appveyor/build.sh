#!/bin/sh

echo
echo "## Starting build ..."
echo

cd "$APPVEYOR_BUILD_FOLDER" && \
mkdir build && cd build && cmake ../src

# vim: set ts=2 sw=2 sts=2 tw=0:
