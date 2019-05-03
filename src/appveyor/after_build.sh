#!/bin/sh
# some info for debugging after build

for dir in "$APPVEYOR_BUILD_FOLDER/.." "$APPVEYOR_BUILD_FOLDER" "$APPVEYOR_BUILD_FOLDER/src"; do
    echo "$dir:"
    ls -la "$dir"
done
head -n 40 "$APPVEYOR_BUILD_FOLDER/src/Doxyfile"

# vim: set ts=4 sw=4 sts=4 tw=0 et:
