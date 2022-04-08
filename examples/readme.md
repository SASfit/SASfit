# Example of a standalone program using a precompiled SASfit plugin

This example shows how to make use of SASfit plugins from its binary package in custom applications, no need to build SASfit itself from source.

However, the source code files of SASfit are required for its header files which let the compiler know what the referenced function and variable names are. This can be acquired by downloading the source tree via `git clone https://github.com/SASfit/SASfit.git` or extracting [a source tree snapshot ZIP file](https://github.com/SASfit/SASfit/archive/refs/heads/master.zip) from [the project page on GitHub](https://github.com/SASfit/SASfit) (where this example lives).

## Assumed directory structure

    sasfit
    ├── sasfit-source  (as cloned from GitHub)
    │   └── examples
    └── sasfit_package (extracted binary package)

## Makefile included

A Makefile is provided as an example and for convenience. That should build and run the example with a single call of `make` in a terminal window, provided that the build tools and the SASfit packages are present. Make sure to adjust the **PACKAGE_DIR** to match the extracted binary package of SASfit.

## Details for macOS 11 (and later)

Build it like that from subdir *examples*:

    $ cd sasfit/sasfit-source/examples
    $ gcc-11 -I../../sasfit_package/plugins -I../src/sasfit_common/include -I../src/sasfit_core/include -I../saskit/src/tcl-8.4.20/generic/ -I../src/gsl/macos64/include/ -I../src/f2c/ -L../../sasfit_package/lib -lsasfit -L../../sasfit_package/plugins  -lsasfit_fuzzysphere  -o example_sasfit_ff_fuzzysphere example_sasfit_ff_fuzzysphere.c

Run it like that:

    $ DYLD_LIBRARY_PATH=../../sasfit_package/plugins/:../../sasfit_package/lib/ ./example_sasfit_ff_fuzzysphere

## Details for Windows 10 (and later, with minGW64)

With the same directory structure as for macOS:

    $ gcc -I../../sasfit_package/plugins/ -I../src/sasfit_common/include -I../src/sasfit_core/include -I../src/gsl/macos64/include/ -I../src/f2c -L../../sasfit_package/lib -lsasfit -L../../sasfit_package/plugins/ -lsasfit_fuzzysphere -o example_sasfit_ff_fuzzysphere example_sasfit_ff_fuzzysphere.c

For running the new `example_sasfit_ff_fuzzysphere.exe`, it expects the DLLs it depends on in the same path or to be found in a directory listed in `%PATH%`. The easiest method is to just copy the respective DLLs (`libsasfit.dll` and `libsasfit_fuzzysphere.dll`) to the current dir first. Run it in a minGW console like this:

    $ cp ../../sasfit_package/lib/libsasfit.dll .
    $ cp ../../sasfit_package/plugins/libsasfit_fuzzysphere.dll .
    $ ./example_sasfit_ff_fuzzysphere.exe

Double-clicking on the file works too but the Windows console with output messages of the program closes afterwards really quick and no messages can be read.
