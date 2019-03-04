@echo off

cd %APPVEYOR_BUILD_FOLDER%
sh -lc "mkdir build && cd build"
sh -lc "cmake ../src"

@echo on
rem vim: set ts=2 sw=2 sts=2 tw=0:
