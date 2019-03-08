
sh -lc "cd %APPVEYOR_BUILD_FOLDER%"
sh -lc "echo 'PWD: '; pwd"
sh -lc "mkdir build && cd build"
sh -lc "cmake ../src"

@echo off
rem vim: set ts=2 sw=2 sts=2 tw=0:
