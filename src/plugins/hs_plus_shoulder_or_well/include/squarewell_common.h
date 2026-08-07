/* squarewell_common.h
 * Math shared across the ORPA-family models: the Percus-Yevick
 * hard-sphere direct correlation function's Fourier transform, the
 * square-well Mayer-function step's FT, and polynomial basis-function
 * FTs used by the non-linear ORPA core-condition solver.
 *
 * All closed forms here were derived via sympy `integrate` then emitted
 * with `ccode` -- NOT hand-rearranged. An earlier hand-rearranged version
 * of sw_c0_hat contained a genuine algebra error, wrong even at 60-digit
 * (mpmath) precision -- a real bug, not a floating-point artifact -- so
 * this module avoids hand-rearranging closed forms entirely. Small-k
 * Taylor series are used below a safety threshold where the closed forms
 * lose precision to floating-point cancellation.
 *
 * Reduced units throughout: sigma=1, k*=Q*sigma, rho*=rho*sigma^3=6*eta/pi.
 */
#ifndef SQUAREWELL_COMMON_H
#define SQUAREWELL_COMMON_H

/* FT of the PY hard-sphere direct correlation function
 * c_HS(r) = alpha + delta*r + gamma*r^3 (r<1, reduced units). */
double sw_c0_hat(double kstar, double phi);

/* FT of the square-well Mayer function step: value x for 1<r*<lam, zero
 * elsewhere (x = exp(eps/kT) - 1, eps/kT>0 = attractive well). */
double sw_fhat_mayer(double k, double x, double lam);

/* FT of the polynomial basis function P_j(r)=r^j for r<1 (else 0),
 * j=0..3. Used by the non-linear ORPA solver. */
double sw_Phat(int j, double k);

/* P_hat_j(k=0) = 4*pi/(j+3), j=0..3 */
extern const double SW_PHAT0[4];

#endif
