# Technical ToDo list

## open

- check example code for plugins (how to build&call a plugin in custom projects)

- check SASfit videos for applicability to current version

- remove Sourceforge page&repo

- build the manual PDF in appveyor as well, move the binary out of source
  - when building SASfit, download the latest manual from latest manual build
  - build locally with MikTex on Windows

- [CMake] remove external software package archives from source (GSL, fftw, sundials)
  - download those packages during CMake stage, just before unpacking

- [CMake] remove duplicate Tcl package from source, can be provided by saskit source tree
  - already same Tcl tree actually

- for multiple curves, they get the same colors and symbols by default?
  Can this be changed? Choose from a color table automatically? [by Andreas]

## done

- [done][appveyor] use bintray api for finding recently uploaded package files (setting them as direct download)

- [done][appveyor] set up appveyor for Linux builds

- [done][appveyor] fix caching of built external packages (GSL, ffts, sundials)

- [done] cleanup old versions and uploads on BinTray
  - get a script for this, perhaps from:
    https://github.com/AppImage/pkg2appimage/blob/master/bintray-tidy.sh

- [done][CMake] determine number of cores in CMake stage, so that it configures Makefiles to always use make with '-j<#cores>' in local builds, not just in appveyor mode when it is configured explicitly from outside
  - if Makefiles can be configured by cmake accordingly has to be researched
  - perhaps, CMake module *ProcessorCount* helps, but not needed actually

- [done][Cmake] remove code for creating a source package, provided by GitHub nowadays
