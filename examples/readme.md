# Example of a standalone program using a precompiled SASfit plugin

This example shows how to make use of SASfit plugins from its binary package in custom applications, no need to build SASfit itself from source.

However, the source code files of SASfit are required for its header files which let the compiler know what the referenced function and variable names are. This can be acquired by downloading the source tree via `git clone https://github.com/SASfit/SASfit.git` or extracting [a source tree snapshot ZIP file](https://github.com/SASfit/SASfit/archive/refs/heads/master.zip) from [the project page on GitHub](https://github.com/SASfit/SASfit) (where this example lives).

## macOS 11 (and later)

Build it like that from subdir *examples* in binary package:

    gcc-11 -I../plugins -I../../sasfit-code/src/sasfit_common/include -I../../sasfit-code/src/sasfit_core/include -I../../sasfit-code/saskit/src/tcl-8.4.20/generic/ -I../../sasfit-code/src/gsl/macos64/include/ -I../../sasfit-code/src/f2c/ -L../lib -lsasfit -L../plugins  -lsasfit_fuzzysphere_shared  -o test test.c

Run it like that:

    DYLD_LIBRARY_PATH=../plugins/:../lib/ ./test

## Windows 10 (and later, with minGW64)

In a directory with 'test.c' parallel to sasfit binary dir:

    gcc -I../sasfit_220317104242_windows64/plugins/ -I/y/shared/sasfit-code/src/sasfit_common/include -I/y/shared/sasfit-code/src/sasfit_core/include -I/y/shared/sasfit-code/src/gsl/macos64/include/ -I/y/shared/sasfit-code/src/f2c -L../sasfit_220317104242_windows64/lib -lsasfit -L../sasfit_220317104242_windows64/plugins/ -lsasfit_fuzzysphere -o test test.c

For running the built `test.exe`, it expects the DLLs it depends on in the same path or to be found in a directory listed in `%PATH%`. The easiest method is to just copy the respective DLLs (`libsasfit.dll` and  `libsasfit_fuzzysphere.dll`) to the current dir first. Run it in a minGW console like this:

    ./test.exe

Double-clicking on the file works too but the Windows console containing output messages of the program closes afterwards really quick.
