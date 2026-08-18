/* ------------------------------------------------------------------
 * polyroots.h -- quartic root finder used by rmsa.c to find all four
 * roots of
 *   quarticCoeff4*x^4 + quarticCoeff3*x^3 + quarticCoeff2*x^2
 *     + quarticCoeff1*x + quarticCoeff0 = 0
 * which appears once, in the very first MSA solve, when selecting the
 * physically correct root (the one with g(r<sigma) = 0).
 *
 * Implementation: GSL's gsl_poly_complex_solve(), which finds the
 * eigenvalues of the polynomial's companion matrix via a balanced
 * QR algorithm. This is the same class of method LAPACK (and hence
 * numpy.roots(), used by the reference jscatter implementation this
 * library is ported from) uses, and is materially more robust than a
 * closed-form (Ferrari/Cardano) solver for the near-degenerate
 * quartics that can occur near the boundaries of the physically
 * valid parameter region -- see README.md for a worked comparison.
 *
 * Thread safety: this function allocates and frees its own GSL
 * workspace on every call (no shared/static state), so concurrent
 * calls from different threads are safe as long as each call is given
 * its own rootsOut buffer. It also disables GSL's default (aborting)
 * error handler on first use, via pthread_once() so that this is
 * itself race-free regardless of how many threads call
 * quartic_roots() concurrently -- see README.md's "Thread safety"
 * section for why this is necessary and what its process-wide,
 * GSL-global side effect means for a host application that also uses
 * GSL elsewhere.
 * ------------------------------------------------------------------ */
#ifndef POLYROOTS_H
#define POLYROOTS_H

#include <complex.h>

/* Roots of quarticCoeff4*x^4 + quarticCoeff3*x^3 + quarticCoeff2*x^2 +
 * quarticCoeff1*x + quarticCoeff0 = 0. Returns 0 and fills
 * rootsOut[0..3] on success. Returns nonzero (and leaves rootsOut
 * untouched) if quarticCoeff4 is exactly 0.0 -- rather than passing
 * that through to GSL, which treats a zero leading coefficient as a
 * fatal usage error and, under GSL's default error handler, aborts
 * the whole process. (A tiny-but-nonzero leading coefficient, however
 * small relative to the other coefficients, is handled correctly by
 * GSL and is not rejected here.) This is the only error path expected
 * in normal use; a nonzero return can also occur (see polyroots.c) if
 * the *host* application has replaced GSL's default handler with a
 * non-aborting one and GSL itself reports an internal error, which
 * has not been observed in this library's validation but is handled
 * defensively regardless.                                             */
int quartic_roots(double quarticCoeff4, double quarticCoeff3, double quarticCoeff2,
                   double quarticCoeff1, double quarticCoeff0, double complex rootsOut[4]);

#endif /* POLYROOTS_H */
