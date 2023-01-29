`quasimontecarlo` is a GSL-compatible quasi-Monte Carlo integrator. It works exactly like the GSL Monte Carlo integration routines, but using a quasirandom number generator instead of a pseudorandom number generator. (Quasirandom number generators produce a predictable sequence that fills an N-dimensional space erratically but almost uniformly.)

Usage is exactly the same as the GSL MC integrator:

1. create a quasirandom number generator using the GSL routine `gsl_qrng_alloc()`
2. create a QMC state object using `quasi_monte_alloc()`
3. do an integral using `quasi_monte_integrate()`
4. free the state object using `quasi_monte_free()`

`quasimontecarlo` uses CMake as its build system. Typically, to install a CMake-based project from source, from the directory containing `CMakeLists.txt`, run

    cmake .
    ./make
    ./make install

All files are licensed under the GPL version 2 or any later version at your option.

Development of `quasimontecarlo` is hosted on the Github project page <https://github.com/diazona/quasimontecarlo>, which is where you should report bugs or offer suggestions for improvement if you would like to do so. If this code ever gets incorporated into GSL itself, that will change.
