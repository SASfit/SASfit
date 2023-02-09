#ifndef SASFIT_SMOLPACK_H
#define SASFIT_SMOLPACK_H

#define maxdim 40

extern double int_smolyak(size_t, int, double *, double *, double(*ff)(double *, size_t, void*),void*, int);
extern double cc_int_smolyak(size_t, int,double *, double *,  double(*ff)(double *, size_t, void*),void*, int);

#endif // smolpack.h
