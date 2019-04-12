#!/bin/sh

cd $APPVEYOR_BUILD_FOLDER
mkdir build && cd build && cmake ../src && make

# vim: set ts=2 sw=2 sts=2 tw=0:
