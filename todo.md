# Technical ToDo list

- [appveyor] use bintray api for finding recently uploaded package files (setting them as direct download)

- [appveyor] set up appveyor for Linux builds

- [appveyor] fix caching of built external packages (GSL, ffts, sundials)

- cleanup old versions and uploads on BinTray
  - get a script for this, perhaps from:
    https://github.com/AppImage/pkg2appimage/blob/master/bintray-tidy.sh

- [CMake] determine number of cores in CMake stage, so that it configures Makefiles to always use make with '-j<#cores>' in local builds, not just in appveyor mode when it is configured explicitly from outside
  - if Makefiles can be configured by cmake accordingly has to be rsearched
  - perhaps, CMake module *ProcessorCount* helps, but not needed actually

- [Cmake] remove code for creating a source package, provided by GitHub nowadays

- build the manual PDF in appveyor as well, move the binary out of source
  - when building SASfit, download the latest manual from latest manual build

- [CMake] remove external software package archives from source (GSL, fftw, sundials)
  - download those packages during CMake stage, just before unpacking

- [CMake] remove duplicate Tcl package from source, can be provided by saskit source tree
  - already same Tcl tree actually

- for multiple curves, they get the same colors and symbols by default?
  Can this be changed? Chose from a color table automatically? [by Andreas]

