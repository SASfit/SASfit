/* Library libcerf:
 *   Compute complex error functions, based on a new implementation of
 *   Faddeeva's w_of_z. Also provide Dawson and Voigt functions.
 *
 * File cerf.h:
 *   Declare exported functions.
 * 
 * Copyright:
 *   (C) 2012 Massachusetts Institute of Technology
 *   (C) 2013 Forschungszentrum Jülich GmbH
 * 
 * Licence:
 *   Permission is hereby granted, free of charge, to any person obtaining
 *   a copy of this software and associated documentation files (the
 *   "Software"), to deal in the Software without restriction, including
 *   without limitation the rights to use, copy, modify, merge, publish,
 *   distribute, sublicense, and/or sell copies of the Software, and to
 *   permit persons to whom the Software is furnished to do so, subject to
 *   the following conditions:
 * 
 *   The above copyright notice and this permission notice shall be
 *   included in all copies or substantial portions of the Software.
 * 
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *   LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 *   OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 *   WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
 *
 * Authors:
 *   Steven G. Johnson, Massachusetts Institute of Technology, 2012, core author
 *   Joachim Wuttke, Forschungszentrum Jülich, 2013, package maintainer
 *
 * Website:
 *   http://apps.jcns.fz-juelich.de/libcerf
 *
 * Revision history:
 *   ../CHANGELOG
 *
 * Man pages:
 *   w_of_z(3), dawson(3), voigt(3), cerf(3), erfcx(3), erfi(3)
 */

#include <complex.h> // C99 complex-number support

#ifndef __CERF_H
#  define __CERF_H
#  undef __BEGIN_DECLS
#  undef __END_DECLS
#  ifdef __cplusplus
#    define __BEGIN_DECLS extern "C" {
#    define __END_DECLS }
#  else
#    define __BEGIN_DECLS
#    define __END_DECLS
#  endif
__BEGIN_DECLS

// compute w(z) = exp(-z^2) erfc(-iz), Faddeeva's scaled complex error function
double _Complex w_of_z   (double _Complex z);
double         im_w_of_x(double x); // special case Im[w(x)] of real x
double re_w_of_z( double x, double y );
double im_w_of_z( double x, double y );

// compute erf(z), the error function of complex arguments
double _Complex cerf  (double _Complex z);

// compute erfc(z) = 1 - erf(z), the complementary error function
double _Complex cerfc(double _Complex z);

// compute erfcx(z) = exp(z^2) erfc(z), an underflow-compensated version of erfc
double _Complex cerfcx(double _Complex z);
double         erfcx (double x); // special case for real x

// compute erfi(z) = -i erf(iz), the imaginary error function
double _Complex cerfi(double _Complex z);
double         erfi (double x); // special case for real x

// compute dawson(z) = sqrt(pi)/2 * exp(-z^2) * erfi(z), Dawson's integral
double _Complex cdawson(double _Complex z);
double         dawson (double x); // special case for real x

// compute voigt(x,??), the convolution of a Gaussian and a Lorentzian
double voigt( double x, double sigma, double gamma );

// EXPERIMENTAL
double cerf_experimental_imw( double x, double y );
double cerf_experimental_rew( double x, double y );

__END_DECLS
#endif /* __CERF_H__ */
