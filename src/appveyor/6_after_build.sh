#!/bin/sh
# some info for debugging after build

for dir in "$APPVEYOR_BUILD_FOLDER/.." \
           "$APPVEYOR_BUILD_FOLDER" \
           "$APPVEYOR_BUILD_FOLDER/src" \
           "$APPVEYOR_BUILD_FOLDER/../sasfit_"*; do
    [ -d "$dir" ] || continue
    echo "$dir:"
    ls -la "$dir"
done

# vim: set ts=4 sw=4 sts=4 tw=0 et:
