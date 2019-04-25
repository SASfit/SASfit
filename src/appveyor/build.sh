#!/bin/sh

cd $APPVEYOR_BUILD_FOLDER
mkdir build && cd build && cmake ../src && make

for dir in "$APPVEYOR_BUILD_FOLDER/.." "$APPVEYOR_BUILD_FOLDER" "$APPVEYOR_BUILD_FOLDER/src"; do
  echo "$dir:"
  ls -la "$dir"
done

# vim: set ts=2 sw=2 sts=2 tw=0:
