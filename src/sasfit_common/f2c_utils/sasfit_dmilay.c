/*
 * src/sasfit_common/f2c_utils/sasfit_dmilay.c
 *
 * Copyright (c) 2008-2009, Paul Scherrer Institute (PSI)
 *
 * This file is part of SASfit.
 *
 * SASfit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SASfit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SASfit.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *   Ingo Bressler (ingo@cs.tu-berlin.de)
 */

/* DMiLay.f -- translated by f2c (version 20041007).
   You must link the resulting object file with libf2c:
	on Microsoft Windows system, link with libf2c.lib;
	on Linux or Unix systems, link with .../path/to/libf2c.a -lm
	or, if you install libf2c.a in a standard place, with -lf2c -lm
	-- in that order, at the end of the command line, as in
		cc *.o -lf2c -lm
	Source for libf2c is in /netlib/f2c/libf2c.zip, e.g.,

		http://www.netlib.org/f2c/libf2c.zip
*/

#include <gsl/gsl_math.h>
#include "../include/sasfit_utils_f2c.h"

int derrmsg_(char *, logical *, ftnlen);

/* Table of constant values */

static logical c_true = TRUE_;
static doublecomplex c_b16 = {1.,0.};
static integer c__2 = 2;
static integer c__1 = 1;

int sasfit_dmilay(real *rcore, real *rshell, real *wvno, complex *rindsh, 
        complex *rindco, real *mu, integer *numang, real *qext, real *qsca, 
        real *qbs, real *gqsc, real *m1, real *m2, real *s21, real *d21,
	integer *maxang)
{
    /* Initialized data */

    static logical pass1 = TRUE_;
    static doublereal toler = 1e-6;
    static doublecomplex czero = {0.,0.};
    static doublecomplex ci = {0.,1.};

    /* System generated locals */
    integer d21_dim1, d21_offset, m1_dim1, m1_offset, m2_dim1, m2_offset, 
	    s21_dim1, s21_offset, i__1, i__2, i__3, i__4, i__5;
    real r__1;
    doublereal d__1, d__2, d__3, d__4, d__5;
    complex q__1;
    doublecomplex z__1, z__2, z__3, z__4, z__5, z__6, z__7, z__8, z__9, z__10,
	     z__11, z__12, z__13, z__14, z__15, z__16;

    /* Builtin functions */
    /*
    double asin(doublereal), c_abs(complex *), r_imag(complex *), d_imag(
	    doublecomplex *);
    void z_div(doublecomplex *, doublecomplex *, doublecomplex *);
    double cos(doublereal), sin(doublereal), exp(doublereal);
    void pow_zi(doublecomplex *, doublecomplex *, integer *);
    */

    /* Local variables */
    static integer j, k, m, n;
    static doublereal t[5];
    static doublecomplex u[8], w[3000]	/* was [3][1000] */, z__[4], k1, k2, 
	    k3, s1[200]	/* was [100][2] */, s2[200]	/* was [100][2] */;
    static doublereal x1, y1, x4, y4, aa, bb;
    static doublecomplex ac;
    static doublereal cc, dd;
    static doublecomplex bc;
    static doublereal ta[4], pi[300]	/* was [100][3] */;
    static integer nn;
    static doublereal rx;
    static doublecomplex dh1, dh2, dh4;
    static doublereal ey1, ey4;
    static doublecomplex wm1;
    static doublereal aim, are, bim, bre, e2y1, tau[300]	/* was [100][
	    3] */;
    static doublecomplex wfn[2];
    static doublereal rmm;
    static integer nmx1, nmx2;
    static doublecomplex acap[1000], acoe, bcoe, p24h21, p24h24, rrfx;
    static doublereal am1im, am1re, bm1im, bm1re, cosx1, cosx4, sinx1, sinx4;
    static doublecomplex sback;
    static doublereal dqsca, dgqsc, denom, xcore, dqext, ey1my4, pinum;
    static doublecomplex dumsq;
    static doublereal ey1py4;
    static doublecomplex dummy, acoem1, bcoem1;
    static doublereal si2tht[100];
    static logical inperr;
    static doublereal xshell;

/* ********************************************************************** */
/*    DOUBLE PRECISION version of MieLay, which computes electromagnetic */
/*    scattering by a stratified sphere, i.e. a particle consisting of a */
/*    spherical core surrounded by a spherical shell.  The surrounding */
/*    medium is assumed to have refractive index unity.  The formulas, */
/*    manipulated to avoid the ill-conditioning that plagued earlier */
/*    formulations, were published in: */
/*        Toon, O. and T. Ackerman, Applied Optics 20, 3657 (1981) */
/*    Further documentation, including definitons of input and output */
/*    arguments, is inside the single precision version of this program */
/*    (SUBROUTINE MieLay, available by anonymous ftp from */
/*    climate.gsfc.nasa.gov in directory pub/wiscombe). */
/*    It is recommended to use this DOUBLE PRECISION version for IEEE */
/*    arithmetic (32-bit floating point) computers, just to be safe. */
/*    If computer time is critical, back-to-back tests with the single */
/*    precision version should be done for ranges of radii and refractive */
/*    index relevant to your particular problem, before adopting the */
/*    single precision version.  This version is also recommended for */
/*    cases of large size parameter (bigger than 10 or so) and/or large */
/*    imaginary refractive index (bigger than 1 or so) and also whenever */
/*    overflows or strange behavior are encountered in running the */
/*    single precision version.  Sometimes the bigger exponent range in */
/*    DOUBLE PRECISION is as important as the added precision. */
/*    This version is designed to be interchangeable with the single */
/*    precision version:  all the floating-point input arguments are */
/*    still single precision.  Only the name of the routine has been */
/*    changed to prevent confusion (and it is strongly urged not to */
/*    change it to MieLay for the same reason). */
/* ********************************************************************** */
/*     .. Parameters .. */
/*     .. */
/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Local Arrays .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Save statement .. */
/*     .. */
/*     .. Data statements .. */
    /* Parameter adjustments */
    --mu;
    d21_dim1 = *maxang;
    d21_offset = 1 + d21_dim1;
    d21 -= d21_offset;
    s21_dim1 = *maxang;
    s21_offset = 1 + s21_dim1;
    s21 -= s21_offset;
    m2_dim1 = *maxang;
    m2_offset = 1 + m2_dim1;
    m2 -= m2_offset;
    m1_dim1 = *maxang;
    m1_offset = 1 + m1_dim1;
    m1 -= m1_offset;

    /* Function Body */
/*     .. */
    if (pass1) {
	pinum = asin(1.) * 2.;
	pass1 = FALSE_;
    }
    xshell = *rshell * *wvno;
    xcore = *rcore * *wvno;
    t[0] = xshell * c_abs(rindsh);
    nmx1 = (integer) (t[0] * 1.1);
    nmx2 = (integer) t[0];
    if (nmx1 <= 150) {
	nmx1 = 150;
	nmx2 = 135;
    }
/*                        ** Check input arguments for gross errors */
    inperr = FALSE_;
    if (*wvno <= 0.f) {
	inperr = sasfit_f2c_wrtbad_("WVNO", (ftnlen)4);
    }
    if (*rshell <= 0.f) {
	inperr = sasfit_f2c_wrtbad_("Rshell", (ftnlen)6);
    }
    if (*rcore <= 0.f || *rcore > *rshell) {
	inperr = sasfit_f2c_wrtbad_("Rcore", (ftnlen)5);
    }
    if (rindsh->r <= 0.f || r_imag(rindsh) > 0.f) {
	inperr = sasfit_f2c_wrtbad_("RindSh", (ftnlen)6);
    }
    if (rindco->r <= 0.f || r_imag(rindco) > 0.f) {
	inperr = sasfit_f2c_wrtbad_("RindCo", (ftnlen)6);
    }
    if (*numang < 0) {
	inperr = sasfit_f2c_wrtbad_("NumAng", (ftnlen)6);
    }
    if (*numang > 100) {
	inperr = sasfit_f2c_wrtdim_("MxAng", numang, (ftnlen)5);
    }
    if (*numang > *maxang) {
	inperr = sasfit_f2c_wrtdim_("MaxAng", numang, (ftnlen)6);
    }
    if (nmx1 + 1 > 1000) {
	i__1 = nmx1 + 1;
	inperr = sasfit_f2c_wrtdim_("LL", &i__1, (ftnlen)2);
    }
    i__1 = *numang;
    for (j = 1; j <= i__1; ++j) {
	if (mu[j] < -toler || mu[j] > toler + 1.f) {
	    inperr = sasfit_f2c_wrtbad_("MU", (ftnlen)2);
	}
/* L10: */
    }
    if (inperr) {
	derrmsg_("MIELAY--Input argument errors.  Aborting...", &c_true, (
		ftnlen)43);
    }
    q__1.r = *wvno * rindco->r, q__1.i = *wvno * rindco->i;
    k1.r = q__1.r, k1.i = q__1.i;
    q__1.r = *wvno * rindsh->r, q__1.i = *wvno * rindsh->i;
    k2.r = q__1.r, k2.i = q__1.i;
    z__1.r = *wvno, z__1.i = 0.;
    k3.r = z__1.r, k3.i = z__1.i;
    z__1.r = xshell * rindsh->r, z__1.i = xshell * rindsh->i;
    z__[0].r = z__1.r, z__[0].i = z__1.i;
    z__[1].r = xshell, z__[1].i = 0.;
    z__1.r = xcore * rindco->r, z__1.i = xcore * rindco->i;
    z__[2].r = z__1.r, z__[2].i = z__1.i;
    z__1.r = xcore * rindsh->r, z__1.i = xcore * rindsh->i;
    z__[3].r = z__1.r, z__[3].i = z__1.i;
    x1 = z__[0].r;
    y1 = d_imag(z__);
    x4 = z__[3].r;
    y4 = d_imag(&z__[3]);
    rx = 1. / xshell;
/*                                ** Down-recurrence for A function */
    i__1 = nmx1;
    acap[i__1].r = czero.r, acap[i__1].i = czero.i;
    for (m = 1; m <= 3; ++m) {
	i__1 = m + (nmx1 + 1) * 3 - 4;
	w[i__1].r = czero.r, w[i__1].i = czero.i;
/* L20: */
    }
    z__2.r = xshell * rindsh->r, z__2.i = xshell * rindsh->i;
    z_div(&z__1, &c_b16, &z__2);
    rrfx.r = z__1.r, rrfx.i = z__1.i;
    for (nn = nmx1; nn >= 1; --nn) {
	i__1 = nn - 1;
	i__2 = nn + 1;
	d__1 = (doublereal) i__2;
	z__2.r = d__1 * rrfx.r, z__2.i = d__1 * rrfx.i;
	i__3 = nn + 1;
	d__2 = (doublereal) i__3;
	z__5.r = d__2 * rrfx.r, z__5.i = d__2 * rrfx.i;
	i__4 = nn;
	z__4.r = z__5.r + acap[i__4].r, z__4.i = z__5.i + acap[i__4].i;
	z_div(&z__3, &c_b16, &z__4);
	z__1.r = z__2.r - z__3.r, z__1.i = z__2.i - z__3.i;
	acap[i__1].r = z__1.r, acap[i__1].i = z__1.i;
	for (m = 1; m <= 3; ++m) {
	    i__1 = m + nn * 3 - 4;
	    i__2 = nn + 1;
	    z__3.r = (doublereal) i__2, z__3.i = 0.;
	    z_div(&z__2, &z__3, &z__[m]);
	    i__3 = nn + 1;
	    z__7.r = (doublereal) i__3, z__7.i = 0.;
	    z_div(&z__6, &z__7, &z__[m]);
	    i__4 = m + (nn + 1) * 3 - 4;
	    z__5.r = z__6.r + w[i__4].r, z__5.i = z__6.i + w[i__4].i;
	    z_div(&z__4, &c_b16, &z__5);
	    z__1.r = z__2.r - z__4.r, z__1.i = z__2.i - z__4.i;
	    w[i__1].r = z__1.r, w[i__1].i = z__1.i;
/* L30: */
	}
/* L40: */
    }
    i__1 = *numang;
    for (j = 1; j <= i__1; ++j) {
/* Computing 2nd power */
	r__1 = mu[j];
	si2tht[j - 1] = 1. - r__1 * r__1;
	pi[j - 1] = 0.;
	pi[j + 99] = 1.;
	tau[j - 1] = 0.;
	tau[j + 99] = mu[j];
/* L50: */
    }
/*                          ** Initialization of homogeneous sphere */
    t[0] = cos(xshell);
    t[1] = sin(xshell);
    d__1 = -t[1];
    z__1.r = t[0], z__1.i = d__1;
    wm1.r = z__1.r, wm1.i = z__1.i;
    z__1.r = t[1], z__1.i = t[0];
    wfn[0].r = z__1.r, wfn[0].i = z__1.i;
    ta[0] = t[1];
    ta[1] = t[0];
    z__2.r = rx * wfn[0].r, z__2.i = rx * wfn[0].i;
    z__1.r = z__2.r - wm1.r, z__1.i = z__2.i - wm1.i;
    wfn[1].r = z__1.r, wfn[1].i = z__1.i;
    ta[2] = wfn[1].r;
    ta[3] = d_imag(&wfn[1]);
/*                      ** Initialization procedure for stratified sphere */
    n = 1;
    sinx1 = sin(x1);
    sinx4 = sin(x4);
    cosx1 = cos(x1);
    cosx4 = cos(x4);
    ey1 = exp(y1);
/* Computing 2nd power */
    d__1 = ey1;
    e2y1 = d__1 * d__1;
    ey4 = exp(y4);
    ey1my4 = exp(y1 - y4);
    ey1py4 = ey1 * ey4;
    aa = sinx4 * (ey1py4 + ey1my4);
    bb = cosx4 * (ey1py4 - ey1my4);
    cc = sinx1 * (e2y1 + 1.);
    dd = cosx1 * (e2y1 - 1.);
/* Computing 2nd power */
    d__1 = sinx1;
    denom = e2y1 * (d__1 * d__1 * 4. - 2. + e2y1) + 1.;
    d__1 = (aa * cc + bb * dd) / denom;
    d__2 = (bb * cc - aa * dd) / denom;
    z__1.r = d__1, z__1.i = d__2;
    dummy.r = z__1.r, dummy.i = z__1.i;
    i__1 = n - 1;
    z__5.r = (doublereal) n, z__5.i = 0.;
    z_div(&z__4, &z__5, z__);
    z__3.r = acap[i__1].r + z__4.r, z__3.i = acap[i__1].i + z__4.i;
    z__2.r = dummy.r * z__3.r - dummy.i * z__3.i, z__2.i = dummy.r * z__3.i + 
	    dummy.i * z__3.r;
    i__2 = n * 3 - 1;
    z__8.r = (doublereal) n, z__8.i = 0.;
    z_div(&z__7, &z__8, &z__[3]);
    z__6.r = w[i__2].r + z__7.r, z__6.i = w[i__2].i + z__7.i;
    z_div(&z__1, &z__2, &z__6);
    dummy.r = z__1.r, dummy.i = z__1.i;
    pow_zi(&z__1, &dummy, &c__2);
    dumsq.r = z__1.r, dumsq.i = z__1.i;
/* Computing 2nd power */
    d__2 = sinx4;
    d__1 = d__2 * d__2 - .5;
    d__3 = cosx4 * sinx4;
    z__3.r = d__1, z__3.i = d__3;
/* Computing 2nd power */
    d__5 = ey4;
    d__4 = d__5 * d__5;
    z__2.r = d__4 * z__3.r, z__2.i = d__4 * z__3.i;
    z__1.r = z__2.r + .5, z__1.i = z__2.i;
    p24h24.r = z__1.r, p24h24.i = z__1.i;
    d__1 = sinx1 * sinx4 - cosx1 * cosx4;
    d__2 = sinx1 * cosx4 + cosx1 * sinx4;
    z__4.r = d__1, z__4.i = d__2;
    z__3.r = z__4.r * .5, z__3.i = z__4.i * .5;
    z__2.r = ey1py4 * z__3.r, z__2.i = ey1py4 * z__3.i;
    d__3 = sinx1 * sinx4 + cosx1 * cosx4;
    d__4 = -sinx1 * cosx4 + cosx1 * sinx4;
    z__7.r = d__3, z__7.i = d__4;
    z__6.r = z__7.r * .5, z__6.i = z__7.i * .5;
    z__5.r = ey1my4 * z__6.r, z__5.i = ey1my4 * z__6.i;
    z__1.r = z__2.r + z__5.r, z__1.i = z__2.i + z__5.i;
    p24h21.r = z__1.r, p24h21.i = z__1.i;
    z__4.r = ci.r * z__[0].r - ci.i * z__[0].i, z__4.i = ci.r * z__[0].i + 
	    ci.i * z__[0].r;
    z__3.r = z__4.r + 1., z__3.i = z__4.i;
    z_div(&z__2, z__, &z__3);
    z_div(&z__5, &c_b16, z__);
    z__1.r = z__2.r - z__5.r, z__1.i = z__2.i - z__5.i;
    dh1.r = z__1.r, dh1.i = z__1.i;
    z__4.r = ci.r * z__[1].r - ci.i * z__[1].i, z__4.i = ci.r * z__[1].i + 
	    ci.i * z__[1].r;
    z__3.r = z__4.r + 1., z__3.i = z__4.i;
    z_div(&z__2, &z__[1], &z__3);
    z_div(&z__5, &c_b16, &z__[1]);
    z__1.r = z__2.r - z__5.r, z__1.i = z__2.i - z__5.i;
    dh2.r = z__1.r, dh2.i = z__1.i;
    z__4.r = ci.r * z__[3].r - ci.i * z__[3].i, z__4.i = ci.r * z__[3].i + 
	    ci.i * z__[3].r;
    z__3.r = z__4.r + 1., z__3.i = z__4.i;
    z_div(&z__2, &z__[3], &z__3);
    z_div(&z__5, &c_b16, &z__[3]);
    z__1.r = z__2.r - z__5.r, z__1.i = z__2.i - z__5.i;
    dh4.r = z__1.r, dh4.i = z__1.i;
    z__5.r = (doublereal) n, z__5.i = 0.;
    z_div(&z__4, &z__5, &z__[3]);
    z__3.r = dh4.r + z__4.r, z__3.i = dh4.i + z__4.i;
    i__1 = n * 3 - 1;
    z__8.r = (doublereal) n, z__8.i = 0.;
    z_div(&z__7, &z__8, &z__[3]);
    z__6.r = w[i__1].r + z__7.r, z__6.i = w[i__1].i + z__7.i;
    z__2.r = z__3.r * z__6.r - z__3.i * z__6.i, z__2.i = z__3.r * z__6.i + 
	    z__3.i * z__6.r;
    z_div(&z__1, &p24h24, &z__2);
    p24h24.r = z__1.r, p24h24.i = z__1.i;
    z__5.r = (doublereal) n, z__5.i = 0.;
    z_div(&z__4, &z__5, z__);
    z__3.r = dh1.r + z__4.r, z__3.i = dh1.i + z__4.i;
    i__1 = n * 3 - 1;
    z__8.r = (doublereal) n, z__8.i = 0.;
    z_div(&z__7, &z__8, &z__[3]);
    z__6.r = w[i__1].r + z__7.r, z__6.i = w[i__1].i + z__7.i;
    z__2.r = z__3.r * z__6.r - z__3.i * z__6.i, z__2.i = z__3.r * z__6.i + 
	    z__3.i * z__6.r;
    z_div(&z__1, &p24h21, &z__2);
    p24h21.r = z__1.r, p24h21.i = z__1.i;
    i__1 = n - 1;
    z__2.r = k3.r * acap[i__1].r - k3.i * acap[i__1].i, z__2.i = k3.r * acap[
	    i__1].i + k3.i * acap[i__1].r;
    i__2 = n * 3 - 3;
    z__3.r = k2.r * w[i__2].r - k2.i * w[i__2].i, z__3.i = k2.r * w[i__2].i + 
	    k2.i * w[i__2].r;
    z__1.r = z__2.r - z__3.r, z__1.i = z__2.i - z__3.i;
    u[0].r = z__1.r, u[0].i = z__1.i;
    i__1 = n - 1;
    z__2.r = k3.r * acap[i__1].r - k3.i * acap[i__1].i, z__2.i = k3.r * acap[
	    i__1].i + k3.i * acap[i__1].r;
    z__3.r = k2.r * dh2.r - k2.i * dh2.i, z__3.i = k2.r * dh2.i + k2.i * 
	    dh2.r;
    z__1.r = z__2.r - z__3.r, z__1.i = z__2.i - z__3.i;
    u[1].r = z__1.r, u[1].i = z__1.i;
    i__1 = n - 1;
    z__2.r = k2.r * acap[i__1].r - k2.i * acap[i__1].i, z__2.i = k2.r * acap[
	    i__1].i + k2.i * acap[i__1].r;
    i__2 = n * 3 - 3;
    z__3.r = k3.r * w[i__2].r - k3.i * w[i__2].i, z__3.i = k3.r * w[i__2].i + 
	    k3.i * w[i__2].r;
    z__1.r = z__2.r - z__3.r, z__1.i = z__2.i - z__3.i;
    u[2].r = z__1.r, u[2].i = z__1.i;
    i__1 = n - 1;
    z__2.r = k2.r * acap[i__1].r - k2.i * acap[i__1].i, z__2.i = k2.r * acap[
	    i__1].i + k2.i * acap[i__1].r;
    z__3.r = k3.r * dh2.r - k3.i * dh2.i, z__3.i = k3.r * dh2.i + k3.i * 
	    dh2.r;
    z__1.r = z__2.r - z__3.r, z__1.i = z__2.i - z__3.i;
    u[3].r = z__1.r, u[3].i = z__1.i;
    i__1 = n * 3 - 1;
    z__2.r = k1.r * w[i__1].r - k1.i * w[i__1].i, z__2.i = k1.r * w[i__1].i + 
	    k1.i * w[i__1].r;
    i__2 = n * 3 - 2;
    z__3.r = k2.r * w[i__2].r - k2.i * w[i__2].i, z__3.i = k2.r * w[i__2].i + 
	    k2.i * w[i__2].r;
    z__1.r = z__2.r - z__3.r, z__1.i = z__2.i - z__3.i;
    u[4].r = z__1.r, u[4].i = z__1.i;
    i__1 = n * 3 - 1;
    z__2.r = k2.r * w[i__1].r - k2.i * w[i__1].i, z__2.i = k2.r * w[i__1].i + 
	    k2.i * w[i__1].r;
    i__2 = n * 3 - 2;
    z__3.r = k1.r * w[i__2].r - k1.i * w[i__2].i, z__3.i = k1.r * w[i__2].i + 
	    k1.i * w[i__2].r;
    z__1.r = z__2.r - z__3.r, z__1.i = z__2.i - z__3.i;
    u[5].r = z__1.r, u[5].i = z__1.i;
    z__2.r = -ci.r, z__2.i = -ci.i;
    z__4.r = dummy.r * p24h21.r - dummy.i * p24h21.i, z__4.i = dummy.r * 
	    p24h21.i + dummy.i * p24h21.r;
    z__3.r = z__4.r - p24h24.r, z__3.i = z__4.i - p24h24.i;
    z__1.r = z__2.r * z__3.r - z__2.i * z__3.i, z__1.i = z__2.r * z__3.i + 
	    z__2.i * z__3.r;
    u[6].r = z__1.r, u[6].i = z__1.i;
    z__2.r = ta[2], z__2.i = 0.;
    z_div(&z__1, &z__2, &wfn[1]);
    u[7].r = z__1.r, u[7].i = z__1.i;
    z__6.r = u[0].r * u[4].r - u[0].i * u[4].i, z__6.i = u[0].r * u[4].i + u[
	    0].i * u[4].r;
    z__5.r = z__6.r * u[6].r - z__6.i * u[6].i, z__5.i = z__6.r * u[6].i + 
	    z__6.i * u[6].r;
    z__7.r = k1.r * u[0].r - k1.i * u[0].i, z__7.i = k1.r * u[0].i + k1.i * u[
	    0].r;
    z__4.r = z__5.r + z__7.r, z__4.i = z__5.i + z__7.i;
    z__9.r = dumsq.r * k3.r - dumsq.i * k3.i, z__9.i = dumsq.r * k3.i + 
	    dumsq.i * k3.r;
    z__8.r = z__9.r * u[4].r - z__9.i * u[4].i, z__8.i = z__9.r * u[4].i + 
	    z__9.i * u[4].r;
    z__3.r = z__4.r - z__8.r, z__3.i = z__4.i - z__8.i;
    z__2.r = u[7].r * z__3.r - u[7].i * z__3.i, z__2.i = u[7].r * z__3.i + u[
	    7].i * z__3.r;
    z__13.r = u[1].r * u[4].r - u[1].i * u[4].i, z__13.i = u[1].r * u[4].i + 
	    u[1].i * u[4].r;
    z__12.r = z__13.r * u[6].r - z__13.i * u[6].i, z__12.i = z__13.r * u[6].i 
	    + z__13.i * u[6].r;
    z__14.r = k1.r * u[1].r - k1.i * u[1].i, z__14.i = k1.r * u[1].i + k1.i * 
	    u[1].r;
    z__11.r = z__12.r + z__14.r, z__11.i = z__12.i + z__14.i;
    z__16.r = dumsq.r * k3.r - dumsq.i * k3.i, z__16.i = dumsq.r * k3.i + 
	    dumsq.i * k3.r;
    z__15.r = z__16.r * u[4].r - z__16.i * u[4].i, z__15.i = z__16.r * u[4].i 
	    + z__16.i * u[4].r;
    z__10.r = z__11.r - z__15.r, z__10.i = z__11.i - z__15.i;
    z_div(&z__1, &z__2, &z__10);
    acoe.r = z__1.r, acoe.i = z__1.i;
    z__6.r = u[2].r * u[5].r - u[2].i * u[5].i, z__6.i = u[2].r * u[5].i + u[
	    2].i * u[5].r;
    z__5.r = z__6.r * u[6].r - z__6.i * u[6].i, z__5.i = z__6.r * u[6].i + 
	    z__6.i * u[6].r;
    z__7.r = k2.r * u[2].r - k2.i * u[2].i, z__7.i = k2.r * u[2].i + k2.i * u[
	    2].r;
    z__4.r = z__5.r + z__7.r, z__4.i = z__5.i + z__7.i;
    z__9.r = dumsq.r * k2.r - dumsq.i * k2.i, z__9.i = dumsq.r * k2.i + 
	    dumsq.i * k2.r;
    z__8.r = z__9.r * u[5].r - z__9.i * u[5].i, z__8.i = z__9.r * u[5].i + 
	    z__9.i * u[5].r;
    z__3.r = z__4.r - z__8.r, z__3.i = z__4.i - z__8.i;
    z__2.r = u[7].r * z__3.r - u[7].i * z__3.i, z__2.i = u[7].r * z__3.i + u[
	    7].i * z__3.r;
    z__13.r = u[3].r * u[5].r - u[3].i * u[5].i, z__13.i = u[3].r * u[5].i + 
	    u[3].i * u[5].r;
    z__12.r = z__13.r * u[6].r - z__13.i * u[6].i, z__12.i = z__13.r * u[6].i 
	    + z__13.i * u[6].r;
    z__14.r = k2.r * u[3].r - k2.i * u[3].i, z__14.i = k2.r * u[3].i + k2.i * 
	    u[3].r;
    z__11.r = z__12.r + z__14.r, z__11.i = z__12.i + z__14.i;
    z__16.r = dumsq.r * k2.r - dumsq.i * k2.i, z__16.i = dumsq.r * k2.i + 
	    dumsq.i * k2.r;
    z__15.r = z__16.r * u[5].r - z__16.i * u[5].i, z__15.i = z__16.r * u[5].i 
	    + z__16.i * u[5].r;
    z__10.r = z__11.r - z__15.r, z__10.i = z__11.i - z__15.i;
    z_div(&z__1, &z__2, &z__10);
    bcoe.r = z__1.r, bcoe.i = z__1.i;
    acoem1.r = acoe.r, acoem1.i = acoe.i;
    bcoem1.r = bcoe.r, bcoem1.i = bcoe.i;
    are = acoe.r;
    aim = d_imag(&acoe);
    bre = bcoe.r;
    bim = d_imag(&bcoe);
    dqext = (are + bre) * 3.;
/* Computing 2nd power */
    d__1 = are;
/* Computing 2nd power */
    d__2 = aim;
/* Computing 2nd power */
    d__3 = bre;
/* Computing 2nd power */
    d__4 = bim;
    dqsca = (d__1 * d__1 + d__2 * d__2 + d__3 * d__3 + d__4 * d__4) * 3.;
    dgqsc = 0.;
    z__2.r = acoe.r - bcoe.r, z__2.i = acoe.i - bcoe.i;
    z__1.r = z__2.r * 3., z__1.i = z__2.i * 3.;
    sback.r = z__1.r, sback.i = z__1.i;
    rmm = 1.;
    z__1.r = acoe.r * 1.5, z__1.i = acoe.i * 1.5;
    ac.r = z__1.r, ac.i = z__1.i;
    z__1.r = bcoe.r * 1.5, z__1.i = bcoe.i * 1.5;
    bc.r = z__1.r, bc.i = z__1.i;
    i__1 = *numang;
    for (j = 1; j <= i__1; ++j) {
	i__2 = j - 1;
	i__3 = j + 99;
	z__2.r = pi[i__3] * ac.r, z__2.i = pi[i__3] * ac.i;
	i__4 = j + 99;
	z__3.r = tau[i__4] * bc.r, z__3.i = tau[i__4] * bc.i;
	z__1.r = z__2.r + z__3.r, z__1.i = z__2.i + z__3.i;
	s1[i__2].r = z__1.r, s1[i__2].i = z__1.i;
	i__2 = j + 99;
	i__3 = j + 99;
	z__2.r = pi[i__3] * ac.r, z__2.i = pi[i__3] * ac.i;
	i__4 = j + 99;
	z__3.r = tau[i__4] * bc.r, z__3.i = tau[i__4] * bc.i;
	z__1.r = z__2.r - z__3.r, z__1.i = z__2.i - z__3.i;
	s1[i__2].r = z__1.r, s1[i__2].i = z__1.i;
	i__2 = j - 1;
	i__3 = j + 99;
	z__2.r = pi[i__3] * bc.r, z__2.i = pi[i__3] * bc.i;
	i__4 = j + 99;
	z__3.r = tau[i__4] * ac.r, z__3.i = tau[i__4] * ac.i;
	z__1.r = z__2.r + z__3.r, z__1.i = z__2.i + z__3.i;
	s2[i__2].r = z__1.r, s2[i__2].i = z__1.i;
	i__2 = j + 99;
	i__3 = j + 99;
	z__2.r = pi[i__3] * bc.r, z__2.i = pi[i__3] * bc.i;
	i__4 = j + 99;
	z__3.r = tau[i__4] * ac.r, z__3.i = tau[i__4] * ac.i;
	z__1.r = z__2.r - z__3.r, z__1.i = z__2.i - z__3.i;
	s2[i__2].r = z__1.r, s2[i__2].i = z__1.i;
/* L60: */
    }
/* ***************** Start of Mie summing loop ****************** */
    n = 2;
L70:
/*                              ** Recurrences for functions little-pi, */
/*                                 little-tau of Mie theory */
    t[0] = (doublereal) ((n << 1) - 1);
    t[1] = (doublereal) (n - 1);
    i__1 = *numang;
    for (j = 1; j <= i__1; ++j) {
	pi[j + 199] = (t[0] * pi[j + 99] * mu[j] - n * pi[j - 1]) / t[1];
	tau[j + 199] = mu[j] * (pi[j + 199] - pi[j - 1]) - t[0] * si2tht[j - 
		1] * pi[j + 99] + tau[j - 1];
/* L80: */
    }
/*                                 ** Here set up homogeneous sphere */
    wm1.r = wfn[0].r, wm1.i = wfn[0].i;
    wfn[0].r = wfn[1].r, wfn[0].i = wfn[1].i;
    d__1 = t[0] * rx;
    z__2.r = d__1 * wfn[0].r, z__2.i = d__1 * wfn[0].i;
    z__1.r = z__2.r - wm1.r, z__1.i = z__2.i - wm1.i;
    wfn[1].r = z__1.r, wfn[1].i = z__1.i;
    ta[0] = wfn[0].r;
    ta[1] = d_imag(wfn);
    ta[2] = wfn[1].r;
    ta[3] = d_imag(&wfn[1]);
/*                                 ** Here set up stratified sphere */
    i__1 = -n;
    z__3.r = (doublereal) i__1, z__3.i = 0.;
    z_div(&z__2, &z__3, z__);
    z__7.r = (doublereal) n, z__7.i = 0.;
    z_div(&z__6, &z__7, z__);
    z__5.r = z__6.r - dh1.r, z__5.i = z__6.i - dh1.i;
    z_div(&z__4, &c_b16, &z__5);
    z__1.r = z__2.r + z__4.r, z__1.i = z__2.i + z__4.i;
    dh1.r = z__1.r, dh1.i = z__1.i;
    i__1 = -n;
    z__3.r = (doublereal) i__1, z__3.i = 0.;
    z_div(&z__2, &z__3, &z__[1]);
    z__7.r = (doublereal) n, z__7.i = 0.;
    z_div(&z__6, &z__7, &z__[1]);
    z__5.r = z__6.r - dh2.r, z__5.i = z__6.i - dh2.i;
    z_div(&z__4, &c_b16, &z__5);
    z__1.r = z__2.r + z__4.r, z__1.i = z__2.i + z__4.i;
    dh2.r = z__1.r, dh2.i = z__1.i;
    i__1 = -n;
    z__3.r = (doublereal) i__1, z__3.i = 0.;
    z_div(&z__2, &z__3, &z__[3]);
    z__7.r = (doublereal) n, z__7.i = 0.;
    z_div(&z__6, &z__7, &z__[3]);
    z__5.r = z__6.r - dh4.r, z__5.i = z__6.i - dh4.i;
    z_div(&z__4, &c_b16, &z__5);
    z__1.r = z__2.r + z__4.r, z__1.i = z__2.i + z__4.i;
    dh4.r = z__1.r, dh4.i = z__1.i;
    z__5.r = (doublereal) n, z__5.i = 0.;
    z_div(&z__4, &z__5, &z__[3]);
    z__3.r = dh4.r + z__4.r, z__3.i = dh4.i + z__4.i;
    i__1 = n * 3 - 1;
    z__8.r = (doublereal) n, z__8.i = 0.;
    z_div(&z__7, &z__8, &z__[3]);
    z__6.r = w[i__1].r + z__7.r, z__6.i = w[i__1].i + z__7.i;
    z__2.r = z__3.r * z__6.r - z__3.i * z__6.i, z__2.i = z__3.r * z__6.i + 
	    z__3.i * z__6.r;
    z_div(&z__1, &p24h24, &z__2);
    p24h24.r = z__1.r, p24h24.i = z__1.i;
    z__5.r = (doublereal) n, z__5.i = 0.;
    z_div(&z__4, &z__5, z__);
    z__3.r = dh1.r + z__4.r, z__3.i = dh1.i + z__4.i;
    i__1 = n * 3 - 1;
    z__8.r = (doublereal) n, z__8.i = 0.;
    z_div(&z__7, &z__8, &z__[3]);
    z__6.r = w[i__1].r + z__7.r, z__6.i = w[i__1].i + z__7.i;
    z__2.r = z__3.r * z__6.r - z__3.i * z__6.i, z__2.i = z__3.r * z__6.i + 
	    z__3.i * z__6.r;
    z_div(&z__1, &p24h21, &z__2);
    p24h21.r = z__1.r, p24h21.i = z__1.i;
    i__1 = n - 1;
    z__5.r = (doublereal) n, z__5.i = 0.;
    z_div(&z__4, &z__5, z__);
    z__3.r = acap[i__1].r + z__4.r, z__3.i = acap[i__1].i + z__4.i;
    z__2.r = dummy.r * z__3.r - dummy.i * z__3.i, z__2.i = dummy.r * z__3.i + 
	    dummy.i * z__3.r;
    i__2 = n * 3 - 1;
    z__8.r = (doublereal) n, z__8.i = 0.;
    z_div(&z__7, &z__8, &z__[3]);
    z__6.r = w[i__2].r + z__7.r, z__6.i = w[i__2].i + z__7.i;
    z_div(&z__1, &z__2, &z__6);
    dummy.r = z__1.r, dummy.i = z__1.i;
    pow_zi(&z__1, &dummy, &c__2);
    dumsq.r = z__1.r, dumsq.i = z__1.i;
    i__1 = n - 1;
    z__2.r = k3.r * acap[i__1].r - k3.i * acap[i__1].i, z__2.i = k3.r * acap[
	    i__1].i + k3.i * acap[i__1].r;
    i__2 = n * 3 - 3;
    z__3.r = k2.r * w[i__2].r - k2.i * w[i__2].i, z__3.i = k2.r * w[i__2].i + 
	    k2.i * w[i__2].r;
    z__1.r = z__2.r - z__3.r, z__1.i = z__2.i - z__3.i;
    u[0].r = z__1.r, u[0].i = z__1.i;
    i__1 = n - 1;
    z__2.r = k3.r * acap[i__1].r - k3.i * acap[i__1].i, z__2.i = k3.r * acap[
	    i__1].i + k3.i * acap[i__1].r;
    z__3.r = k2.r * dh2.r - k2.i * dh2.i, z__3.i = k2.r * dh2.i + k2.i * 
	    dh2.r;
    z__1.r = z__2.r - z__3.r, z__1.i = z__2.i - z__3.i;
    u[1].r = z__1.r, u[1].i = z__1.i;
    i__1 = n - 1;
    z__2.r = k2.r * acap[i__1].r - k2.i * acap[i__1].i, z__2.i = k2.r * acap[
	    i__1].i + k2.i * acap[i__1].r;
    i__2 = n * 3 - 3;
    z__3.r = k3.r * w[i__2].r - k3.i * w[i__2].i, z__3.i = k3.r * w[i__2].i + 
	    k3.i * w[i__2].r;
    z__1.r = z__2.r - z__3.r, z__1.i = z__2.i - z__3.i;
    u[2].r = z__1.r, u[2].i = z__1.i;
    i__1 = n - 1;
    z__2.r = k2.r * acap[i__1].r - k2.i * acap[i__1].i, z__2.i = k2.r * acap[
	    i__1].i + k2.i * acap[i__1].r;
    z__3.r = k3.r * dh2.r - k3.i * dh2.i, z__3.i = k3.r * dh2.i + k3.i * 
	    dh2.r;
    z__1.r = z__2.r - z__3.r, z__1.i = z__2.i - z__3.i;
    u[3].r = z__1.r, u[3].i = z__1.i;
    i__1 = n * 3 - 1;
    z__2.r = k1.r * w[i__1].r - k1.i * w[i__1].i, z__2.i = k1.r * w[i__1].i + 
	    k1.i * w[i__1].r;
    i__2 = n * 3 - 2;
    z__3.r = k2.r * w[i__2].r - k2.i * w[i__2].i, z__3.i = k2.r * w[i__2].i + 
	    k2.i * w[i__2].r;
    z__1.r = z__2.r - z__3.r, z__1.i = z__2.i - z__3.i;
    u[4].r = z__1.r, u[4].i = z__1.i;
    i__1 = n * 3 - 1;
    z__2.r = k2.r * w[i__1].r - k2.i * w[i__1].i, z__2.i = k2.r * w[i__1].i + 
	    k2.i * w[i__1].r;
    i__2 = n * 3 - 2;
    z__3.r = k1.r * w[i__2].r - k1.i * w[i__2].i, z__3.i = k1.r * w[i__2].i + 
	    k1.i * w[i__2].r;
    z__1.r = z__2.r - z__3.r, z__1.i = z__2.i - z__3.i;
    u[5].r = z__1.r, u[5].i = z__1.i;
    z__2.r = -ci.r, z__2.i = -ci.i;
    z__4.r = dummy.r * p24h21.r - dummy.i * p24h21.i, z__4.i = dummy.r * 
	    p24h21.i + dummy.i * p24h21.r;
    z__3.r = z__4.r - p24h24.r, z__3.i = z__4.i - p24h24.i;
    z__1.r = z__2.r * z__3.r - z__2.i * z__3.i, z__1.i = z__2.r * z__3.i + 
	    z__2.i * z__3.r;
    u[6].r = z__1.r, u[6].i = z__1.i;
    z__2.r = ta[2], z__2.i = 0.;
    z_div(&z__1, &z__2, &wfn[1]);
    u[7].r = z__1.r, u[7].i = z__1.i;
    z__6.r = u[0].r * u[4].r - u[0].i * u[4].i, z__6.i = u[0].r * u[4].i + u[
	    0].i * u[4].r;
    z__5.r = z__6.r * u[6].r - z__6.i * u[6].i, z__5.i = z__6.r * u[6].i + 
	    z__6.i * u[6].r;
    z__7.r = k1.r * u[0].r - k1.i * u[0].i, z__7.i = k1.r * u[0].i + k1.i * u[
	    0].r;
    z__4.r = z__5.r + z__7.r, z__4.i = z__5.i + z__7.i;
    z__9.r = dumsq.r * k3.r - dumsq.i * k3.i, z__9.i = dumsq.r * k3.i + 
	    dumsq.i * k3.r;
    z__8.r = z__9.r * u[4].r - z__9.i * u[4].i, z__8.i = z__9.r * u[4].i + 
	    z__9.i * u[4].r;
    z__3.r = z__4.r - z__8.r, z__3.i = z__4.i - z__8.i;
    z__2.r = u[7].r * z__3.r - u[7].i * z__3.i, z__2.i = u[7].r * z__3.i + u[
	    7].i * z__3.r;
    z__13.r = u[1].r * u[4].r - u[1].i * u[4].i, z__13.i = u[1].r * u[4].i + 
	    u[1].i * u[4].r;
    z__12.r = z__13.r * u[6].r - z__13.i * u[6].i, z__12.i = z__13.r * u[6].i 
	    + z__13.i * u[6].r;
    z__14.r = k1.r * u[1].r - k1.i * u[1].i, z__14.i = k1.r * u[1].i + k1.i * 
	    u[1].r;
    z__11.r = z__12.r + z__14.r, z__11.i = z__12.i + z__14.i;
    z__16.r = dumsq.r * k3.r - dumsq.i * k3.i, z__16.i = dumsq.r * k3.i + 
	    dumsq.i * k3.r;
    z__15.r = z__16.r * u[4].r - z__16.i * u[4].i, z__15.i = z__16.r * u[4].i 
	    + z__16.i * u[4].r;
    z__10.r = z__11.r - z__15.r, z__10.i = z__11.i - z__15.i;
    z_div(&z__1, &z__2, &z__10);
    acoe.r = z__1.r, acoe.i = z__1.i;
    z__6.r = u[2].r * u[5].r - u[2].i * u[5].i, z__6.i = u[2].r * u[5].i + u[
	    2].i * u[5].r;
    z__5.r = z__6.r * u[6].r - z__6.i * u[6].i, z__5.i = z__6.r * u[6].i + 
	    z__6.i * u[6].r;
    z__7.r = k2.r * u[2].r - k2.i * u[2].i, z__7.i = k2.r * u[2].i + k2.i * u[
	    2].r;
    z__4.r = z__5.r + z__7.r, z__4.i = z__5.i + z__7.i;
    z__9.r = dumsq.r * k2.r - dumsq.i * k2.i, z__9.i = dumsq.r * k2.i + 
	    dumsq.i * k2.r;
    z__8.r = z__9.r * u[5].r - z__9.i * u[5].i, z__8.i = z__9.r * u[5].i + 
	    z__9.i * u[5].r;
    z__3.r = z__4.r - z__8.r, z__3.i = z__4.i - z__8.i;
    z__2.r = u[7].r * z__3.r - u[7].i * z__3.i, z__2.i = u[7].r * z__3.i + u[
	    7].i * z__3.r;
    z__13.r = u[3].r * u[5].r - u[3].i * u[5].i, z__13.i = u[3].r * u[5].i + 
	    u[3].i * u[5].r;
    z__12.r = z__13.r * u[6].r - z__13.i * u[6].i, z__12.i = z__13.r * u[6].i 
	    + z__13.i * u[6].r;
    z__14.r = k2.r * u[3].r - k2.i * u[3].i, z__14.i = k2.r * u[3].i + k2.i * 
	    u[3].r;
    z__11.r = z__12.r + z__14.r, z__11.i = z__12.i + z__14.i;
    z__16.r = dumsq.r * k2.r - dumsq.i * k2.i, z__16.i = dumsq.r * k2.i + 
	    dumsq.i * k2.r;
    z__15.r = z__16.r * u[5].r - z__16.i * u[5].i, z__15.i = z__16.r * u[5].i 
	    + z__16.i * u[5].r;
    z__10.r = z__11.r - z__15.r, z__10.i = z__11.i - z__15.i;
    z_div(&z__1, &z__2, &z__10);
    bcoe.r = z__1.r, bcoe.i = z__1.i;
    are = acoe.r;
    aim = d_imag(&acoe);
    bre = bcoe.r;
    bim = d_imag(&bcoe);
/*                           ** Increment sums for efficiency factors */
    am1re = acoem1.r;
    am1im = d_imag(&acoem1);
    bm1re = bcoem1.r;
    bm1im = d_imag(&bcoem1);
    t[3] = ((n << 1) - 1.) / (n * (n - 1.));
    t[1] = (n - 1.) * (n + 1.) / n;
    dgqsc = dgqsc + t[1] * (am1re * are + am1im * aim + bm1re * bre + bm1im * 
	    bim) + t[3] * (am1re * bm1re + am1im * bm1im);
    t[2] = (doublereal) ((n << 1) + 1);
    dqext += t[2] * (are + bre);
/* Computing 2nd power */
    d__1 = are;
/* Computing 2nd power */
    d__2 = aim;
/* Computing 2nd power */
    d__3 = bre;
/* Computing 2nd power */
    d__4 = bim;
    t[3] = d__1 * d__1 + d__2 * d__2 + d__3 * d__3 + d__4 * d__4;
    dqsca += t[2] * t[3];
    rmm = -rmm;
    d__1 = t[2] * rmm;
    z__3.r = acoe.r - bcoe.r, z__3.i = acoe.i - bcoe.i;
    z__2.r = d__1 * z__3.r, z__2.i = d__1 * z__3.i;
    z__1.r = sback.r + z__2.r, z__1.i = sback.i + z__2.i;
    sback.r = z__1.r, sback.i = z__1.i;
    t[1] = (doublereal) (n * (n + 1));
    t[0] = t[2] / t[1];
    z__1.r = t[0] * acoe.r, z__1.i = t[0] * acoe.i;
    ac.r = z__1.r, ac.i = z__1.i;
    z__1.r = t[0] * bcoe.r, z__1.i = t[0] * bcoe.i;
    bc.r = z__1.r, bc.i = z__1.i;
    i__1 = *numang;
    for (j = 1; j <= i__1; ++j) {
	i__2 = j - 1;
	i__3 = j - 1;
	i__4 = j + 199;
	z__3.r = pi[i__4] * ac.r, z__3.i = pi[i__4] * ac.i;
	z__2.r = s1[i__3].r + z__3.r, z__2.i = s1[i__3].i + z__3.i;
	i__5 = j + 199;
	z__4.r = tau[i__5] * bc.r, z__4.i = tau[i__5] * bc.i;
	z__1.r = z__2.r + z__4.r, z__1.i = z__2.i + z__4.i;
	s1[i__2].r = z__1.r, s1[i__2].i = z__1.i;
	i__2 = j - 1;
	i__3 = j - 1;
	i__4 = j + 199;
	z__3.r = pi[i__4] * bc.r, z__3.i = pi[i__4] * bc.i;
	z__2.r = s2[i__3].r + z__3.r, z__2.i = s2[i__3].i + z__3.i;
	i__5 = j + 199;
	z__4.r = tau[i__5] * ac.r, z__4.i = tau[i__5] * ac.i;
	z__1.r = z__2.r + z__4.r, z__1.i = z__2.i + z__4.i;
	s2[i__2].r = z__1.r, s2[i__2].i = z__1.i;
/* L90: */
    }
/*                               ** Scattering matrix elements for */
/*                                  supplements of 0-90 degree scattering */
/*                                  angles submitted by user */
    if (n % 2 == 0) {
	i__1 = *numang;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = j + 99;
	    i__3 = j + 99;
	    i__4 = j + 199;
	    z__3.r = pi[i__4] * ac.r, z__3.i = pi[i__4] * ac.i;
	    z__2.r = s1[i__3].r - z__3.r, z__2.i = s1[i__3].i - z__3.i;
	    i__5 = j + 199;
	    z__4.r = tau[i__5] * bc.r, z__4.i = tau[i__5] * bc.i;
	    z__1.r = z__2.r + z__4.r, z__1.i = z__2.i + z__4.i;
	    s1[i__2].r = z__1.r, s1[i__2].i = z__1.i;
	    i__2 = j + 99;
	    i__3 = j + 99;
	    i__4 = j + 199;
	    z__3.r = pi[i__4] * bc.r, z__3.i = pi[i__4] * bc.i;
	    z__2.r = s2[i__3].r - z__3.r, z__2.i = s2[i__3].i - z__3.i;
	    i__5 = j + 199;
	    z__4.r = tau[i__5] * ac.r, z__4.i = tau[i__5] * ac.i;
	    z__1.r = z__2.r + z__4.r, z__1.i = z__2.i + z__4.i;
	    s2[i__2].r = z__1.r, s2[i__2].i = z__1.i;
/* L100: */
	}
    } else {
	i__1 = *numang;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = j + 99;
	    i__3 = j + 99;
	    i__4 = j + 199;
	    z__3.r = pi[i__4] * ac.r, z__3.i = pi[i__4] * ac.i;
	    z__2.r = s1[i__3].r + z__3.r, z__2.i = s1[i__3].i + z__3.i;
	    i__5 = j + 199;
	    z__4.r = tau[i__5] * bc.r, z__4.i = tau[i__5] * bc.i;
	    z__1.r = z__2.r - z__4.r, z__1.i = z__2.i - z__4.i;
	    s1[i__2].r = z__1.r, s1[i__2].i = z__1.i;
	    i__2 = j + 99;
	    i__3 = j + 99;
	    i__4 = j + 199;
	    z__3.r = pi[i__4] * bc.r, z__3.i = pi[i__4] * bc.i;
	    z__2.r = s2[i__3].r + z__3.r, z__2.i = s2[i__3].i + z__3.i;
	    i__5 = j + 199;
	    z__4.r = tau[i__5] * ac.r, z__4.i = tau[i__5] * ac.i;
	    z__1.r = z__2.r - z__4.r, z__1.i = z__2.i - z__4.i;
	    s2[i__2].r = z__1.r, s2[i__2].i = z__1.i;
/* L110: */
	}
    }
/*                                      ** Test for convergence of sums */
    if (t[3] >= 1e-14) {
	++n;
	if (n > nmx2) {
	    derrmsg_("MIELAY--Dimensions for W,ACAP not enough. Suggest get d"
		    "etailed output, modify routine", &c_true, (ftnlen)85);
	}
	i__1 = *numang;
	for (j = 1; j <= i__1; ++j) {
	    pi[j - 1] = pi[j + 99];
	    pi[j + 99] = pi[j + 199];
	    tau[j - 1] = tau[j + 99];
	    tau[j + 99] = tau[j + 199];
/* L120: */
	}
	acoem1.r = acoe.r, acoem1.i = acoe.i;
	bcoem1.r = bcoe.r, bcoem1.i = bcoe.i;
	goto L70;
    }
/* ***************** End of summing loop ****************** */
/*                            ** Transform complex scattering amplitudes */
/*                               into elements of real scattering matrix */
    i__1 = *numang;
    for (j = 1; j <= i__1; ++j) {
	for (k = 1; k <= 2; ++k) {
	    i__2 = j + k * 100 - 101;
/* Computing 2nd power */
	    d__1 = s1[i__2].r;
/* Computing 2nd power */
	    d__2 = d_imag(&s1[j + k * 100 - 101]);
	    m1[j + k * m1_dim1] = (real)(d__1 * d__1 + d__2 * d__2);
	    i__2 = j + k * 100 - 101;
/* Computing 2nd power */
	    d__1 = s2[i__2].r;
/* Computing 2nd power */
	    d__2 = d_imag(&s2[j + k * 100 - 101]);
	    m2[j + k * m2_dim1] = (real)(d__1 * d__1 + d__2 * d__2);
	    i__2 = j + k * 100 - 101;
	    i__3 = j + k * 100 - 101;
	    s21[j + k * s21_dim1] = (real)(s1[i__2].r * s2[i__3].r + d_imag(&s1[j + 
		    k * 100 - 101]) * d_imag(&s2[j + k * 100 - 101]));
	    i__2 = j + k * 100 - 101;
	    i__3 = j + k * 100 - 101;
	    d21[j + k * d21_dim1] = (real)(d_imag(&s1[j + k * 100 - 101]) * s2[i__2]
		    .r - d_imag(&s2[j + k * 100 - 101]) * s1[i__3].r);
/* L130: */
	}
/* L140: */
    }
/* Computing 2nd power */
    d__1 = rx;
    t[0] = d__1 * d__1 * 2.;
    *qext = (real)(t[0] * dqext);
    *qsca = (real)(t[0] * dqsca);
    *gqsc = (real)(t[0] * 2. * dgqsc);
    z__1.r = sback.r * .5f, z__1.i = sback.i * .5f;
    sback.r = z__1.r, sback.i = z__1.i;
/* Computing 2nd power */
    d__1 = sback.r;
/* Computing 2nd power */
    d__2 = d_imag(&sback);
/* Computing 2nd power */
    d__3 = xshell;
    *qbs = (real)((d__1 * d__1 + d__2 * d__2) / (pinum * (d__3 * d__3)));
    return 0;
} /* dmilay_ */

int derrmsg_(char *messag, logical *fatal, ftnlen messag_len)
{
    /* Initialized data */

    static integer nummsg = 0;
    static integer maxmsg = 100;
    static logical msglim = FALSE_;

    /* Builtin functions */
    /*
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);
    int s_stop(char *, ftnlen);
    */

    /* Fortran I/O blocks */
    static cilist io___81 = { 0, 6, 0, "(//,2A,//)", 0 };
    static cilist io___82 = { 0, 6, 0, "(/,2A,/)", 0 };
    static cilist io___83 = { 0, 6, 0, "(//,A,//)", 0 };


/*        Print out a warning or error message;  abort if error */
/*        after making symbolic dump (machine-specific) */
/*       Provenance:  the 3 error-handlers ErrMsg, WrtBad, WrtDim are */
/*                    borrowed from MIEV, the Wiscombe Mie program */
/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Subroutines .. */
/* cccc EXTERNAL  SYMDUMP */
/*     .. */
/*     .. Save statement .. */
/*     .. */
/*     .. Data statements .. */
/*     .. */
    if (*fatal) {
	s_wsfe(&io___81);
	do_fio(&c__1, " ****** ERROR *****  ", (ftnlen)21);
	do_fio(&c__1, messag, messag_len);
	e_wsfe();
/*                                 ** Example symbolic dump call for Cray */
/* cccc    CALL SYMDUMP( '-B -c3' ) */
	s_stop("", (ftnlen)0);
    }
    ++nummsg;
    if (msglim) {
	return 0;
    }
    if (nummsg <= maxmsg) {
	s_wsfe(&io___82);
	do_fio(&c__1, " ****** WARNING *****  ", (ftnlen)23);
	do_fio(&c__1, messag, messag_len);
	e_wsfe();
    } else {
	s_wsfe(&io___83);
	do_fio(&c__1, " ****** TOO MANY WARNING MESSAGES --  They will no lo"
		"nger be printed *******", (ftnlen)76);
	e_wsfe();
	msglim = TRUE_;
    }
    return 0;
} /* derrmsg_ */

