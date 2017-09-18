DE-Quadrature (Numerical Automatic Integrator) Package

files
    intde1.c    : Quadrature Package in C       - Easy Version
    intde1.f    : Quadrature Package in Fortran - Easy Version
    intde1t.c   : Test Program of "intde1.c"
    intde1t.f   : Test Program of "intde1.f"
    intde1.doc  : Document of "intde1.*" (Japanese)
    intde2.c    : Quadrature Package in C       - Fast Version
    intde2.f    : Quadrature Package in Fortran - Fast Version
    intde2t.c   : Test Program of "intde2.c"
    intde2t.f   : Test Program of "intde2.f"
    intde2.doc  : Document of "intde2.*" (Japanese)

routines in the package
    intde  : integrator of f(x) over (a,b).
    intdei : integrator of f(x) over (a,infinity), 
                 f(x) is non oscillatory function.
    intdeo : integrator of f(x) over (a,infinity), 
                 f(x) is oscillatory function.

usage
    see block comments in the package file

copyright
    Copyright(C) 1996 Takuya OOURA (email: ooura@mmm.t.u-tokyo.ac.jp).
    You may use, copy, modify this code for any purpose and 
    without fee. You may distribute this ORIGINAL package.

