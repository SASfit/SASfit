#ifndef POLYROOTS_H
#define POLYROOTS_H

#include <complex.h>

int quartic_roots(double quarticCoeff4, double quarticCoeff3, double quarticCoeff2,
                   double quarticCoeff1, double quarticCoeff0, double complex rootsOut[4]);

#endif /* POLYROOTS_H */
