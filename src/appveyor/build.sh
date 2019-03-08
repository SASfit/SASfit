#!/bin/sh

echo 'PWD0: '; pwd
cd $APPVEYOR_BUILD_FOLDER
echo 'PWD1: '; pwd
mkdir build && cd build && cmake ../src

# vim: set ts=2 sw=2 sts=2 tw=0:
