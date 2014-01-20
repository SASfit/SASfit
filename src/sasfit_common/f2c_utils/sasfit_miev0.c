/*
 * src/sasfit_common/f2c_utils/sasfit_miev0.c
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

/* MIEV0.f -- translated by f2c (version 20041007).
   You must link the resulting object file with libf2c:
	on Microsoft Windows system, link with libf2c.lib;
	on Linux or Unix systems, link with .../path/to/libf2c.a -lm
	or, if you install libf2c.a in a standard place, with -lf2c -lm
	-- in that order, at the end of the command line, as in
		cc *.o -lf2c -lm
	Source for libf2c is in /netlib/f2c/libf2c.zip, e.g.,

		http://www.netlib.org/f2c/libf2c.zip

	formally modified by Ingo Bressler, 2008-06-06
*/

#include <gsl/gsl_math.h>
#include "../include/sasfit_utils_f2c.h"

int biga_(complex *, real *, integer *, logical *, logical *, real *, complex *);
int small1_(real *, integer *, real *, real *, real *, real *, complex *, complex *, 
		complex *, complex *, complex *, complex *, complex *, complex *);
int small2_(real *, complex *, logical *, integer *, real *, real *, real *, real *,
		complex *, complex *, complex *, complex *, complex *, complex *, 
		complex *, complex *);
int lpcoef_(integer *, integer *, integer *, integer *, logical *, integer *, complex *, 
		complex *, real *);
int ckinmi_(integer *, integer *, real *, logical *, complex *, integer *, integer *, 
		integer *, logical *, real *, logical *, integer *);
int testmi_(logical *, real *, complex *, real *, logical *, logical *, integer *, 
		integer *, integer *, real *, real *, real *, real *, complex *, 
		complex *, complex *, complex *, complex *, complex *, real *, integer *);
int miprnt_(logical *, real *, logical *, complex *, integer *, real *, real *, real *,
		real *, integer *, integer *, integer *, logical *, real *, 
		complex *, complex *, complex *, complex *, complex *, complex *);
int lpco1t_(integer *, integer *, integer *, logical *, complex *, complex *, real *);
int lpco2t_(integer *, integer *, integer *, logical *, complex *, complex *, real *);
void confra_(complex *, integer *, complex *);

/* Table of constant values */

static logical c_false = FALSE_;
static integer c__501 = 501;
static complex c_b6 = {1.f,0.f};
static doublereal c_b7 = .33333333333333331;
static logical c_true = TRUE_;
static integer c__1 = 1;
static integer c__2 = 2;
static real c_b153 = 0.f;
static real c_b158 = 180.f;

int sasfit_miev0(real *xx, complex *crefin, logical *perfct, real *mimcut, 
		logical *anyang, integer *numang, real *xmu, integer *nmom, 
		integer *ipolzn, integer *momdim, logical *prnt, real *qext, 
		real *qsca, real *gqsc, real *pmom, complex *sforw, complex *sback, 
		complex *s1, complex *s2, complex *tforw, complex *tback, real *spike)
{
    /* Initialized data */

    static logical pass1 = TRUE_;

    /* System generated locals */
    integer pmom_dim1, pmom_offset, i__1, i__2, i__3, i__4;
    real r__1, r__2, r__3, r__4;
    doublereal d__1;
    complex q__1, q__2, q__3, q__4, q__5, q__6, q__7;
    logical L__1;

    /* Builtin functions */
    /*
    double r_imag(complex *);
    void r_cnjg(complex *, complex *), c_div(complex *, complex *, complex *);
    double c_abs(complex *), pow_dd(doublereal *, doublereal *), sin(
	    doublereal), cos(doublereal);
*/

    /* Local variables */
    static integer i__, j, n;
    static complex an, bn;
    static real fn, mm, rn;
    static complex sm[501], sp[501], anp, bnp;
    static real mim, mre, pin[501];
    static complex zet, sms[251], sps[251], anm1, bnm1;
    static real chin;
    static complex lita[10100], litb[10100], anpm, bnpm, cior;
    static real taun, psin;
    static integer ntrm;
    static real rtmp, xinv;
    static complex zetn;
    static real np1dn, pinm1[501];
    static complex csum1, csum2, cbiga[10100];
    static real coeff, denan, denbn, rbiga[10100], tcoef;
    static logical noabs;
    static real ratio;
    static integer nangd2;
    static real chinm1;
    static real psinm1;
    static complex zetnm1;
    static real twonp1;
    static complex cdenan, cdenbn;
    static logical calcmo[4];
    static logical yesang;
    static complex cioriv;
    static integer npquan;
    static real rioriv;

/*    Computes Mie scattering and extinction efficiencies; asymmetry */
/*    factor;  forward- and backscatter amplitude;  scattering */
/*    amplitudes vs. scattering angle for incident polarization parallel */
/*    and perpendicular to the plane of scattering; */
/*    coefficients in the Legendre polynomial expansions of either the */
/*    unpolarized phase function or the polarized phase matrix; */
/*    some quantities needed in polarized radiative transfer;  and */
/*    information about whether or not a resonance has been hit. */
/*    Input and output variables are described in file MIEV.doc. */
/*    Many statements are accompanied by comments referring to */
/*    references in MIEV.doc, notably the NCAR Mie report which is now */
/*    available electronically and which is referred to using the */
/*    shorthand (Rn), meaning Eq. (n) of the report. */
/*    CALLING TREE: */
/*        MIEV0 */
/*            TESTMI */
/*                TSTBAD */
/*                MIPRNT */
/*                ERRMSG */
/*            CKINMI */
/*                WRTBAD */
/*                WRTDIM */
/*                ERRMSG */
/*            SMALL1 */
/*            SMALL2 */
/*            ERRMSG */
/*            BIGA */
/*                CONFRA */
/*                    ERRMSG */
/*            LPCOEF */
/*                LPCO1T */
/*                LPCO2T */
/*                ERRMSG */
/*            MIPRNT */
/*      I N T E R N A L   V A R I A B L E S */
/*      ----------------------------------- */
/*  AN,BN           Mie coefficients a-sub-n, b-sub-n ( Ref. 1, Eq. 16 ) */
/*  ANM1,BNM1       Mie coefficients  a-sub-(n-1), */
/*                     b-sub-(n-1);  used in GQSC sum */
/*  ANP             Coeffs. in S+ expansion ( Ref. 2, p. 1507 ) */
/*  BNP             Coeffs. in S- expansion ( Ref. 2, p. 1507 ) */
/*  ANPM            Coeffs. in S+ expansion ( Ref. 2, p. 1507 ) */
/*                     when  MU  is replaced by  - MU */
/*  BNPM            Coeffs. in S- expansion ( Ref. 2, p. 1507 ) */
/*                     when  MU  is replaced by  - MU */
/*  CALCMO(K)       TRUE, calculate moments for K-th phase quantity */
/*                     (derived from IPOLZN) */
/*  CBIGA(N)        Bessel function ratio A-sub-N (Ref. 2, Eq. 2) */
/*                     ( COMPLEX version ) */
/*  CDENAN,         (COMPLEX) denominators of An,Bn */
/*   CDENBN */
/*  CIOR            Complex index of refraction with negative */
/*                     imaginary part (Van de Hulst convention) */
/*  CIORIV          1 / cIoR */
/*  COEFF           ( 2N + 1 ) / ( N ( N + 1 ) ) */
/*  CSUM1,2         temporary sum variables for TFORW, TBACK */
/*  FN              Floating point version of loop index for */
/*                     Mie series summation */
/*  LITA,LITB(N)    Mie coefficients An, Bn, saved in arrays for */
/*                     use in calculating Legendre moments PMOM */
/*  MAXTRM          Max. possible no. of terms in Mie series */
/*  MM              (-1)^(n+1), where n is Mie series sum index */
/*  MIM             Magnitude of imaginary refractive index */
/*  MRE             Real part of refractive index */
/*  MAXANG          Max. possible value of input variable NUMANG */
/*  NANGD2          (NUMANG+1)/2 ( no. of angles in 0-90 deg; ANYANG=F ) */
/*  NOABS           TRUE, sphere non-absorbing (determined by MIMCUT) */
/*  NP1DN           ( N + 1 ) / N */
/*  NPQUAN          Highest-numbered phase quantity for which moments are */
/*                     to be calculated (the largest digit in IPOLZN */
/*                     if  IPOLZN .NE. 0) */
/*  NTRM            No. of terms in Mie series */
/*  PASS1           TRUE on first entry, FALSE thereafter; for self-test */
/*  PIN(J)          Angular function pi-sub-n ( Ref. 2, Eq. 3 ) */
/*                     at J-th angle */
/*  PINM1(J)        pi-sub-(n-1) ( see PIn ) at J-th angle */
/*  PSINM1          Ricatti-Bessel function psi-sub-(n-1), argument XX */
/*  PSIN            Ricatti-Bessel function psi-sub-n of argument XX */
/*                     ( Ref. 1, p. 11 ff. ) */
/*  RBIGA(N)        Bessel function ratio A-sub-N (Ref. 2, Eq. 2) */
/*                     ( REAL version, for when imag refrac index = 0 ) */
/*  RIORIV          1 / Mre */
/*  RN              1 / N */
/*  RTMP            (REAL) temporary variable */
/*  SP(J)           S+  for J-th angle  ( Ref. 2, p. 1507 ) */
/*  SM(J)           S-  for J-TH angle  ( Ref. 2, p. 1507 ) */
/*  SPS(J)          S+  for (NUMANG+1-J)-th angle ( ANYANG=FALSE ) */
/*  SMS(J)          S-  for (NUMANG+1-J)-th angle ( ANYANG=FALSE ) */
/*  TAUN            Angular function tau-sub-n ( Ref. 2, Eq. 4 ) */
/*                     at J-th angle */
/*  TCOEF           N ( N+1 ) ( 2N+1 ) (for summing TFORW,TBACK series) */
/*  TWONP1          2N + 1 */
/*  YESANG          TRUE if scattering amplitudes are to be calculated */
/*  ZETNM1          Ricatti-Bessel function  zeta-sub-(n-1) of argument */
/*                     XX  ( Ref. 2, Eq. 17 ) */
/*  ZETN            Ricatti-Bessel function  zeta-sub-n of argument XX */
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
/* --------  I / O SPECIFICATIONS FOR SUBROUTINE MIEV0  ----------------- */
/* ---------------------------------------------------------------------- */
/* ---------------------------------------------------------------------- */
/*                                  ** NOTE --  MAXTRM = 10100  is neces- */
/*                                  ** sary to do some of the test probs, */
/*                                  ** but 1100 is sufficient for most */
/*                                  ** conceivable applications */
/*     .. Parameters .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Local Arrays .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Statement Functions .. */
/*     .. */
/*     .. Statement Function definitions .. */
/*     .. */
    /* Parameter adjustments */
    --xmu;
    pmom_dim1 = *momdim - 0 + 1;
    pmom_offset = 0 + pmom_dim1;
    pmom -= pmom_offset;
    --prnt;
    --s1;
    --s2;
    --tforw;
    --tback;

    /* Function Body */
/*                    ** Save some input variables and replace them */
/*                    ** with values needed to do the self-test */
    if (pass1) {
	testmi_(&c_false, xx, crefin, mimcut, perfct, anyang, nmom, ipolzn, 
		numang, &xmu[1], qext, qsca, gqsc, sforw, sback, &s1[1], &s2[
		1], &tforw[1], &tback[1], &pmom[4 * pmom_offset / 4], momdim);
    }
L10:
/*                                        ** Check input and calculate */
/*                                        ** certain variables from input */
    ckinmi_(numang, &c__501, xx, perfct, crefin, momdim, nmom, ipolzn, anyang,
	     &xmu[1], calcmo, &npquan);
    if (*perfct && *xx <= .1f) {
/*                                            ** Use totally-reflecting */
/*                                            ** small-particle limit */
	small1_(xx, numang, &xmu[1], qext, qsca, gqsc, sforw, sback, &s1[1], &
		s2[1], &tforw[1], &tback[1], lita, litb);
	ntrm = 2;
	goto L100;
    }
    noabs = TRUE_;
    if (! (*perfct)) {
	cior.r = crefin->r, cior.i = crefin->i;
	if (r_imag(&cior) > 0.f) {
	    r_cnjg(&q__1, &cior);
	    cior.r = q__1.r, cior.i = q__1.i;
	}
	mre = cior.r;
	mim = (real)-r_imag(&cior);
	noabs = mim <= *mimcut;
	c_div(&q__1, &c_b6, &cior);
	cioriv.r = q__1.r, cioriv.i = q__1.i;
	rioriv = 1.f / mre;
/* Computing MAX */
	r__1 = 1.f, r__2 = (real)c_abs(&cior);
	if (*xx * dmax(r__1,r__2) <= .1f) {
/*                                    ** Use general-refractive-index */
/*                                    ** small-particle limit */
	    L__1 = mim > *mimcut;
	    small2_(xx, &cior, &L__1, numang, &xmu[1], qext, qsca, gqsc, 
		    sforw, sback, &s1[1], &s2[1], &tforw[1], &tback[1], lita, 
		    litb);
	    ntrm = 2;
	    goto L100;
	}
    }
    nangd2 = (*numang + 1) / 2;
    yesang = *numang > 0;
/*                              ** Number of terms in Mie series; Eq R50 */
    if (*xx <= 8.f) {
	d__1 = (doublereal) (*xx);
	ntrm = (integer)(*xx + pow_dd(&d__1, &c_b7) * 4.f + 1.f);
    } else if (*xx < 4200.f) {
	d__1 = (doublereal) (*xx);
	ntrm = (integer)(*xx + pow_dd(&d__1, &c_b7) * 4.05f + 2.f);
    } else {
	d__1 = (doublereal) (*xx);
	ntrm = (integer)(*xx + pow_dd(&d__1, &c_b7) * 4.f + 2.f);
    }
    if (ntrm + 1 > 10100) {
	sasfit_f2c_errmsg_("MIEV0--PARAMETER MaxTrm TOO SMALL", &c_true, (ftnlen)33);
    }
/*                            ** Calculate logarithmic derivatives of */
/*                            ** J-Bessel-fcn., A-sub-(1 to NTrm) */
    if (! (*perfct)) {
	biga_(&cior, xx, &ntrm, &noabs, &yesang, rbiga, cbiga);
    }
/*                            ** Initialize Ricatti-Bessel functions */
/*                            ** (psi,chi,zeta)-sub-(0,1) for upward */
/*                            ** recurrence ( Eq. R19 ) */
    xinv = 1.f / *xx;
    psinm1 = (real)sin(*xx);
    chinm1 = (real)cos(*xx);
    psin = psinm1 * xinv - chinm1;
    chin = chinm1 * xinv + psinm1;
    q__1.r = psinm1, q__1.i = chinm1;
    zetnm1.r = q__1.r, zetnm1.i = q__1.i;
    q__1.r = psin, q__1.i = chin;
    zetn.r = q__1.r, zetn.i = q__1.i;
/*                                     ** Initialize previous coeffi- */
/*                                     ** cients for GQSC series */
    anm1.r = 0.f, anm1.i = 0.f;
    bnm1.r = 0.f, bnm1.i = 0.f;
/*                             ** Initialize angular function  pi */
/*                             ** and sums for S+, S- ( Ref. 2, p. 1507 ) */
    if (*anyang) {
	i__1 = *numang;
	for (j = 1; j <= i__1; ++j) {
/*                             ** Eq. R39 */
	    pinm1[j - 1] = 0.f;
	    pin[j - 1] = 1.f;
	    i__2 = j - 1;
	    sp[i__2].r = 0.f, sp[i__2].i = 0.f;
	    i__2 = j - 1;
	    sm[i__2].r = 0.f, sm[i__2].i = 0.f;
/* L20: */
	}
    } else {
	i__1 = nangd2;
	for (j = 1; j <= i__1; ++j) {
/*                             ** Eq. R39 */
	    pinm1[j - 1] = 0.f;
	    pin[j - 1] = 1.f;
	    i__2 = j - 1;
	    sp[i__2].r = 0.f, sp[i__2].i = 0.f;
	    i__2 = j - 1;
	    sm[i__2].r = 0.f, sm[i__2].i = 0.f;
	    i__2 = j - 1;
	    sps[i__2].r = 0.f, sps[i__2].i = 0.f;
	    i__2 = j - 1;
	    sms[i__2].r = 0.f, sms[i__2].i = 0.f;
/* L30: */
	}
    }
/*                       ** Initialize Mie sums for efficiencies, etc. */
    *qsca = 0.f;
    *gqsc = 0.f;
    sforw->r = 0.f, sforw->i = 0.f;
    sback->r = 0.f, sback->i = 0.f;
    csum1.r = 0.f, csum1.i = 0.f;
    csum2.r = 0.f, csum2.i = 0.f;
/* ---------  LOOP TO SUM MIE SERIES  ----------------------------------- */
    mm = 1.f;
    *spike = 1.f;
    i__1 = ntrm;
    for (n = 1; n <= i__1; ++n) {
/*                           ** Compute various numerical coefficients */
	fn = (real) n;
	rn = 1.f / fn;
	np1dn = rn + 1.f;
	twonp1 = (real) ((n << 1) + 1);
	coeff = twonp1 / (fn * (n + 1));
	tcoef = twonp1 * (fn * (n + 1));
/*                           ** Calculate Mie series coefficients */
	if (*perfct) {
/*                                 ** Totally-reflecting case; Eq R/A.1,2 */
	    r__1 = fn * xinv * psin - psinm1;
	    q__2.r = r__1, q__2.i = 0.f;
	    r__2 = fn * xinv;
	    q__4.r = r__2 * zetn.r, q__4.i = r__2 * zetn.i;
	    q__3.r = q__4.r - zetnm1.r, q__3.i = q__4.i - zetnm1.i;
	    c_div(&q__1, &q__2, &q__3);
	    an.r = q__1.r, an.i = q__1.i;
	    q__2.r = psin, q__2.i = 0.f;
	    c_div(&q__1, &q__2, &zetn);
	    bn.r = q__1.r, bn.i = q__1.i;
	} else if (noabs) {
/*                                      ** No-absorption case; Eq (R16) */
	    r__1 = rioriv * rbiga[n - 1] + fn * xinv;
	    q__2.r = r__1 * zetn.r, q__2.i = r__1 * zetn.i;
	    q__1.r = q__2.r - zetnm1.r, q__1.i = q__2.i - zetnm1.i;
	    cdenan.r = q__1.r, cdenan.i = q__1.i;
	    r__1 = (rioriv * rbiga[n - 1] + fn * xinv) * psin - psinm1;
	    q__2.r = r__1, q__2.i = 0.f;
	    c_div(&q__1, &q__2, &cdenan);
	    an.r = q__1.r, an.i = q__1.i;
	    r__1 = mre * rbiga[n - 1] + fn * xinv;
	    q__2.r = r__1 * zetn.r, q__2.i = r__1 * zetn.i;
	    q__1.r = q__2.r - zetnm1.r, q__1.i = q__2.i - zetnm1.i;
	    cdenbn.r = q__1.r, cdenbn.i = q__1.i;
	    r__1 = (mre * rbiga[n - 1] + fn * xinv) * psin - psinm1;
	    q__2.r = r__1, q__2.i = 0.f;
	    c_div(&q__1, &q__2, &cdenbn);
	    bn.r = q__1.r, bn.i = q__1.i;
	} else {
/*                                       ** Absorptive case; Eq (R16) */
	    i__2 = n - 1;
	    q__4.r = cioriv.r * cbiga[i__2].r - cioriv.i * cbiga[i__2].i, 
		    q__4.i = cioriv.r * cbiga[i__2].i + cioriv.i * cbiga[i__2]
		    .r;
	    r__1 = fn * xinv;
	    q__3.r = q__4.r + r__1, q__3.i = q__4.i;
	    q__2.r = q__3.r * zetn.r - q__3.i * zetn.i, q__2.i = q__3.r * 
		    zetn.i + q__3.i * zetn.r;
	    q__1.r = q__2.r - zetnm1.r, q__1.i = q__2.i - zetnm1.i;
	    cdenan.r = q__1.r, cdenan.i = q__1.i;
	    i__2 = n - 1;
	    q__4.r = cior.r * cbiga[i__2].r - cior.i * cbiga[i__2].i, q__4.i =
		     cior.r * cbiga[i__2].i + cior.i * cbiga[i__2].r;
	    r__1 = fn * xinv;
	    q__3.r = q__4.r + r__1, q__3.i = q__4.i;
	    q__2.r = q__3.r * zetn.r - q__3.i * zetn.i, q__2.i = q__3.r * 
		    zetn.i + q__3.i * zetn.r;
	    q__1.r = q__2.r - zetnm1.r, q__1.i = q__2.i - zetnm1.i;
	    cdenbn.r = q__1.r, cdenbn.i = q__1.i;
	    i__2 = n - 1;
	    q__5.r = cioriv.r * cbiga[i__2].r - cioriv.i * cbiga[i__2].i, 
		    q__5.i = cioriv.r * cbiga[i__2].i + cioriv.i * cbiga[i__2]
		    .r;
	    r__1 = fn * xinv;
	    q__4.r = q__5.r + r__1, q__4.i = q__5.i;
	    q__3.r = psin * q__4.r, q__3.i = psin * q__4.i;
	    q__2.r = q__3.r - psinm1, q__2.i = q__3.i;
	    c_div(&q__1, &q__2, &cdenan);
	    an.r = q__1.r, an.i = q__1.i;
	    i__2 = n - 1;
	    q__5.r = cior.r * cbiga[i__2].r - cior.i * cbiga[i__2].i, q__5.i =
		     cior.r * cbiga[i__2].i + cior.i * cbiga[i__2].r;
	    r__1 = fn * xinv;
	    q__4.r = q__5.r + r__1, q__4.i = q__5.i;
	    q__3.r = psin * q__4.r, q__3.i = psin * q__4.i;
	    q__2.r = q__3.r - psinm1, q__2.i = q__3.i;
	    c_div(&q__1, &q__2, &cdenbn);
	    bn.r = q__1.r, bn.i = q__1.i;
/*                                         ** Eq (R7) */
/* Computing 2nd power */
	    r__1 = an.r;
/* Computing 2nd power */
	    r__2 = (real)r_imag(&an);
/* Computing 2nd power */
	    r__3 = bn.r;
/* Computing 2nd power */
	    r__4 = (real)r_imag(&bn);
	    *qsca += twonp1 * (r__1 * r__1 + r__2 * r__2 + (r__3 * r__3 + 
		    r__4 * r__4));
	}
/*                       ** Save Mie coefficients for PMOM calculation */
	i__2 = n - 1;
	lita[i__2].r = an.r, lita[i__2].i = an.i;
	i__2 = n - 1;
	litb[i__2].r = bn.r, litb[i__2].i = bn.i;
	if (! (*perfct) && (real) n > *xx) {
/*                                               ** Flag resonance spikes */
	    denan = (real)c_abs(&cdenan);
	    denbn = (real)c_abs(&cdenbn);
/*                                                   ** Eq. R/B.9 */
	    ratio = denan / denbn;
/*                                                   ** Eq. R/B.10 */
	    if (ratio <= .2f || ratio >= 5.f) {
/* Computing MIN */
		r__1 = min(*spike,denan);
		*spike = (real)dmin(r__1,denbn);
	    }
	}
/*                                  ** Increment Mie sums for non-angle- */
/*                                  ** dependent quantities */
/*                                                   ** Eq. R/B.2 */
	q__3.r = an.r + bn.r, q__3.i = an.i + bn.i;
	q__2.r = twonp1 * q__3.r, q__2.i = twonp1 * q__3.i;
	q__1.r = sforw->r + q__2.r, q__1.i = sforw->i + q__2.i;
	sforw->r = q__1.r, sforw->i = q__1.i;
/*                                                   ** Eq. R/B.5,6 */
	q__3.r = an.r - bn.r, q__3.i = an.i - bn.i;
	q__2.r = tcoef * q__3.r, q__2.i = tcoef * q__3.i;
	q__1.r = csum1.r + q__2.r, q__1.i = csum1.i + q__2.i;
	csum1.r = q__1.r, csum1.i = q__1.i;
/*                                                   ** Eq. R/B.1 */
	r__1 = mm * twonp1;
	q__3.r = an.r - bn.r, q__3.i = an.i - bn.i;
	q__2.r = r__1 * q__3.r, q__2.i = r__1 * q__3.i;
	q__1.r = sback->r + q__2.r, q__1.i = sback->i + q__2.i;
	sback->r = q__1.r, sback->i = q__1.i;
/*                                                   ** Eq. R/B.7,8 */
	r__1 = mm * tcoef;
	q__3.r = an.r + bn.r, q__3.i = an.i + bn.i;
	q__2.r = r__1 * q__3.r, q__2.i = r__1 * q__3.i;
	q__1.r = csum2.r + q__2.r, q__1.i = csum2.i + q__2.i;
	csum2.r = q__1.r, csum2.i = q__1.i;
/*                                         ** Eq (R8) */
	r_cnjg(&q__3, &an);
	q__2.r = anm1.r * q__3.r - anm1.i * q__3.i, q__2.i = anm1.r * q__3.i 
		+ anm1.i * q__3.r;
	r_cnjg(&q__5, &bn);
	q__4.r = bnm1.r * q__5.r - bnm1.i * q__5.i, q__4.i = bnm1.r * q__5.i 
		+ bnm1.i * q__5.r;
	q__1.r = q__2.r + q__4.r, q__1.i = q__2.i + q__4.i;
	r_cnjg(&q__7, &bn);
	q__6.r = an.r * q__7.r - an.i * q__7.i, q__6.i = an.r * q__7.i + an.i 
		* q__7.r;
	*gqsc = *gqsc + (fn - rn) * q__1.r + coeff * q__6.r;
	if (yesang) {
/*                                      ** Put Mie coefficients in form */
/*                                      ** needed for computing S+, S- */
/*                                      ** ( Eq R10 ) */
	    q__2.r = an.r + bn.r, q__2.i = an.i + bn.i;
	    q__1.r = coeff * q__2.r, q__1.i = coeff * q__2.i;
	    anp.r = q__1.r, anp.i = q__1.i;
	    q__2.r = an.r - bn.r, q__2.i = an.i - bn.i;
	    q__1.r = coeff * q__2.r, q__1.i = coeff * q__2.i;
	    bnp.r = q__1.r, bnp.i = q__1.i;
/*                                      ** Increment Mie sums for S+, S- */
/*                                      ** while upward recursing */
/*                                      ** angular functions pi and tau */
	    if (*anyang) {
/*                                         ** Arbitrary angles */
/*                                              ** vectorizable loop */
		i__2 = *numang;
		for (j = 1; j <= i__2; ++j) {
/*                                                 ** Eq. (R37b) */
		    rtmp = xmu[j] * pin[j - 1] - pinm1[j - 1];
/*                                                 ** Eq. (R38b) */
		    taun = fn * rtmp - pinm1[j - 1];
/*                                                   ** Eq (R10) */
		    i__3 = j - 1;
		    i__4 = j - 1;
		    r__1 = pin[j - 1] + taun;
		    q__2.r = r__1 * anp.r, q__2.i = r__1 * anp.i;
		    q__1.r = sp[i__4].r + q__2.r, q__1.i = sp[i__4].i + 
			    q__2.i;
		    sp[i__3].r = q__1.r, sp[i__3].i = q__1.i;
		    i__3 = j - 1;
		    i__4 = j - 1;
		    r__1 = pin[j - 1] - taun;
		    q__2.r = r__1 * bnp.r, q__2.i = r__1 * bnp.i;
		    q__1.r = sm[i__4].r + q__2.r, q__1.i = sm[i__4].i + 
			    q__2.i;
		    sm[i__3].r = q__1.r, sm[i__3].i = q__1.i;
		    pinm1[j - 1] = pin[j - 1];
/*                                                 ** Eq. R37c */
		    pin[j - 1] = xmu[j] * pin[j - 1] + np1dn * rtmp;
/* L40: */
		}
	    } else {
/*                                  ** Angles symmetric about 90 degrees */
		q__1.r = mm * anp.r, q__1.i = mm * anp.i;
		anpm.r = q__1.r, anpm.i = q__1.i;
		q__1.r = mm * bnp.r, q__1.i = mm * bnp.i;
		bnpm.r = q__1.r, bnpm.i = q__1.i;
/*                                          ** vectorizable loop */
		i__2 = nangd2;
		for (j = 1; j <= i__2; ++j) {
/*                                                 ** Eq. (R37b) */
		    rtmp = xmu[j] * pin[j - 1] - pinm1[j - 1];
/*                                                 ** Eq. (R38b) */
		    taun = fn * rtmp - pinm1[j - 1];
/*                                                 ** Eq (R10,12) */
		    i__3 = j - 1;
		    i__4 = j - 1;
		    r__1 = pin[j - 1] + taun;
		    q__2.r = r__1 * anp.r, q__2.i = r__1 * anp.i;
		    q__1.r = sp[i__4].r + q__2.r, q__1.i = sp[i__4].i + 
			    q__2.i;
		    sp[i__3].r = q__1.r, sp[i__3].i = q__1.i;
		    i__3 = j - 1;
		    i__4 = j - 1;
		    r__1 = pin[j - 1] + taun;
		    q__2.r = r__1 * bnpm.r, q__2.i = r__1 * bnpm.i;
		    q__1.r = sms[i__4].r + q__2.r, q__1.i = sms[i__4].i + 
			    q__2.i;
		    sms[i__3].r = q__1.r, sms[i__3].i = q__1.i;
		    i__3 = j - 1;
		    i__4 = j - 1;
		    r__1 = pin[j - 1] - taun;
		    q__2.r = r__1 * bnp.r, q__2.i = r__1 * bnp.i;
		    q__1.r = sm[i__4].r + q__2.r, q__1.i = sm[i__4].i + 
			    q__2.i;
		    sm[i__3].r = q__1.r, sm[i__3].i = q__1.i;
		    i__3 = j - 1;
		    i__4 = j - 1;
		    r__1 = pin[j - 1] - taun;
		    q__2.r = r__1 * anpm.r, q__2.i = r__1 * anpm.i;
		    q__1.r = sps[i__4].r + q__2.r, q__1.i = sps[i__4].i + 
			    q__2.i;
		    sps[i__3].r = q__1.r, sps[i__3].i = q__1.i;
		    pinm1[j - 1] = pin[j - 1];
/*                                                 ** Eq. R37c */
		    pin[j - 1] = xmu[j] * pin[j - 1] + np1dn * rtmp;
/* L50: */
		}
	    }
	}
/*                          ** Update relevant quantities for next */
/*                          ** pass through loop */
	mm = -mm;
	anm1.r = an.r, anm1.i = an.i;
	bnm1.r = bn.r, bnm1.i = bn.i;
/*                           ** Upward recurrence for Ricatti-Bessel */
/*                           ** functions ( Eq. R17 ) */
	r__1 = twonp1 * xinv;
	q__2.r = r__1 * zetn.r, q__2.i = r__1 * zetn.i;
	q__1.r = q__2.r - zetnm1.r, q__1.i = q__2.i - zetnm1.i;
	zet.r = q__1.r, zet.i = q__1.i;
	zetnm1.r = zetn.r, zetnm1.i = zetn.i;
	zetn.r = zet.r, zetn.i = zet.i;
	psinm1 = psin;
	psin = zetn.r;
/* L60: */
    }
/* ---------- END LOOP TO SUM MIE SERIES -------------------------------- */
/*                                         ** Eq (R6) */
/* Computing 2nd power */
    r__1 = *xx;
    *qext = 2.f / (r__1 * r__1) * sforw->r;
    if (*perfct || noabs) {
	*qsca = *qext;
    } else {
/* Computing 2nd power */
	r__1 = *xx;
	*qsca = 2.f / (r__1 * r__1) * *qsca;
    }
/* Computing 2nd power */
    r__1 = *xx;
    *gqsc = 4.f / (r__1 * r__1) * *gqsc;
    q__1.r = sforw->r * .5f, q__1.i = sforw->i * .5f;
    sforw->r = q__1.r, sforw->i = q__1.i;
    q__1.r = sback->r * .5f, q__1.i = sback->i * .5f;
    sback->r = q__1.r, sback->i = q__1.i;
    q__2.r = sforw->r * .5f, q__2.i = sforw->i * .5f;
    q__3.r = csum1.r * .125f, q__3.i = csum1.i * .125f;
    q__1.r = q__2.r - q__3.r, q__1.i = q__2.i - q__3.i;
    tforw[1].r = q__1.r, tforw[1].i = q__1.i;
    q__2.r = sforw->r * .5f, q__2.i = sforw->i * .5f;
    q__3.r = csum1.r * .125f, q__3.i = csum1.i * .125f;
    q__1.r = q__2.r + q__3.r, q__1.i = q__2.i + q__3.i;
    tforw[2].r = q__1.r, tforw[2].i = q__1.i;
    q__2.r = sback->r * -.5f, q__2.i = sback->i * -.5f;
    q__3.r = csum2.r * .125f, q__3.i = csum2.i * .125f;
    q__1.r = q__2.r + q__3.r, q__1.i = q__2.i + q__3.i;
    tback[1].r = q__1.r, tback[1].i = q__1.i;
    q__2.r = sback->r * .5f, q__2.i = sback->i * .5f;
    q__3.r = csum2.r * .125f, q__3.i = csum2.i * .125f;
    q__1.r = q__2.r + q__3.r, q__1.i = q__2.i + q__3.i;
    tback[2].r = q__1.r, tback[2].i = q__1.i;
    if (yesang) {
/*                                ** Recover scattering amplitudes */
/*                                ** from S+, S- ( Eq (R11) ) */
	if (*anyang) {
/*                                         ** vectorizable loop */
	    i__1 = *numang;
	    for (j = 1; j <= i__1; ++j) {
/*                                                   ** Eq (R11) */
		i__2 = j;
		i__3 = j - 1;
		i__4 = j - 1;
		q__2.r = sp[i__3].r + sm[i__4].r, q__2.i = sp[i__3].i + sm[
			i__4].i;
		q__1.r = q__2.r * .5f, q__1.i = q__2.i * .5f;
		s1[i__2].r = q__1.r, s1[i__2].i = q__1.i;
		i__2 = j;
		i__3 = j - 1;
		i__4 = j - 1;
		q__2.r = sp[i__3].r - sm[i__4].r, q__2.i = sp[i__3].i - sm[
			i__4].i;
		q__1.r = q__2.r * .5f, q__1.i = q__2.i * .5f;
		s2[i__2].r = q__1.r, s2[i__2].i = q__1.i;
/* L70: */
	    }
	} else {
/*                                         ** vectorizable loop */
	    i__1 = nangd2;
	    for (j = 1; j <= i__1; ++j) {
/*                                                   ** Eq (R11) */
		i__2 = j;
		i__3 = j - 1;
		i__4 = j - 1;
		q__2.r = sp[i__3].r + sm[i__4].r, q__2.i = sp[i__3].i + sm[
			i__4].i;
		q__1.r = q__2.r * .5f, q__1.i = q__2.i * .5f;
		s1[i__2].r = q__1.r, s1[i__2].i = q__1.i;
		i__2 = j;
		i__3 = j - 1;
		i__4 = j - 1;
		q__2.r = sp[i__3].r - sm[i__4].r, q__2.i = sp[i__3].i - sm[
			i__4].i;
		q__1.r = q__2.r * .5f, q__1.i = q__2.i * .5f;
		s2[i__2].r = q__1.r, s2[i__2].i = q__1.i;
/* L80: */
	    }
/*                                         ** vectorizable loop */
	    i__1 = nangd2;
	    for (j = 1; j <= i__1; ++j) {
		i__2 = *numang + 1 - j;
		i__3 = j - 1;
		i__4 = j - 1;
		q__2.r = sps[i__3].r + sms[i__4].r, q__2.i = sps[i__3].i + 
			sms[i__4].i;
		q__1.r = q__2.r * .5f, q__1.i = q__2.i * .5f;
		s1[i__2].r = q__1.r, s1[i__2].i = q__1.i;
		i__2 = *numang + 1 - j;
		i__3 = j - 1;
		i__4 = j - 1;
		q__2.r = sps[i__3].r - sms[i__4].r, q__2.i = sps[i__3].i - 
			sms[i__4].i;
		q__1.r = q__2.r * .5f, q__1.i = q__2.i * .5f;
		s2[i__2].r = q__1.r, s2[i__2].i = q__1.i;
/* L90: */
	    }
	}
    }
/*                                 ** Calculate Legendre moments */
L100:
    if (*nmom > 0) {
	lpcoef_(&ntrm, nmom, ipolzn, momdim, calcmo, &npquan, lita, litb, &
		pmom[pmom_offset]);
    }
    if (r_imag(crefin) > 0.f) {
/*                                         ** Take complex conjugates */
/*                                         ** of scattering amplitudes */
	r_cnjg(&q__1, sforw);
	sforw->r = q__1.r, sforw->i = q__1.i;
	r_cnjg(&q__1, sback);
	sback->r = q__1.r, sback->i = q__1.i;
	for (i__ = 1; i__ <= 2; ++i__) {
	    i__1 = i__;
	    r_cnjg(&q__1, &tforw[i__]);
	    tforw[i__1].r = q__1.r, tforw[i__1].i = q__1.i;
	    i__1 = i__;
	    r_cnjg(&q__1, &tback[i__]);
	    tback[i__1].r = q__1.r, tback[i__1].i = q__1.i;
/* L110: */
	}
	i__1 = *numang;
	for (j = 1; j <= i__1; ++j) {
	    i__2 = j;
	    r_cnjg(&q__1, &s1[j]);
	    s1[i__2].r = q__1.r, s1[i__2].i = q__1.i;
	    i__2 = j;
	    r_cnjg(&q__1, &s2[j]);
	    s2[i__2].r = q__1.r, s2[i__2].i = q__1.i;
/* L120: */
	}
    }
    if (pass1) {
/*                           ** Compare test case results with */
/*                           ** correct answers and abort if bad; */
/*                           ** otherwise restore user input and proceed */
	testmi_(&c_true, xx, crefin, mimcut, perfct, anyang, nmom, ipolzn, 
		numang, &xmu[1], qext, qsca, gqsc, sforw, sback, &s1[1], &s2[
		1], &tforw[1], &tback[1], &pmom[pmom_offset], momdim);
	pass1 = FALSE_;
	goto L10;
    }
    if (prnt[1] || prnt[2]) {
	miprnt_(&prnt[1], xx, perfct, crefin, numang, &xmu[1], qext, qsca, 
		gqsc, nmom, ipolzn, momdim, calcmo, &pmom[pmom_offset], sforw,
		 sback, &tforw[1], &tback[1], &s1[1], &s2[1]);
    }
    return 0;
} /* miev0_ */

int ckinmi_(integer *numang, integer *maxang, real *xx, 
	logical *perfct, complex *crefin, integer *momdim, integer *nmom, 
	integer *ipolzn, logical *anyang, real *xmu, logical *calcmo, integer 
	*npquan)
{
    /* System generated locals */
    integer i__1;
    real r__1;

    /* Builtin functions */
    /*
    integer s_wsfi(icilist *), do_fio(integer *, char *, ftnlen), e_wsfi(void)
	    ;
    double r_imag(complex *);
*/

    /* Local variables */
    static integer i__, j, l, ip;
    static logical inperr;
    static char string[4];

    /* Fortran I/O blocks */
    static icilist io___61 = { 0, string, 0, "(I4)", 4, 1 };


/*        Check for bad input to MIEV0 and calculate CALCMO, NPQUAN */
/*     Routines called :  ERRMSG, WRTBAD, WRTDIM */
/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
    /* Parameter adjustments */
    --calcmo;
    --xmu;

    /* Function Body */
    inperr = FALSE_;
    if (*numang > *maxang) {
	inperr = sasfit_f2c_wrtdim_("MaxAng", numang, (ftnlen)6);
    }
    if (*numang < 0) {
	inperr = sasfit_f2c_wrtbad_("NUMANG", (ftnlen)6);
    }
    if (*xx < 0.f) {
	inperr = sasfit_f2c_wrtbad_("XX", (ftnlen)2);
    }
    if (! (*perfct) && crefin->r <= 0.f) {
	inperr = sasfit_f2c_wrtbad_("CREFIN", (ftnlen)6);
    }
    if (*momdim < 0) {
	inperr = sasfit_f2c_wrtbad_("MOMDIM", (ftnlen)6);
    }
    if (*nmom != 0) {
	if (*nmom < 0 || *nmom > *momdim) {
	    inperr = sasfit_f2c_wrtbad_("NMOM", (ftnlen)4);
	}
	if (abs(*ipolzn) > 4444) {
	    inperr = sasfit_f2c_wrtbad_("IPOLZN", (ftnlen)6);
	}
	*npquan = 0;
	for (l = 1; l <= 4; ++l) {
	    calcmo[l] = FALSE_;
/* L10: */
	}
	if (*ipolzn != 0) {
/*                                 ** Parse out IPOLZN into its digits */
/*                                 ** to find which phase quantities are */
/*                                 ** to have their moments calculated */
	    s_wsfi(&io___61);
	    i__1 = abs(*ipolzn);
	    do_fio(&c__1, (char *)&i__1, (ftnlen)sizeof(integer));
	    e_wsfi();
	    for (j = 1; j <= 4; ++j) {
		ip = *(unsigned char *)&string[j - 1] - '0';
		if (ip >= 1 && ip <= 4) {
		    calcmo[ip] = TRUE_;
		}
		if (ip == 0 || (ip >= 5 && ip <= 9)) {
		    inperr = sasfit_f2c_wrtbad_("IPOLZN", (ftnlen)6);
		}
		*npquan = max(*npquan,ip);
/* L20: */
	    }
	}
    }
    if (*anyang) {
/*                                ** Allow for slight imperfections in */
/*                                ** computation of cosine */
	i__1 = *numang;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (xmu[i__] < -1.00001f || xmu[i__] > 1.00001f) {
		inperr = sasfit_f2c_wrtbad_("XMU", (ftnlen)3);
	    }
/* L30: */
	}
    } else {
	i__1 = (*numang + 1) / 2;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    if (xmu[i__] < -1e-5f || xmu[i__] > 1.00001f) {
		inperr = sasfit_f2c_wrtbad_("XMU", (ftnlen)3);
	    }
/* L40: */
	}
    }
    if (inperr) {
	sasfit_f2c_errmsg_("MIEV0--Input error(S).  Aborting...", &c_true, (ftnlen)35);
    }
    if (*xx > 2e4f || crefin->r > 10.f || (r__1 = (real)r_imag(crefin), dabs(r__1)) 
	    > 10.f) {
	sasfit_f2c_errmsg_("MIEV0--XX or CREFIN outside tested range", &c_false, (ftnlen)
		40);
    }
    return 0;
} /* ckinmi_ */

int lpcoef_(integer *ntrm, integer *nmom, integer *ipolzn, 
	integer *momdim, logical *calcmo, integer *npquan, complex *a, 
	complex *b, real *pmom)
{
    /* Initialized data */

    static logical pass1 = TRUE_;

    /* System generated locals */
    integer pmom_dim1, pmom_offset, i__1, i__2, i__3, i__4, i__5;
    real r__1, r__2, r__3;
    doublereal d__1;
    complex q__1, q__2, q__3, q__4, q__5, q__6;
    static complex equiv_0[1102], equiv_1[1102];

    /* Builtin functions */
    /*
    void r_cnjg(complex *, complex *);
    double r_imag(complex *);
    */

    /* Local variables */
#define c__ (equiv_0)
#define d__ (equiv_1)
    static integer i__, j, k, l, m;
    static real bi[1103], am[1103];
#define cm (equiv_0)
#define dm (equiv_1)
    static complex cs[1102], ds[1102];
    static integer ld2;
    static real sum;
    static integer idel, imax, mmax;
    static real bidel[1103], recip[4410];
    static logical evenl;
    static integer nummom;

/*         Calculate Legendre polynomial expansion coefficients (also */
/*         called moments) for phase quantities ( Ref. 5 formulation ) */
/*     INPUT:  NTRM                    Number terms in Mie series */
/*             NMOM, IPOLZN, MOMDIM    MIEV0 arguments */
/*             CALCMO                  Flags calculated from IPOLZN */
/*             NPQUAN                  Defined in MIEV0 */
/*             A, B                    Mie series coefficients */
/*     OUTPUT: PMOM                   Legendre moments (MIEV0 argument) */
/*     Routines called :  ERRMSG, LPCO1T, LPCO2T */
/*     *** NOTES *** */
/*         (1)  Eqs. 2-5 are in error in Dave, Appl. Opt. 9, */
/*         1888 (1970).  Eq. 2 refers to M1, not M2;  eq. 3 refers to */
/*         M2, not M1.  In eqs. 4 and 5, the subscripts on the second */
/*         term in square brackets should be interchanged. */
/*         (2)  The general-case logic in this subroutine works correctly */
/*         in the two-term Mie series case, but subroutine LPCO2T */
/*         is called instead, for speed. */
/*         (3)  Subroutine  LPCO1T, to do the one-term case, is never */
/*         called within the context of MIEV0, but is included for */
/*         complete generality. */
/*         (4)  Some improvement in speed is obtainable by combining the */
/*         310- and 410-loops, if moments for both the third and fourth */
/*         phase quantities are desired, because the third phase quantity */
/*         is the real part of a complex series, while the fourth phase */
/*         quantity is the imaginary part of that very same series.  But */
/*         most users are not interested in the fourth phase quantity, */
/*         which is related to circular polarization, so the present */
/*         scheme is usually more efficient. */
/*           ** Definitions of local variables *** */
/*      AM(M)       Numerical coefficients  a-sub-m-super-l */
/*                     in Dave, Eqs. 1-15, as simplified in Ref. 5. */
/*      BI(I)       Numerical coefficients  b-sub-i-super-l */
/*                     in Dave, Eqs. 1-15, as simplified in Ref. 5. */
/*      BIDEL(I)    1/2 Bi(I) times factor capital-del in Dave */
/*      CM,DM()     Arrays C and D in Dave, Eqs. 16-17 (Mueller form), */
/*                     calculated using recurrence derived in Ref. 5 */
/*      CS,DS()     Arrays C and D in Ref. 4, Eqs. A5-A6 (Sekera form), */
/*                     calculated using recurrence derived in Ref. 5 */
/*      C,D()       Either CM,DM or CS,DS, depending on IPOLZN */
/*      EVENL       True for even-numbered moments;  false otherwise */
/*      IDEL        1 + little-del  in Dave */
/*      MAXTRM      Max. no. of terms in Mie series */
/*      MAXMOM      Max. no. of non-zero moments */
/*      NUMMOM      Number of non-zero moments */
/*      RECIP(K)    1 / K */
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
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Equivalences .. */
/*     .. */
    /* Parameter adjustments */
    pmom_dim1 = *momdim - 0 + 1;
    pmom_offset = 0 + pmom_dim1;
    pmom -= pmom_offset;
    --calcmo;
    --a;
    --b;

    /* Function Body */
    if (pass1) {
	for (k = 1; k <= 4410; ++k) {
	    recip[k - 1] = 1.f / k;
/* L10: */
	}
	pass1 = FALSE_;
    }
    i__1 = max(1,*npquan);
    for (j = 1; j <= i__1; ++j) {
	i__2 = *nmom;
	for (l = 0; l <= i__2; ++l) {
	    pmom[l + j * pmom_dim1] = 0.f;
/* L20: */
	}
/* L30: */
    }
    if (*ntrm == 1) {
	lpco1t_(nmom, ipolzn, momdim, &calcmo[1], &a[1], &b[1], &pmom[
		pmom_offset]);
	return 0;
    } else if (*ntrm == 2) {
	lpco2t_(nmom, ipolzn, momdim, &calcmo[1], &a[1], &b[1], &pmom[
		pmom_offset]);
	return 0;
    }
    if (*ntrm + 2 > 1102) {
	sasfit_f2c_errmsg_("LPCoef--PARAMETER MaxTrm too small", &c_true, (ftnlen)34);
    }
/*                                     ** Calculate Mueller C, D arrays */
    i__1 = *ntrm + 1;
    cm[i__1].r = 0.f, cm[i__1].i = 0.f;
    i__1 = *ntrm + 1;
    dm[i__1].r = 0.f, dm[i__1].i = 0.f;
    i__1 = *ntrm;
    r__1 = 1.f - recip[*ntrm];
    i__2 = *ntrm;
    q__1.r = r__1 * b[i__2].r, q__1.i = r__1 * b[i__2].i;
    cm[i__1].r = q__1.r, cm[i__1].i = q__1.i;
    i__1 = *ntrm;
    r__1 = 1.f - recip[*ntrm];
    i__2 = *ntrm;
    q__1.r = r__1 * a[i__2].r, q__1.i = r__1 * a[i__2].i;
    dm[i__1].r = q__1.r, dm[i__1].i = q__1.i;
    i__1 = *ntrm - 1;
    r__1 = recip[*ntrm - 1] + recip[*ntrm];
    i__2 = *ntrm;
    q__2.r = r__1 * a[i__2].r, q__2.i = r__1 * a[i__2].i;
    r__2 = 1.f - recip[*ntrm - 1];
    i__3 = *ntrm - 1;
    q__3.r = r__2 * b[i__3].r, q__3.i = r__2 * b[i__3].i;
    q__1.r = q__2.r + q__3.r, q__1.i = q__2.i + q__3.i;
    cm[i__1].r = q__1.r, cm[i__1].i = q__1.i;
    i__1 = *ntrm - 1;
    r__1 = recip[*ntrm - 1] + recip[*ntrm];
    i__2 = *ntrm;
    q__2.r = r__1 * b[i__2].r, q__2.i = r__1 * b[i__2].i;
    r__2 = 1.f - recip[*ntrm - 1];
    i__3 = *ntrm - 1;
    q__3.r = r__2 * a[i__3].r, q__3.i = r__2 * a[i__3].i;
    q__1.r = q__2.r + q__3.r, q__1.i = q__2.i + q__3.i;
    dm[i__1].r = q__1.r, dm[i__1].i = q__1.i;
    for (k = *ntrm - 1; k >= 2; --k) {
	i__1 = k - 1;
	i__2 = k + 1;
	r__1 = recip[k] + 1.f;
	i__3 = k + 1;
	q__4.r = r__1 * b[i__3].r, q__4.i = r__1 * b[i__3].i;
	q__3.r = cm[i__2].r - q__4.r, q__3.i = cm[i__2].i - q__4.i;
	r__2 = recip[k - 1] + recip[k];
	i__4 = k;
	q__5.r = r__2 * a[i__4].r, q__5.i = r__2 * a[i__4].i;
	q__2.r = q__3.r + q__5.r, q__2.i = q__3.i + q__5.i;
	r__3 = 1.f - recip[k - 1];
	i__5 = k - 1;
	q__6.r = r__3 * b[i__5].r, q__6.i = r__3 * b[i__5].i;
	q__1.r = q__2.r + q__6.r, q__1.i = q__2.i + q__6.i;
	cm[i__1].r = q__1.r, cm[i__1].i = q__1.i;
	i__1 = k - 1;
	i__2 = k + 1;
	r__1 = recip[k] + 1.f;
	i__3 = k + 1;
	q__4.r = r__1 * a[i__3].r, q__4.i = r__1 * a[i__3].i;
	q__3.r = dm[i__2].r - q__4.r, q__3.i = dm[i__2].i - q__4.i;
	r__2 = recip[k - 1] + recip[k];
	i__4 = k;
	q__5.r = r__2 * b[i__4].r, q__5.i = r__2 * b[i__4].i;
	q__2.r = q__3.r + q__5.r, q__2.i = q__3.i + q__5.i;
	r__3 = 1.f - recip[k - 1];
	i__5 = k - 1;
	q__6.r = r__3 * a[i__5].r, q__6.i = r__3 * a[i__5].i;
	q__1.r = q__2.r + q__6.r, q__1.i = q__2.i + q__6.i;
	dm[i__1].r = q__1.r, dm[i__1].i = q__1.i;
/* L40: */
    }
    q__3.r = a[1].r - b[2].r, q__3.i = a[1].i - b[2].i;
    q__2.r = q__3.r * 1.5f, q__2.i = q__3.i * 1.5f;
    q__1.r = cm[2].r + q__2.r, q__1.i = cm[2].i + q__2.i;
    cm[0].r = q__1.r, cm[0].i = q__1.i;
    q__3.r = b[1].r - a[2].r, q__3.i = b[1].i - a[2].i;
    q__2.r = q__3.r * 1.5f, q__2.i = q__3.i * 1.5f;
    q__1.r = dm[2].r + q__2.r, q__1.i = dm[2].i + q__2.i;
    dm[0].r = q__1.r, dm[0].i = q__1.i;
    if (*ipolzn >= 0) {
	i__1 = *ntrm + 2;
	for (k = 1; k <= i__1; ++k) {
	    i__2 = k - 1;
	    i__3 = (k << 1) - 1;
	    i__4 = k - 1;
	    d__1 = (doublereal) i__3;
	    q__1.r = (real)d__1 * cm[i__4].r, q__1.i = (real)d__1 * cm[i__4].i;
	    c__[i__2].r = q__1.r, c__[i__2].i = q__1.i;
	    i__2 = k - 1;
	    i__3 = (k << 1) - 1;
	    i__4 = k - 1;
	    d__1 = (doublereal) i__3;
	    q__1.r = (real)d__1 * dm[i__4].r, q__1.i = (real)d__1 * dm[i__4].i;
	    d__[i__2].r = q__1.r, d__[i__2].i = q__1.i;
/* L50: */
	}
    } else {
/*                                    ** Compute Sekera C and D arrays */
	i__1 = *ntrm + 1;
	cs[i__1].r = 0.f, cs[i__1].i = 0.f;
	i__1 = *ntrm + 1;
	ds[i__1].r = 0.f, ds[i__1].i = 0.f;
	i__1 = *ntrm;
	cs[i__1].r = 0.f, cs[i__1].i = 0.f;
	i__1 = *ntrm;
	ds[i__1].r = 0.f, ds[i__1].i = 0.f;
	for (k = *ntrm; k >= 1; --k) {
	    i__1 = k - 1;
	    i__2 = k + 1;
	    i__3 = (k << 1) + 1;
	    i__4 = k;
	    i__5 = k;
	    q__3.r = cm[i__4].r - b[i__5].r, q__3.i = cm[i__4].i - b[i__5].i;
	    d__1 = (doublereal) i__3;
	    q__2.r = (real)d__1 * q__3.r, q__2.i = (real)d__1 * q__3.i;
	    q__1.r = cs[i__2].r + q__2.r, q__1.i = cs[i__2].i + q__2.i;
	    cs[i__1].r = q__1.r, cs[i__1].i = q__1.i;
	    i__1 = k - 1;
	    i__2 = k + 1;
	    i__3 = (k << 1) + 1;
	    i__4 = k;
	    i__5 = k;
	    q__3.r = dm[i__4].r - a[i__5].r, q__3.i = dm[i__4].i - a[i__5].i;
	    d__1 = (doublereal) i__3;
	    q__2.r = (real)d__1 * q__3.r, q__2.i = (real)d__1 * q__3.i;
	    q__1.r = ds[i__2].r + q__2.r, q__1.i = ds[i__2].i + q__2.i;
	    ds[i__1].r = q__1.r, ds[i__1].i = q__1.i;
/* L60: */
	}
	i__1 = *ntrm + 2;
	for (k = 1; k <= i__1; ++k) {
	    i__2 = k - 1;
	    i__3 = (k << 1) - 1;
	    i__4 = k - 1;
	    d__1 = (doublereal) i__3;
	    q__1.r = (real)d__1 * cs[i__4].r, q__1.i = (real)d__1 * cs[i__4].i;
	    c__[i__2].r = q__1.r, c__[i__2].i = q__1.i;
	    i__2 = k - 1;
	    i__3 = (k << 1) - 1;
	    i__4 = k - 1;
	    d__1 = (doublereal) i__3;
	    q__1.r = (real)d__1 * ds[i__4].r, q__1.i = (real)d__1 * ds[i__4].i;
	    d__[i__2].r = q__1.r, d__[i__2].i = q__1.i;
/* L70: */
	}
    }
    if (*ipolzn < 0) {
/* Computing MIN */
	i__1 = *nmom, i__2 = (*ntrm << 1) - 2;
	nummom = min(i__1,i__2);
    }
    if (*ipolzn >= 0) {
/* Computing MIN */
	i__1 = *nmom, i__2 = *ntrm << 1;
	nummom = min(i__1,i__2);
    }
    if (nummom > 2204) {
	sasfit_f2c_errmsg_("LPCoef--PARAMETER MaxTrm too small", &c_true, (ftnlen)34);
    }
/*                          ** Loop over moments */
    i__1 = nummom;
    for (l = 0; l <= i__1; ++l) {
	ld2 = l / 2;
	evenl = l % 2 == 0;
/*                                    ** Calculate numerical coefficients */
/*                                    ** a-sub-m and b-sub-i in Dave */
/*                                    ** double-sums for moments */
	if (l == 0) {
	    idel = 1;
	    i__2 = *ntrm;
	    for (m = 0; m <= i__2; ++m) {
		am[m] = recip[m * 2] * 2.f;
/* L80: */
	    }
	    bi[0] = 1.f;
	} else if (evenl) {
	    idel = 1;
	    i__2 = *ntrm;
	    for (m = ld2; m <= i__2; ++m) {
		am[m] = (recip[(m << 1) - l] + 1.f) * am[m];
/* L90: */
	    }
	    i__2 = ld2 - 1;
	    for (i__ = 0; i__ <= i__2; ++i__) {
		bi[i__] = (1.f - recip[l - (i__ << 1) - 1]) * bi[i__];
/* L100: */
	    }
	    bi[ld2] = (2.f - recip[l - 1]) * bi[ld2 - 1];
	} else {
	    idel = 2;
	    i__2 = *ntrm;
	    for (m = ld2; m <= i__2; ++m) {
		am[m] = (1.f - recip[(m << 1) + l + 1]) * am[m];
/* L110: */
	    }
	    i__2 = ld2;
	    for (i__ = 0; i__ <= i__2; ++i__) {
		bi[i__] = (1.f - recip[l + (i__ << 1)]) * bi[i__];
/* L120: */
	    }
	}
/*                                     ** Establish upper limits for sums */
/*                                     ** and incorporate factor capital- */
/*                                     ** del into b-sub-i */
	mmax = *ntrm - idel;
	if (*ipolzn >= 0) {
	    ++mmax;
	}
/* Computing MIN */
	i__2 = ld2, i__3 = mmax - ld2;
	imax = min(i__2,i__3);
	if (imax < 0) {
	    goto L250;
	}
	i__2 = imax;
	for (i__ = 0; i__ <= i__2; ++i__) {
	    bidel[i__] = bi[i__];
/* L130: */
	}
	if (evenl) {
	    bidel[0] *= .5f;
	}
/*                                    ** Perform double sums just for */
/*                                    ** phase quantities desired by user */
	if (*ipolzn == 0) {
	    i__2 = imax;
	    for (i__ = 0; i__ <= i__2; ++i__) {
/*                                           ** vectorizable loop */
		sum = 0.f;
		i__3 = mmax - i__;
		for (m = ld2; m <= i__3; ++m) {
		    i__4 = m - i__;
		    r_cnjg(&q__2, &c__[m + i__ + idel - 1]);
		    q__1.r = c__[i__4].r * q__2.r - c__[i__4].i * q__2.i, 
			    q__1.i = c__[i__4].r * q__2.i + c__[i__4].i * 
			    q__2.r;
		    i__5 = m - i__;
		    r_cnjg(&q__4, &d__[m + i__ + idel - 1]);
		    q__3.r = d__[i__5].r * q__4.r - d__[i__5].i * q__4.i, 
			    q__3.i = d__[i__5].r * q__4.i + d__[i__5].i * 
			    q__4.r;
		    sum += am[m] * (q__1.r + q__3.r);
/* L140: */
		}
		pmom[l + pmom_dim1] += bidel[i__] * sum;
/* L150: */
	    }
	    pmom[l + pmom_dim1] *= .5f;
	    goto L240;
	}
	if (calcmo[1]) {
	    i__2 = imax;
	    for (i__ = 0; i__ <= i__2; ++i__) {
		sum = 0.f;
/*                                           ** vectorizable loop */
		i__3 = mmax - i__;
		for (m = ld2; m <= i__3; ++m) {
		    i__4 = m - i__;
		    r_cnjg(&q__2, &c__[m + i__ + idel - 1]);
		    q__1.r = c__[i__4].r * q__2.r - c__[i__4].i * q__2.i, 
			    q__1.i = c__[i__4].r * q__2.i + c__[i__4].i * 
			    q__2.r;
		    sum += am[m] * q__1.r;
/* L160: */
		}
		pmom[l + pmom_dim1] += bidel[i__] * sum;
/* L170: */
	    }
	}
	if (calcmo[2]) {
	    i__2 = imax;
	    for (i__ = 0; i__ <= i__2; ++i__) {
		sum = 0.f;
/*                                           ** vectorizable loop */
		i__3 = mmax - i__;
		for (m = ld2; m <= i__3; ++m) {
		    i__4 = m - i__;
		    r_cnjg(&q__2, &d__[m + i__ + idel - 1]);
		    q__1.r = d__[i__4].r * q__2.r - d__[i__4].i * q__2.i, 
			    q__1.i = d__[i__4].r * q__2.i + d__[i__4].i * 
			    q__2.r;
		    sum += am[m] * q__1.r;
/* L180: */
		}
		pmom[l + (pmom_dim1 << 1)] += bidel[i__] * sum;
/* L190: */
	    }
	}
	if (calcmo[3]) {
	    i__2 = imax;
	    for (i__ = 0; i__ <= i__2; ++i__) {
		sum = 0.f;
/*                                           ** vectorizable loop */
		i__3 = mmax - i__;
		for (m = ld2; m <= i__3; ++m) {
		    i__4 = m - i__;
		    r_cnjg(&q__2, &d__[m + i__ + idel - 1]);
		    q__1.r = c__[i__4].r * q__2.r - c__[i__4].i * q__2.i, 
			    q__1.i = c__[i__4].r * q__2.i + c__[i__4].i * 
			    q__2.r;
		    i__5 = m + i__ + idel - 1;
		    r_cnjg(&q__4, &d__[m - i__]);
		    q__3.r = c__[i__5].r * q__4.r - c__[i__5].i * q__4.i, 
			    q__3.i = c__[i__5].r * q__4.i + c__[i__5].i * 
			    q__4.r;
		    sum += am[m] * (q__1.r + q__3.r);
/* L200: */
		}
		pmom[l + pmom_dim1 * 3] += bidel[i__] * sum;
/* L210: */
	    }
	    pmom[l + pmom_dim1 * 3] *= .5f;
	}
	if (calcmo[4]) {
	    i__2 = imax;
	    for (i__ = 0; i__ <= i__2; ++i__) {
		sum = 0.f;
/*                                           ** vectorizable loop */
		i__3 = mmax - i__;
		for (m = ld2; m <= i__3; ++m) {
		    i__4 = m - i__;
		    r_cnjg(&q__2, &d__[m + i__ + idel - 1]);
		    q__1.r = c__[i__4].r * q__2.r - c__[i__4].i * q__2.i, 
			    q__1.i = c__[i__4].r * q__2.i + c__[i__4].i * 
			    q__2.r;
		    i__5 = m + i__ + idel - 1;
		    r_cnjg(&q__4, &d__[m - i__]);
		    q__3.r = c__[i__5].r * q__4.r - c__[i__5].i * q__4.i, 
			    q__3.i = c__[i__5].r * q__4.i + c__[i__5].i * 
			    q__4.r;
		    sum += am[m] * (real)(r_imag(&q__1) + r_imag(&q__3));
/* L220: */
		}
		pmom[l + (pmom_dim1 << 2)] += bidel[i__] * sum;
/* L230: */
	    }
	    pmom[l + (pmom_dim1 << 2)] *= -.5f;
	}
L240:
	;
    }
L250:
    return 0;
} /* lpcoef_ */

#undef dm
#undef cm
#undef d__
#undef c__


int lpco1t_(integer *nmom, integer *ipolzn, integer *momdim, 
	logical *calcmo, complex *a, complex *b, real *pmom)
{
    /* System generated locals */
    integer pmom_dim1, pmom_offset, i__1;
    real r__1, r__2;
    complex q__1, q__2;

    /* Builtin functions */
    /*
    double r_imag(complex *);
    void r_cnjg(complex *, complex *);
    */

    /* Local variables */
    static integer l;
    static real a1sq, b1sq;
    static complex a1b1c;
    static integer nummom;

/*         Calculate Legendre polynomial expansion coefficients (also */
/*         called moments) for phase quantities in special case where */
/*         no. terms in Mie series = 1 */
/*        INPUT:  NMOM, IPOLZN, MOMDIM     MIEV0 arguments */
/*                CALCMO                   Flags calculated from IPOLZN */
/*                A(1), B(1)               Mie series coefficients */
/*        OUTPUT: PMOM                     Legendre moments */
/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Statement Functions .. */
/*     .. */
/*     .. Statement Function definitions .. */
/*     .. */
    /* Parameter adjustments */
    pmom_dim1 = *momdim - 0 + 1;
    pmom_offset = 0 + pmom_dim1;
    pmom -= pmom_offset;
    --calcmo;
    --a;
    --b;

    /* Function Body */
/* Computing 2nd power */
    r__1 = a[1].r;
/* Computing 2nd power */
    r__2 = (real)r_imag(&a[1]);
    a1sq = r__1 * r__1 + r__2 * r__2;
/* Computing 2nd power */
    r__1 = b[1].r;
/* Computing 2nd power */
    r__2 = (real)r_imag(&b[1]);
    b1sq = r__1 * r__1 + r__2 * r__2;
    r_cnjg(&q__2, &b[1]);
    q__1.r = a[1].r * q__2.r - a[1].i * q__2.i, q__1.i = a[1].r * q__2.i + a[
	    1].i * q__2.r;
    a1b1c.r = q__1.r, a1b1c.i = q__1.i;
    if (*ipolzn < 0) {
	if (calcmo[1]) {
	    pmom[pmom_dim1] = b1sq * 2.25f;
	}
	if (calcmo[2]) {
	    pmom[pmom_dim1 * 2] = a1sq * 2.25f;
	}
	if (calcmo[3]) {
	    pmom[pmom_dim1 * 3] = a1b1c.r * 2.25f;
	}
	if (calcmo[4]) {
	    pmom[pmom_dim1 * 4] = (real)r_imag(&a1b1c) * 2.25f;
	}
    } else {
	nummom = min(*nmom,2);
/*                             ** Loop over moments */
	i__1 = nummom;
	for (l = 0; l <= i__1; ++l) {
	    if (*ipolzn == 0) {
		if (l == 0) {
		    pmom[l + pmom_dim1] = (a1sq + b1sq) * 1.5f;
		}
		if (l == 1) {
		    pmom[l + pmom_dim1] = a1b1c.r * 1.5f;
		}
		if (l == 2) {
		    pmom[l + pmom_dim1] = (a1sq + b1sq) * .15f;
		}
		goto L10;
	    }
	    if (calcmo[1]) {
		if (l == 0) {
		    pmom[l + pmom_dim1] = (a1sq + b1sq / 3.f) * 2.25f;
		}
		if (l == 1) {
		    pmom[l + pmom_dim1] = a1b1c.r * 1.5f;
		}
		if (l == 2) {
		    pmom[l + pmom_dim1] = b1sq * .3f;
		}
	    }
	    if (calcmo[2]) {
		if (l == 0) {
		    pmom[l + (pmom_dim1 << 1)] = (b1sq + a1sq / 3.f) * 2.25f;
		}
		if (l == 1) {
		    pmom[l + (pmom_dim1 << 1)] = a1b1c.r * 1.5f;
		}
		if (l == 2) {
		    pmom[l + (pmom_dim1 << 1)] = a1sq * .3f;
		}
	    }
	    if (calcmo[3]) {
		if (l == 0) {
		    pmom[l + pmom_dim1 * 3] = a1b1c.r * 3.f;
		}
		if (l == 1) {
		    pmom[l + pmom_dim1 * 3] = (a1sq + b1sq) * .75f;
		}
		if (l == 2) {
		    pmom[l + pmom_dim1 * 3] = a1b1c.r * .3f;
		}
	    }
	    if (calcmo[4]) {
		if (l == 0) {
		    pmom[l + (pmom_dim1 << 2)] = (real)r_imag(&a1b1c) * -1.5f;
		}
		if (l == 1) {
		    pmom[l + (pmom_dim1 << 2)] = 0.f;
		}
		if (l == 2) {
		    pmom[l + (pmom_dim1 << 2)] = (real)r_imag(&a1b1c) * .3f;
		}
	    }
L10:
	    ;
	}
    }
    return 0;
} /* lpco1t_ */

int lpco2t_(integer *nmom, integer *ipolzn, integer *momdim, 
	logical *calcmo, complex *a, complex *b, real *pmom)
{
    /* System generated locals */
    integer pmom_dim1, pmom_offset, i__1;
    real r__1, r__2, r__3, r__4;
    complex q__1, q__2, q__3, q__4, q__5, q__6, q__7;

    /* Builtin functions */
    /*
    void r_cnjg(complex *, complex *);
    double r_imag(complex *);
    */

    /* Local variables */
    static integer l;
    static complex ca, cb, cg, ch, a2c, b2c;
    static real pm1, pm2;
    static complex cac, cbc, cat, cbt;
    static real a2sq, b2sq;
    static integer nummom;

/*         Calculate Legendre polynomial expansion coefficients (also */
/*         called moments) for phase quantities in special case where */
/*         no. terms in Mie series = 2 */
/*        INPUT:  NMOM, IPOLZN, MOMDIM     MIEV0 arguments */
/*                CALCMO                   Flags calculated from IPOLZN */
/*                A(1-2), B(1-2)           Mie series coefficients */
/*        OUTPUT: PMOM                     Legendre moments */
/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Statement Functions .. */
/*     .. */
/*     .. Statement Function definitions .. */
/*     .. */
    /* Parameter adjustments */
    pmom_dim1 = *momdim - 0 + 1;
    pmom_offset = 0 + pmom_dim1;
    pmom -= pmom_offset;
    --calcmo;
    --a;
    --b;

    /* Function Body */
    q__2.r = a[1].r * 3.f, q__2.i = a[1].i * 3.f;
    q__3.r = b[2].r * 5.f, q__3.i = b[2].i * 5.f;
    q__1.r = q__2.r - q__3.r, q__1.i = q__2.i - q__3.i;
    ca.r = q__1.r, ca.i = q__1.i;
    q__2.r = b[1].r * 3.f, q__2.i = b[1].i * 3.f;
    q__3.r = a[2].r * 5.f, q__3.i = a[2].i * 5.f;
    q__1.r = q__2.r - q__3.r, q__1.i = q__2.i - q__3.i;
    cat.r = q__1.r, cat.i = q__1.i;
    r_cnjg(&q__1, &ca);
    cac.r = q__1.r, cac.i = q__1.i;
/* Computing 2nd power */
    r__1 = a[2].r;
/* Computing 2nd power */
    r__2 = (real)r_imag(&a[2]);
    a2sq = r__1 * r__1 + r__2 * r__2;
/* Computing 2nd power */
    r__1 = b[2].r;
/* Computing 2nd power */
    r__2 = (real)r_imag(&b[2]);
    b2sq = r__1 * r__1 + r__2 * r__2;
    r_cnjg(&q__1, &a[2]);
    a2c.r = q__1.r, a2c.i = q__1.i;
    r_cnjg(&q__1, &b[2]);
    b2c.r = q__1.r, b2c.i = q__1.i;
    if (*ipolzn < 0) {
/*                                   ** Loop over Sekera moments */
	nummom = min(*nmom,2);
	i__1 = nummom;
	for (l = 0; l <= i__1; ++l) {
	    if (calcmo[1]) {
		if (l == 0) {
/* Computing 2nd power */
		    r__1 = cat.r;
/* Computing 2nd power */
		    r__2 = (real)r_imag(&cat);
		    pmom[l + pmom_dim1] = (r__1 * r__1 + r__2 * r__2 + b2sq * 
			    33.333333333333336f) * .25f;
		}
		if (l == 1) {
		    q__1.r = cat.r * b2c.r - cat.i * b2c.i, q__1.i = cat.r * 
			    b2c.i + cat.i * b2c.r;
		    pmom[l + pmom_dim1] = q__1.r * 1.6666666666666667f;
		}
		if (l == 2) {
		    pmom[l + pmom_dim1] = b2sq * 3.3333333333333335f;
		}
	    }
	    if (calcmo[2]) {
		if (l == 0) {
/* Computing 2nd power */
		    r__1 = ca.r;
/* Computing 2nd power */
		    r__2 = (real)r_imag(&ca);
		    pmom[l + (pmom_dim1 << 1)] = (r__1 * r__1 + r__2 * r__2 + 
			    a2sq * 33.333333333333336f) * .25f;
		}
		if (l == 1) {
		    q__1.r = ca.r * a2c.r - ca.i * a2c.i, q__1.i = ca.r * 
			    a2c.i + ca.i * a2c.r;
		    pmom[l + (pmom_dim1 << 1)] = q__1.r * 1.6666666666666667f;
		}
		if (l == 2) {
		    pmom[l + (pmom_dim1 << 1)] = a2sq * 3.3333333333333335f;
		}
	    }
	    if (calcmo[3]) {
		if (l == 0) {
		    q__2.r = cat.r * cac.r - cat.i * cac.i, q__2.i = cat.r * 
			    cac.i + cat.i * cac.r;
		    q__4.r = b[2].r * 33.333333333333336f, q__4.i = b[2].i * 
			    33.333333333333336f;
		    q__3.r = q__4.r * a2c.r - q__4.i * a2c.i, q__3.i = q__4.r 
			    * a2c.i + q__4.i * a2c.r;
		    q__1.r = q__2.r + q__3.r, q__1.i = q__2.i + q__3.i;
		    pmom[l + pmom_dim1 * 3] = q__1.r * .25f;
		}
		if (l == 1) {
		    q__2.r = b[2].r * cac.r - b[2].i * cac.i, q__2.i = b[2].r 
			    * cac.i + b[2].i * cac.r;
		    q__3.r = cat.r * a2c.r - cat.i * a2c.i, q__3.i = cat.r * 
			    a2c.i + cat.i * a2c.r;
		    q__1.r = q__2.r + q__3.r, q__1.i = q__2.i + q__3.i;
		    pmom[l + pmom_dim1 * 3] = q__1.r * .83333333333333337f;
		}
		if (l == 2) {
		    q__1.r = b[2].r * a2c.r - b[2].i * a2c.i, q__1.i = b[2].r 
			    * a2c.i + b[2].i * a2c.r;
		    pmom[l + pmom_dim1 * 3] = q__1.r * 3.3333333333333335f;
		}
	    }
	    if (calcmo[4]) {
		if (l == 0) {
		    q__2.r = cat.r * cac.r - cat.i * cac.i, q__2.i = cat.r * 
			    cac.i + cat.i * cac.r;
		    q__4.r = b[2].r * 33.333333333333336f, q__4.i = b[2].i * 
			    33.333333333333336f;
		    q__3.r = q__4.r * a2c.r - q__4.i * a2c.i, q__3.i = q__4.r 
			    * a2c.i + q__4.i * a2c.r;
		    q__1.r = q__2.r + q__3.r, q__1.i = q__2.i + q__3.i;
		    pmom[l + (pmom_dim1 << 2)] = (real)r_imag(&q__1) * -.25f;
		}
		if (l == 1) {
		    q__2.r = b[2].r * cac.r - b[2].i * cac.i, q__2.i = b[2].r 
			    * cac.i + b[2].i * cac.r;
		    q__3.r = cat.r * a2c.r - cat.i * a2c.i, q__3.i = cat.r * 
			    a2c.i + cat.i * a2c.r;
		    q__1.r = q__2.r + q__3.r, q__1.i = q__2.i + q__3.i;
		    pmom[l + (pmom_dim1 << 2)] = (real)r_imag(&q__1) * 
			    -.83333333333333337f;
		}
		if (l == 2) {
		    q__1.r = b[2].r * a2c.r - b[2].i * a2c.i, q__1.i = b[2].r 
			    * a2c.i + b[2].i * a2c.r;
		    pmom[l + (pmom_dim1 << 2)] = (real)r_imag(&q__1) * 
			    -3.3333333333333335f;
		}
	    }
/* L10: */
	}
    } else {
	q__2.r = b[1].r * 3.f, q__2.i = b[1].i * 3.f;
	q__3.r = a[2].r * 5.f, q__3.i = a[2].i * 5.f;
	q__1.r = q__2.r + q__3.r, q__1.i = q__2.i + q__3.i;
	cb.r = q__1.r, cb.i = q__1.i;
	q__2.r = a[1].r * 3.f, q__2.i = a[1].i * 3.f;
	q__3.r = b[2].r * 5.f, q__3.i = b[2].i * 5.f;
	q__1.r = q__2.r + q__3.r, q__1.i = q__2.i + q__3.i;
	cbt.r = q__1.r, cbt.i = q__1.i;
	r_cnjg(&q__1, &cb);
	cbc.r = q__1.r, cbc.i = q__1.i;
	q__3.r = cbc.r * cbt.r - cbc.i * cbt.i, q__3.i = cbc.r * cbt.i + 
		cbc.i * cbt.r;
	q__6.r = cac.r * a[2].r - cac.i * a[2].i, q__6.i = cac.r * a[2].i + 
		cac.i * a[2].r;
	q__7.r = b2c.r * cat.r - b2c.i * cat.i, q__7.i = b2c.r * cat.i + 
		b2c.i * cat.r;
	q__5.r = q__6.r + q__7.r, q__5.i = q__6.i + q__7.i;
	q__4.r = q__5.r * 10.f, q__4.i = q__5.i * 10.f;
	q__2.r = q__3.r + q__4.r, q__2.i = q__3.i + q__4.i;
	q__1.r = q__2.r / 3.f, q__1.i = q__2.i / 3.f;
	cg.r = q__1.r, cg.i = q__1.i;
	q__3.r = cbc.r * a[2].r - cbc.i * a[2].i, q__3.i = cbc.r * a[2].i + 
		cbc.i * a[2].r;
	q__4.r = b2c.r * cbt.r - b2c.i * cbt.i, q__4.i = b2c.r * cbt.i + 
		b2c.i * cbt.r;
	q__2.r = q__3.r + q__4.r, q__2.i = q__3.i + q__4.i;
	q__1.r = q__2.r * 2.f, q__1.i = q__2.i * 2.f;
	ch.r = q__1.r, ch.i = q__1.i;
/*                               ** Loop over Mueller moments */
	nummom = min(*nmom,4);
	i__1 = nummom;
	for (l = 0; l <= i__1; ++l) {
	    if (*ipolzn == 0 || calcmo[1]) {
		if (l == 0) {
/* Computing 2nd power */
		    r__1 = ca.r;
/* Computing 2nd power */
		    r__2 = (real)r_imag(&ca);
/* Computing 2nd power */
		    r__3 = cb.r;
/* Computing 2nd power */
		    r__4 = (real)r_imag(&cb);
		    q__1.r = ca.r * b2c.r - ca.i * b2c.i, q__1.i = ca.r * 
			    b2c.i + ca.i * b2c.r;
		    pm1 = (r__1 * r__1 + r__2 * r__2) * .25f + (r__3 * r__3 + 
			    r__4 * r__4) / 12.f + q__1.r * 
			    1.6666666666666667f + b2sq * 5.f;
		}
		if (l == 1) {
		    q__3.r = cac.r / 6.f, q__3.i = cac.i / 6.f;
		    q__2.r = q__3.r + b2c.r, q__2.i = q__3.i + b2c.i;
		    q__1.r = cb.r * q__2.r - cb.i * q__2.i, q__1.i = cb.r * 
			    q__2.i + cb.i * q__2.r;
		    pm1 = q__1.r;
		}
		if (l == 2) {
/* Computing 2nd power */
		    r__1 = cb.r;
/* Computing 2nd power */
		    r__2 = (real)r_imag(&cb);
		    q__1.r = ca.r * b2c.r - ca.i * b2c.i, q__1.i = ca.r * 
			    b2c.i + ca.i * b2c.r;
		    pm1 = (r__1 * r__1 + r__2 * r__2) / 30.f + b2sq * 
			    2.8571428571428572f + q__1.r * 
			    .66666666666666663f;
		}
		if (l == 3) {
		    q__1.r = cb.r * b2c.r - cb.i * b2c.i, q__1.i = cb.r * 
			    b2c.i + cb.i * b2c.r;
		    pm1 = q__1.r * .2857142857142857f;
		}
		if (l == 4) {
		    pm1 = b2sq * .63492063492063489f;
		}
		if (calcmo[1]) {
		    pmom[l + pmom_dim1] = pm1;
		}
	    }
	    if (*ipolzn == 0 || calcmo[2]) {
		if (l == 0) {
/* Computing 2nd power */
		    r__1 = cat.r;
/* Computing 2nd power */
		    r__2 = (real)r_imag(&cat);
/* Computing 2nd power */
		    r__3 = cbt.r;
/* Computing 2nd power */
		    r__4 = (real)r_imag(&cbt);
		    q__1.r = cat.r * a2c.r - cat.i * a2c.i, q__1.i = cat.r * 
			    a2c.i + cat.i * a2c.r;
		    pm2 = (r__1 * r__1 + r__2 * r__2) * .25f + (r__3 * r__3 + 
			    r__4 * r__4) / 12.f + q__1.r * 
			    1.6666666666666667f + a2sq * 5.f;
		}
		if (l == 1) {
		    r_cnjg(&q__4, &cat);
		    q__3.r = q__4.r / 6.f, q__3.i = q__4.i / 6.f;
		    q__2.r = q__3.r + a2c.r, q__2.i = q__3.i + a2c.i;
		    q__1.r = cbt.r * q__2.r - cbt.i * q__2.i, q__1.i = cbt.r *
			     q__2.i + cbt.i * q__2.r;
		    pm2 = q__1.r;
		}
		if (l == 2) {
/* Computing 2nd power */
		    r__1 = cbt.r;
/* Computing 2nd power */
		    r__2 = (real)r_imag(&cbt);
		    q__1.r = cat.r * a2c.r - cat.i * a2c.i, q__1.i = cat.r * 
			    a2c.i + cat.i * a2c.r;
		    pm2 = (r__1 * r__1 + r__2 * r__2) / 30.f + a2sq * 
			    2.8571428571428572f + q__1.r * 
			    .66666666666666663f;
		}
		if (l == 3) {
		    q__1.r = cbt.r * a2c.r - cbt.i * a2c.i, q__1.i = cbt.r * 
			    a2c.i + cbt.i * a2c.r;
		    pm2 = q__1.r * .2857142857142857f;
		}
		if (l == 4) {
		    pm2 = a2sq * .63492063492063489f;
		}
		if (calcmo[2]) {
		    pmom[l + (pmom_dim1 << 1)] = pm2;
		}
	    }
	    if (*ipolzn == 0) {
		pmom[l + pmom_dim1] = (pm1 + pm2) * .5f;
		goto L20;
	    }
	    if (calcmo[3]) {
		if (l == 0) {
		    q__3.r = cac.r * cat.r - cac.i * cat.i, q__3.i = cac.r * 
			    cat.i + cac.i * cat.r;
		    q__2.r = q__3.r + cg.r, q__2.i = q__3.i + cg.i;
		    q__5.r = b2c.r * 20.f, q__5.i = b2c.i * 20.f;
		    q__4.r = q__5.r * a[2].r - q__5.i * a[2].i, q__4.i = 
			    q__5.r * a[2].i + q__5.i * a[2].r;
		    q__1.r = q__2.r + q__4.r, q__1.i = q__2.i + q__4.i;
		    pmom[l + pmom_dim1 * 3] = q__1.r * .25f;
		}
		if (l == 1) {
		    q__3.r = cac.r * cbt.r - cac.i * cbt.i, q__3.i = cac.r * 
			    cbt.i + cac.i * cbt.r;
		    q__4.r = cbc.r * cat.r - cbc.i * cat.i, q__4.i = cbc.r * 
			    cat.i + cbc.i * cat.r;
		    q__2.r = q__3.r + q__4.r, q__2.i = q__3.i + q__4.i;
		    q__5.r = ch.r * 3.f, q__5.i = ch.i * 3.f;
		    q__1.r = q__2.r + q__5.r, q__1.i = q__2.i + q__5.i;
		    pmom[l + pmom_dim1 * 3] = q__1.r / 12.f;
		}
		if (l == 2) {
		    q__3.r = b2c.r * 28.571428571428573f, q__3.i = b2c.i * 
			    28.571428571428573f;
		    q__2.r = q__3.r * a[2].r - q__3.i * a[2].i, q__2.i = 
			    q__3.r * a[2].i + q__3.i * a[2].r;
		    q__1.r = cg.r + q__2.r, q__1.i = cg.i + q__2.i;
		    pmom[l + pmom_dim1 * 3] = q__1.r * .1f;
		}
		if (l == 3) {
		    pmom[l + pmom_dim1 * 3] = ch.r / 14.f;
		}
		if (l == 4) {
		    q__1.r = b2c.r * a[2].r - b2c.i * a[2].i, q__1.i = b2c.r *
			     a[2].i + b2c.i * a[2].r;
		    pmom[l + pmom_dim1 * 3] = q__1.r * .63492063492063489f;
		}
	    }
	    if (calcmo[4]) {
		if (l == 0) {
		    q__3.r = cac.r * cat.r - cac.i * cat.i, q__3.i = cac.r * 
			    cat.i + cac.i * cat.r;
		    q__2.r = q__3.r + cg.r, q__2.i = q__3.i + cg.i;
		    q__5.r = b2c.r * 20.f, q__5.i = b2c.i * 20.f;
		    q__4.r = q__5.r * a[2].r - q__5.i * a[2].i, q__4.i = 
			    q__5.r * a[2].i + q__5.i * a[2].r;
		    q__1.r = q__2.r + q__4.r, q__1.i = q__2.i + q__4.i;
		    pmom[l + (pmom_dim1 << 2)] = (real)r_imag(&q__1) * .25f;
		}
		if (l == 1) {
		    q__3.r = cac.r * cbt.r - cac.i * cbt.i, q__3.i = cac.r * 
			    cbt.i + cac.i * cbt.r;
		    q__4.r = cbc.r * cat.r - cbc.i * cat.i, q__4.i = cbc.r * 
			    cat.i + cbc.i * cat.r;
		    q__2.r = q__3.r + q__4.r, q__2.i = q__3.i + q__4.i;
		    q__5.r = ch.r * 3.f, q__5.i = ch.i * 3.f;
		    q__1.r = q__2.r + q__5.r, q__1.i = q__2.i + q__5.i;
		    pmom[l + (pmom_dim1 << 2)] = (real)r_imag(&q__1) / 12.f;
		}
		if (l == 2) {
		    q__3.r = b2c.r * 28.571428571428573f, q__3.i = b2c.i * 
			    28.571428571428573f;
		    q__2.r = q__3.r * a[2].r - q__3.i * a[2].i, q__2.i = 
			    q__3.r * a[2].i + q__3.i * a[2].r;
		    q__1.r = cg.r + q__2.r, q__1.i = cg.i + q__2.i;
		    pmom[l + (pmom_dim1 << 2)] = (real)r_imag(&q__1) * .1f;
		}
		if (l == 3) {
		    pmom[l + (pmom_dim1 << 2)] = (real)r_imag(&ch) / 14.f;
		}
		if (l == 4) {
		    q__1.r = b2c.r * a[2].r - b2c.i * a[2].i, q__1.i = b2c.r *
			     a[2].i + b2c.i * a[2].r;
		    pmom[l + (pmom_dim1 << 2)] = (real)r_imag(&q__1) * 
			    .63492063492063489f;
		}
	    }
L20:
	    ;
	}
    }
    return 0;
} /* lpco2t_ */

int biga_(complex *cior, real *xx, integer *ntrm, logical *
	noabs, logical *yesang, real *rbiga, complex *cbiga)
{
    /* System generated locals */
    integer i__1, i__2, i__3;
    real r__1, r__2;
    doublereal d__1, d__2;
    complex q__1, q__2, q__3, q__4, q__5, q__6, q__7, q__8, q__9;

    /* Builtin functions */
    /*
    double r_imag(complex *);
    void c_div(complex *, complex *, complex *);
    double sin(doublereal), cos(doublereal);
    void c_exp(complex *, complex *);
    */

    /* Local variables */
    static integer n;
    static real mim, mre;
    static complex ctmp;
    static logical down;
    static real rtmp;
    static complex zinv;
    static real rezinv;

/*        Calculate logarithmic derivatives of J-Bessel-function */
/*     Input :  CIOR, XX, NTRM, NOABS, YESANG  (defined in MIEV0) */
/*    Output :  RBIGA or CBIGA  (defined in MIEV0) */
/*    Routines called :  CONFRA */
/*    INTERNAL VARIABLES : */
/*       CONFRA     Value of Lentz continued fraction for CBIGA(NTRM), */
/*                     used to initialize downward recurrence */
/*       DOWN       = True, use down-recurrence.  False, do not. */
/*       F1,F2,F3   Arithmetic statement functions used in determining */
/*                     whether to use up-  or down-recurrence */
/*                     ( Ref. 2, Eqs. 6-8 ) */
/*       MRE        Real refractive index */
/*       MIM        Imaginary refractive index */
/*       REZINV     1 / ( MRE * XX ); temporary variable for recurrence */
/*       ZINV       1 / ( CIOR * XX ); temporary variable for recurrence */
/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Functions .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Statement Functions .. */
/*     .. */
/*     .. Statement Function definitions .. */
/*                                                   ** Eq. R47c */
/*                                                   ** Eq. R47b */
/*                                                   ** Eq. R47a */
/*     .. */
/*                                  ** Decide whether BigA can be */
/*                                  ** calculated by up-recurrence */
    /* Parameter adjustments */
    --cbiga;
    --rbiga;

    /* Function Body */
    mre = cior->r;
    mim = (r__1 = (real)r_imag(cior), (real)dabs(r__1));
    if (mre < 1.f || mre > 10.f || mim > 10.f) {
	down = TRUE_;
    } else if (*yesang) {
	down = TRUE_;
/*                                                    ** Eq. R48 */
	if (mim * *xx < 3.9f + mre * (-10.8f + 13.78f * mre)) {
	    down = FALSE_;
	}
    } else {
	down = TRUE_;
/*                                                    ** Eq. R48 */
/* Computing 2nd power */
	r__1 = mre;
/* Computing 3rd power */
	r__2 = mre;
	if (mim * *xx < -8.f + r__1 * r__1 * (26.22f + mre * (-.4474f + r__2 *
		 (r__2 * r__2) * (.00204f - 1.75e-4f * mre)))) {
	    down = FALSE_;
	}
    }
    q__2.r = *xx * cior->r, q__2.i = *xx * cior->i;
    c_div(&q__1, &c_b6, &q__2);
    zinv.r = q__1.r, zinv.i = q__1.i;
    rezinv = 1.f / (mre * *xx);
    if (down) {
/*                          ** Compute initial high-order BigA using */
/*                          ** Lentz method ( Ref. 1, pp. 17-20 ) */
	confra_(&q__1, ntrm, &zinv);
	ctmp.r = q__1.r, ctmp.i = q__1.i;
/*                                   *** Downward recurrence for BigA */
	if (*noabs) {
/*                                        ** No-absorption case; Eq (R23) */
	    rbiga[*ntrm] = ctmp.r;
	    for (n = *ntrm; n >= 2; --n) {
		rbiga[n - 1] = n * rezinv - 1.f / (n * rezinv + rbiga[n]);
/* L10: */
	    }
	} else {
/*                                         ** Absorptive case; Eq (R23) */
	    i__1 = *ntrm;
	    cbiga[i__1].r = ctmp.r, cbiga[i__1].i = ctmp.i;
	    for (n = *ntrm; n >= 2; --n) {
		i__1 = n - 1;
		d__1 = (doublereal) n;
		q__2.r = (real)d__1 * zinv.r, q__2.i = (real)d__1 * zinv.i;
		d__2 = (doublereal) n;
		q__5.r = (real)d__2 * zinv.r, q__5.i = (real)d__2 * zinv.i;
		i__2 = n;
		q__4.r = q__5.r + cbiga[i__2].r, q__4.i = q__5.i + cbiga[i__2]
			.i;
		c_div(&q__3, &c_b6, &q__4);
		q__1.r = q__2.r - q__3.r, q__1.i = q__2.i - q__3.i;
		cbiga[i__1].r = q__1.r, cbiga[i__1].i = q__1.i;
/* L20: */
	    }
	}
    } else {
/*                            *** Upward recurrence for BigA */
	if (*noabs) {
/*                                  ** No-absorption case; Eq (R20,21) */
	    rtmp = (real)sin(mre * *xx);
	    rbiga[1] = -rezinv + rtmp / (rtmp * rezinv - (real)cos(mre * *xx));
	    i__1 = *ntrm;
	    for (n = 2; n <= i__1; ++n) {
		rbiga[n] = -(n * rezinv) + 1.f / (n * rezinv - rbiga[n - 1]);
/* L30: */
	    }
	} else {
/*                                     ** Absorptive case; Eq (R20,22) */
	    q__3.r = cior->r * -0.f - cior->i * -2.f, q__3.i = cior->r * -2.f 
		    + cior->i * -0.f;
	    q__2.r = *xx * q__3.r, q__2.i = *xx * q__3.i;
	    c_exp(&q__1, &q__2);
	    ctmp.r = q__1.r, ctmp.i = q__1.i;
	    q__2.r = -zinv.r, q__2.i = -zinv.i;
	    q__4.r = 1.f - ctmp.r, q__4.i = -ctmp.i;
	    q__7.r = 1.f - ctmp.r, q__7.i = -ctmp.i;
	    q__6.r = zinv.r * q__7.r - zinv.i * q__7.i, q__6.i = zinv.r * 
		    q__7.i + zinv.i * q__7.r;
	    q__9.r = ctmp.r + 1.f, q__9.i = ctmp.i;
	    q__8.r = q__9.r * 0.f - q__9.i * 1.f, q__8.i = q__9.r * 1.f + 
		    q__9.i * 0.f;
	    q__5.r = q__6.r - q__8.r, q__5.i = q__6.i - q__8.i;
	    c_div(&q__3, &q__4, &q__5);
	    q__1.r = q__2.r + q__3.r, q__1.i = q__2.i + q__3.i;
	    cbiga[1].r = q__1.r, cbiga[1].i = q__1.i;
	    i__1 = *ntrm;
	    for (n = 2; n <= i__1; ++n) {
		i__2 = n;
		d__1 = (doublereal) n;
		q__3.r = (real)d__1 * zinv.r, q__3.i = (real)d__1 * zinv.i;
		q__2.r = -q__3.r, q__2.i = -q__3.i;
		d__2 = (doublereal) n;
		q__6.r = (real)d__2 * zinv.r, q__6.i = (real)d__2 * zinv.i;
		i__3 = n - 1;
		q__5.r = q__6.r - cbiga[i__3].r, q__5.i = q__6.i - cbiga[i__3]
			.i;
		c_div(&q__4, &c_b6, &q__5);
		q__1.r = q__2.r + q__4.r, q__1.i = q__2.i + q__4.i;
		cbiga[i__2].r = q__1.r, cbiga[i__2].i = q__1.i;
/* L40: */
	    }
	}
    }
    return 0;
} /* biga_ */

void confra_(complex * ret_val, integer *n, complex *zinv)
{
    /* Initialized data */

    static real eps1 = .01f;
    static real eps2 = 1e-8f;
    static integer maxit = 10000;

    /* System generated locals */
    integer i__1;
    real r__1, r__2;
    doublereal d__1;
    complex q__1, q__2;

    /* Builtin functions */
    /*
    void c_div(complex *, complex *, complex *);
    double c_abs(complex *), r_imag(complex *);
    */

    /* Local variables */
    static integer kk, mm;
    static complex cak, cdtd, capt, cntn;
    static integer kount;
    static complex cdenom, cnumer;

/*         Compute Bessel function ratio A-sub-N from its */
/*         continued fraction using Lentz method */
/*         ZINV = Reciprocal of argument of A */
/*    I N T E R N A L    V A R I A B L E S */
/*    ------------------------------------ */
/*    CAK      Term in continued fraction expansion of A (Eq. R25) */
/*    CAPT     Factor used in Lentz iteration for A (Eq. R27) */
/*    CNUMER   Numerator   in capT  ( Eq. R28A ) */
/*    CDENOM   Denominator in capT  ( Eq. R28B ) */
/*    CDTD     Product of two successive denominators of capT factors */
/*                 ( Eq. R34C ) */
/*    CNTN     Product of two successive numerators of capT factors */
/*                 ( Eq. R34B ) */
/*    EPS1     Ill-conditioning criterion */
/*    EPS2     Convergence criterion */
/*    KK       Subscript k of cAk  ( Eq. R25B ) */
/*    KOUNT    Iteration counter ( used to prevent infinite looping ) */
/*    MAXIT    Max. allowed no. of iterations */
/*    MM       + 1  and - 1, alternately */
/* -------------------------------------------------------------------- */
/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*                                 ** Eq. R25a */
    i__1 = *n + 1;
    d__1 = (doublereal) i__1;
    q__1.r = (real)d__1 * zinv->r, q__1.i = (real)d__1 * zinv->i;
     ret_val->r = q__1.r,  ret_val->i = q__1.i;
    mm = -1;
    kk = (*n << 1) + 3;
/*                                 ** Eq. R25b, k=2 */
    i__1 = mm * kk;
    d__1 = (doublereal) i__1;
    q__1.r = (real)d__1 * zinv->r, q__1.i = (real)d__1 * zinv->i;
    cak.r = q__1.r, cak.i = q__1.i;
    cdenom.r = cak.r, cdenom.i = cak.i;
    c_div(&q__2, &c_b6,  ret_val);
    q__1.r = cdenom.r + q__2.r, q__1.i = cdenom.i + q__2.i;
    cnumer.r = q__1.r, cnumer.i = q__1.i;
    kount = 1;
L10:
    ++kount;
    if (kount > maxit) {
	sasfit_f2c_errmsg_("ConFra--Iteration failed to converge", &c_true, (ftnlen)36);
    }
    mm = -mm;
    kk += 2;
/*                                 ** Eq. R25b */
    i__1 = mm * kk;
    d__1 = (doublereal) i__1;
    q__1.r = (real)d__1 * zinv->r, q__1.i = (real)d__1 * zinv->i;
    cak.r = q__1.r, cak.i = q__1.i;
/*                                          ** Eq. R32 */
    c_div(&q__1, &cnumer, &cak);
    c_div(&q__2, &cdenom, &cak);
    if (c_abs(&q__1) <= eps1 || c_abs(&q__2) <= eps1) {
/*                                  ** Ill-conditioned case -- stride */
/*                                  ** two terms instead of one */
/*                                       ** Eq. R34 */
	q__2.r = cak.r * cnumer.r - cak.i * cnumer.i, q__2.i = cak.r * 
		cnumer.i + cak.i * cnumer.r;
	q__1.r = q__2.r + 1.f, q__1.i = q__2.i;
	cntn.r = q__1.r, cntn.i = q__1.i;
	q__2.r = cak.r * cdenom.r - cak.i * cdenom.i, q__2.i = cak.r * 
		cdenom.i + cak.i * cdenom.r;
	q__1.r = q__2.r + 1.f, q__1.i = q__2.i;
	cdtd.r = q__1.r, cdtd.i = q__1.i;
/*                                           ** Eq. R33 */
	c_div(&q__2, &cntn, &cdtd);
	q__1.r = q__2.r *  ret_val->r - q__2.i *  ret_val->i, q__1.i = q__2.r 
		*  ret_val->i + q__2.i *  ret_val->r;
	 ret_val->r = q__1.r,  ret_val->i = q__1.i;
	mm = -mm;
	kk += 2;
/*                                 ** Eq. R25b */
	i__1 = mm * kk;
	d__1 = (doublereal) i__1;
	q__1.r = (real)d__1 * zinv->r, q__1.i = (real)d__1 * zinv->i;
	cak.r = q__1.r, cak.i = q__1.i;
/*                                      ** Eq. R35 */
	c_div(&q__2, &cnumer, &cntn);
	q__1.r = cak.r + q__2.r, q__1.i = cak.i + q__2.i;
	cnumer.r = q__1.r, cnumer.i = q__1.i;
	c_div(&q__2, &cdenom, &cdtd);
	q__1.r = cak.r + q__2.r, q__1.i = cak.i + q__2.i;
	cdenom.r = q__1.r, cdenom.i = q__1.i;
	++kount;
	goto L10;
    } else {
/*                           *** Well-conditioned case */
/*                                  ** Eq. R27 */
	c_div(&q__1, &cnumer, &cdenom);
	capt.r = q__1.r, capt.i = q__1.i;
/*                                  ** Eq. R26 */
	q__1.r = capt.r *  ret_val->r - capt.i *  ret_val->i, q__1.i = capt.r 
		*  ret_val->i + capt.i *  ret_val->r;
	 ret_val->r = q__1.r,  ret_val->i = q__1.i;
/*                                  ** Check for convergence; Eq. R31 */
	if ((r__1 = capt.r - 1.f, dabs(r__1)) >= eps2 || (r__2 = (real)r_imag(&capt)
		, (real)dabs(r__2)) >= eps2) {
/*                                        ** Eq. R30 */
	    c_div(&q__2, &c_b6, &cnumer);
	    q__1.r = cak.r + q__2.r, q__1.i = cak.i + q__2.i;
	    cnumer.r = q__1.r, cnumer.i = q__1.i;
	    c_div(&q__2, &c_b6, &cdenom);
	    q__1.r = cak.r + q__2.r, q__1.i = cak.i + q__2.i;
	    cdenom.r = q__1.r, cdenom.i = q__1.i;
	    goto L10;
	}
    }
    return ;
} /* confra_ */

int miprnt_(logical *prnt, real *xx, logical *perfct, 
	complex *crefin, integer *numang, real *xmu, real *qext, real *qsca, 
	real *gqsc, integer *nmom, integer *ipolzn, integer *momdim, logical *
	calcmo, real *pmom, complex *sforw, complex *sback, complex *tforw, 
	complex *tback, complex *s1, complex *s2)
{
    /* System generated locals */
    integer pmom_dim1, pmom_offset, i__1, i__2;
    real r__1, r__2;
    complex q__1, q__2, q__3;

    /* Builtin functions */
    /*
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);
    double r_imag(complex *);
    void r_cnjg(complex *, complex *);
    integer s_wsfi(icilist *), e_wsfi(void);
    */

    /* Local variables */
    static integer i__, j, m;
    static real i1, i2;
    static char fmat[22];
    static real fnorm;

    /* Fortran I/O blocks */
    static cilist io___129 = { 0, 6, 0, "('1',10X,A,1P,E11.4)", 0 };
    static cilist io___130 = { 0, 6, 0, "('1',10X,3(A,1P,E11.4))", 0 };
    static cilist io___131 = { 0, 6, 0, "(/,A)", 0 };
    static cilist io___135 = { 0, 6, 0, "( I4, F10.6, 1P,10E11.3 )", 0 };
    static cilist io___136 = { 0, 6, 0, "(/,A,9X,A,17X,A,17X,A,/,(0P,F7.2, 1"
	    "P,6E12.3) )", 0 };
    static cilist io___137 = { 0, 6, 0, "(/,4(A,1P,E11.4))", 0 };
    static cilist io___138 = { 0, 6, 0, "(/,A)", 0 };
    static cilist io___139 = { 0, 6, 0, "('+',27X,A)", 0 };
    static cilist io___140 = { 0, 6, 0, "('+',33X,A)", 0 };
    static cilist io___141 = { 0, 6, 0, "('+',33X,A)", 0 };
    static cilist io___144 = { 0, 6, 0, "(A,I4)", 0 };
    static icilist io___147 = { 0, fmat, 0, "(A,I2,A)", 22, 1 };
    static cilist io___148 = { 0, 6, 0, fmat, 0 };


/*         Print scattering quantities of a single particle */
/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
    /* Parameter adjustments */
    --prnt;
    --xmu;
    pmom_dim1 = *momdim - 0 + 1;
    pmom_offset = 0 + pmom_dim1;
    pmom -= pmom_offset;
    --calcmo;
    --tforw;
    --tback;
    --s1;
    --s2;

    /* Function Body */
    if (*perfct) {
	s_wsfe(&io___129);
	do_fio(&c__1, "Perfectly Conducting Case, size parameter =", (ftnlen)
		43);
	do_fio(&c__1, (char *)&(*xx), (ftnlen)sizeof(real));
	e_wsfe();
    }
    if (! (*perfct)) {
	s_wsfe(&io___130);
	do_fio(&c__1, "Refractive Index:  Real ", (ftnlen)24);
	r__1 = crefin->r;
	do_fio(&c__1, (char *)&r__1, (ftnlen)sizeof(real));
	do_fio(&c__1, "  Imag ", (ftnlen)7);
	r__2 = (real)r_imag(crefin);
	do_fio(&c__1, (char *)&r__2, (ftnlen)sizeof(real));
	do_fio(&c__1, ",   Size Parameter =", (ftnlen)20);
	do_fio(&c__1, (char *)&(*xx), (ftnlen)sizeof(real));
	e_wsfe();
    }
    if (prnt[1] && *numang > 0) {
	s_wsfe(&io___131);
	do_fio(&c__1, "    cos(angle)  ------- S1 ---------  ------- S2 ----"
		"-----  --- S1*conjg(S2) ---   i1=S1**2   i2=S2**2  (i1+i2)/2"
		"  DEG POLZN", (ftnlen)124);
	e_wsfe();
	i__1 = *numang;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = i__;
/* Computing 2nd power */
	    r__1 = s1[i__2].r;
/* Computing 2nd power */
	    r__2 = (real)r_imag(&s1[i__]);
	    i1 = r__1 * r__1 + r__2 * r__2;
	    i__2 = i__;
/* Computing 2nd power */
	    r__1 = s2[i__2].r;
/* Computing 2nd power */
	    r__2 = (real)r_imag(&s2[i__]);
	    i2 = r__1 * r__1 + r__2 * r__2;
	    s_wsfe(&io___135);
	    do_fio(&c__1, (char *)&i__, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&xmu[i__], (ftnlen)sizeof(real));
	    do_fio(&c__2, (char *)&s1[i__], (ftnlen)sizeof(real));
	    do_fio(&c__2, (char *)&s2[i__], (ftnlen)sizeof(real));
	    i__2 = i__;
	    r_cnjg(&q__3, &s2[i__]);
	    q__2.r = s1[i__2].r * q__3.r - s1[i__2].i * q__3.i, q__2.i = s1[
		    i__2].r * q__3.i + s1[i__2].i * q__3.r;
	    q__1.r = q__2.r, q__1.i = q__2.i;
	    do_fio(&c__2, (char *)&q__1, (ftnlen)sizeof(real));
	    do_fio(&c__1, (char *)&i1, (ftnlen)sizeof(real));
	    do_fio(&c__1, (char *)&i2, (ftnlen)sizeof(real));
	    r__1 = (i1 + i2) * .5f;
	    do_fio(&c__1, (char *)&r__1, (ftnlen)sizeof(real));
	    r__2 = (i2 - i1) / (i2 + i1);
	    do_fio(&c__1, (char *)&r__2, (ftnlen)sizeof(real));
	    e_wsfe();
/* L10: */
	}
    }
    if (prnt[2]) {
	s_wsfe(&io___136);
	do_fio(&c__1, "  Angle", (ftnlen)7);
	do_fio(&c__1, "S-sub-1", (ftnlen)7);
	do_fio(&c__1, "T-sub-1", (ftnlen)7);
	do_fio(&c__1, "T-sub-2", (ftnlen)7);
	do_fio(&c__1, (char *)&c_b153, (ftnlen)sizeof(real));
	do_fio(&c__2, (char *)&(*sforw), (ftnlen)sizeof(real));
	do_fio(&c__2, (char *)&tforw[1], (ftnlen)sizeof(real));
	do_fio(&c__2, (char *)&tforw[2], (ftnlen)sizeof(real));
	do_fio(&c__1, (char *)&c_b158, (ftnlen)sizeof(real));
	do_fio(&c__2, (char *)&(*sback), (ftnlen)sizeof(real));
	do_fio(&c__2, (char *)&tback[1], (ftnlen)sizeof(real));
	do_fio(&c__2, (char *)&tback[2], (ftnlen)sizeof(real));
	e_wsfe();
	s_wsfe(&io___137);
	do_fio(&c__1, " Efficiency Factors,  extinction:", (ftnlen)33);
	do_fio(&c__1, (char *)&(*qext), (ftnlen)sizeof(real));
	do_fio(&c__1, "   scattering:", (ftnlen)14);
	do_fio(&c__1, (char *)&(*qsca), (ftnlen)sizeof(real));
	do_fio(&c__1, "   absorption:", (ftnlen)14);
	r__1 = *qext - *qsca;
	do_fio(&c__1, (char *)&r__1, (ftnlen)sizeof(real));
	do_fio(&c__1, "   rad. pressure:", (ftnlen)17);
	r__2 = *qext - *gqsc;
	do_fio(&c__1, (char *)&r__2, (ftnlen)sizeof(real));
	e_wsfe();
	if (*nmom > 0) {
	    s_wsfe(&io___138);
	    do_fio(&c__1, " Normalized moments of :", (ftnlen)24);
	    e_wsfe();
	    if (*ipolzn == 0) {
		s_wsfe(&io___139);
		do_fio(&c__1, "Phase Fcn", (ftnlen)9);
		e_wsfe();
	    }
	    if (*ipolzn > 0) {
		s_wsfe(&io___140);
		do_fio(&c__1, "M1           M2          S21          D21", (
			ftnlen)41);
		e_wsfe();
	    }
	    if (*ipolzn < 0) {
		s_wsfe(&io___141);
		do_fio(&c__1, "R1           R2           R3           R4", (
			ftnlen)41);
		e_wsfe();
	    }
/* Computing 2nd power */
	    r__1 = *xx;
	    fnorm = 4.f / (r__1 * r__1 * *qsca);
	    i__1 = *nmom;
	    for (m = 0; m <= i__1; ++m) {
		s_wsfe(&io___144);
		do_fio(&c__1, "      Moment no.", (ftnlen)16);
		do_fio(&c__1, (char *)&m, (ftnlen)sizeof(integer));
		e_wsfe();
		for (j = 1; j <= 4; ++j) {
		    if (calcmo[j]) {
			s_wsfi(&io___147);
			do_fio(&c__1, "( '+', T", (ftnlen)8);
			i__2 = (j - 1) * 13 + 24;
			do_fio(&c__1, (char *)&i__2, (ftnlen)sizeof(integer));
			do_fio(&c__1, ", 1P,E13.4 )", (ftnlen)12);
			e_wsfi();
			s_wsfe(&io___148);
			r__1 = fnorm * pmom[m + j * pmom_dim1];
			do_fio(&c__1, (char *)&r__1, (ftnlen)sizeof(real));
			e_wsfe();
		    }
/* L20: */
		}
/* L30: */
	    }
	}
    }
    return 0;
} /* miprnt_ */

int small1_(real *xx, integer *numang, real *xmu, real *qext,
	 real *qsca, real *gqsc, complex *sforw, complex *sback, complex *s1, 
	complex *s2, complex *tforw, complex *tback, complex *a, complex *b)
{
    /* System generated locals */
    integer i__1, i__2, i__3, i__4;
    real r__1, r__2, r__3, r__4, r__5, r__6, r__7, r__8, r__9;
    complex q__1, q__2, q__3, q__4, q__5, q__6, q__7, q__8;

    /* Builtin functions */
    /*
    void c_div(complex *, complex *, complex *);
    double r_imag(complex *);
    void r_cnjg(complex *, complex *);
    */

    /* Local variables */
    static integer j;
    static real rtmp;

/*       Small-particle limit of Mie quantities in totally reflecting */
/*       limit ( Mie series truncated after 2 terms ) */
/*        A,B       First two Mie coefficients, with numerator and */
/*                  denominator expanded in powers of XX ( a factor */
/*                  of XX**3 is missing but is restored before return */
/*                  to calling program )  ( Ref. 2, p. 1508 ) */
/*     .. Parameters .. */
/*     .. */
/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Statement Functions .. */
/*     .. */
/*     .. Statement Function definitions .. */
/*     .. */
/*                                                       ** Eq. R/A.5 */
    /* Parameter adjustments */
    --b;
    --a;
    --tback;
    --tforw;
    --s2;
    --s1;
    --xmu;

    /* Function Body */
/* Computing 2nd power */
    r__2 = *xx;
    r__1 = (1.f - r__2 * r__2 * .2f) * .66666666666666663f;
    q__2.r = 0.f, q__2.i = r__1;
/* Computing 2nd power */
    r__4 = *xx;
    r__3 = 1.f - r__4 * r__4 * .5f;
/* Computing 3rd power */
    r__6 = *xx;
    r__5 = r__6 * (r__6 * r__6) * .66666666666666663f;
    q__3.r = r__3, q__3.i = r__5;
    c_div(&q__1, &q__2, &q__3);
    a[1].r = q__1.r, a[1].i = q__1.i;
/*                                                       ** Eq. R/A.6 */
/* Computing 2nd power */
    r__2 = *xx;
    r__1 = -(1.f - r__2 * r__2 * .1f) / 3.f;
    q__2.r = 0.f, q__2.i = r__1;
/* Computing 2nd power */
    r__4 = *xx;
    r__3 = r__4 * r__4 * .5f + 1.f;
/* Computing 3rd power */
    r__6 = *xx;
    r__5 = -(r__6 * (r__6 * r__6)) / 3.f;
    q__3.r = r__3, q__3.i = r__5;
    c_div(&q__1, &q__2, &q__3);
    b[1].r = q__1.r, b[1].i = q__1.i;
/*                                                       ** Eq. R/A.7,8 */
/* Computing 2nd power */
    r__2 = *xx;
    r__1 = r__2 * r__2 / 30.f;
    q__1.r = 0.f, q__1.i = r__1;
    a[2].r = q__1.r, a[2].i = q__1.i;
/* Computing 2nd power */
    r__2 = *xx;
    r__1 = -(r__2 * r__2) / 45.f;
    q__1.r = 0.f, q__1.i = r__1;
    b[2].r = q__1.r, b[2].i = q__1.i;
/*                                                       ** Eq. R/A.9 */
/* Computing 4th power */
    r__1 = *xx, r__1 *= r__1;
/* Computing 2nd power */
    r__2 = a[1].r;
/* Computing 2nd power */
    r__3 = (real)r_imag(&a[1]);
/* Computing 2nd power */
    r__4 = b[1].r;
/* Computing 2nd power */
    r__5 = (real)r_imag(&b[1]);
/* Computing 2nd power */
    r__6 = a[2].r;
/* Computing 2nd power */
    r__7 = (real)r_imag(&a[2]);
/* Computing 2nd power */
    r__8 = b[2].r;
/* Computing 2nd power */
    r__9 = (real)r_imag(&b[2]);
    *qsca = r__1 * r__1 * 6.f * (r__2 * r__2 + r__3 * r__3 + (r__4 * r__4 + 
	    r__5 * r__5) + (r__6 * r__6 + r__7 * r__7 + (r__8 * r__8 + r__9 * 
	    r__9)) * 1.6666666666666667f);
    *qext = *qsca;
/*                                                       ** Eq. R/A.10 */
/* Computing 4th power */
    r__1 = *xx, r__1 *= r__1;
    q__4.r = a[2].r + b[1].r, q__4.i = a[2].i + b[1].i;
    r_cnjg(&q__3, &q__4);
    q__2.r = a[1].r * q__3.r - a[1].i * q__3.i, q__2.i = a[1].r * q__3.i + a[
	    1].i * q__3.r;
    q__7.r = a[2].r * .55555555555555558f, q__7.i = a[2].i * 
	    .55555555555555558f;
    q__6.r = b[1].r + q__7.r, q__6.i = b[1].i + q__7.i;
    r_cnjg(&q__8, &b[2]);
    q__5.r = q__6.r * q__8.r - q__6.i * q__8.i, q__5.i = q__6.r * q__8.i + 
	    q__6.i * q__8.r;
    q__1.r = q__2.r + q__5.r, q__1.i = q__2.i + q__5.i;
    *gqsc = r__1 * r__1 * 6.f * q__1.r;
/* Computing 3rd power */
    r__1 = *xx;
    rtmp = r__1 * (r__1 * r__1) * 1.5f;
    q__3.r = a[1].r + b[1].r, q__3.i = a[1].i + b[1].i;
    q__5.r = a[2].r + b[2].r, q__5.i = a[2].i + b[2].i;
    q__4.r = q__5.r * 1.6666666666666667f, q__4.i = q__5.i * 
	    1.6666666666666667f;
    q__2.r = q__3.r + q__4.r, q__2.i = q__3.i + q__4.i;
    q__1.r = rtmp * q__2.r, q__1.i = rtmp * q__2.i;
    sforw->r = q__1.r, sforw->i = q__1.i;
    q__3.r = a[1].r - b[1].r, q__3.i = a[1].i - b[1].i;
    q__5.r = a[2].r - b[2].r, q__5.i = a[2].i - b[2].i;
    q__4.r = q__5.r * 1.6666666666666667f, q__4.i = q__5.i * 
	    1.6666666666666667f;
    q__2.r = q__3.r - q__4.r, q__2.i = q__3.i - q__4.i;
    q__1.r = rtmp * q__2.r, q__1.i = rtmp * q__2.i;
    sback->r = q__1.r, sback->i = q__1.i;
    q__5.r = b[2].r * 2.f, q__5.i = b[2].i * 2.f;
    q__4.r = q__5.r - a[2].r, q__4.i = q__5.i - a[2].i;
    q__3.r = q__4.r * 1.6666666666666667f, q__3.i = q__4.i * 
	    1.6666666666666667f;
    q__2.r = b[1].r + q__3.r, q__2.i = b[1].i + q__3.i;
    q__1.r = rtmp * q__2.r, q__1.i = rtmp * q__2.i;
    tforw[1].r = q__1.r, tforw[1].i = q__1.i;
    q__5.r = a[2].r * 2.f, q__5.i = a[2].i * 2.f;
    q__4.r = q__5.r - b[2].r, q__4.i = q__5.i - b[2].i;
    q__3.r = q__4.r * 1.6666666666666667f, q__3.i = q__4.i * 
	    1.6666666666666667f;
    q__2.r = a[1].r + q__3.r, q__2.i = a[1].i + q__3.i;
    q__1.r = rtmp * q__2.r, q__1.i = rtmp * q__2.i;
    tforw[2].r = q__1.r, tforw[2].i = q__1.i;
    q__5.r = b[2].r * 2.f, q__5.i = b[2].i * 2.f;
    q__4.r = q__5.r + a[2].r, q__4.i = q__5.i + a[2].i;
    q__3.r = q__4.r * 1.6666666666666667f, q__3.i = q__4.i * 
	    1.6666666666666667f;
    q__2.r = b[1].r - q__3.r, q__2.i = b[1].i - q__3.i;
    q__1.r = rtmp * q__2.r, q__1.i = rtmp * q__2.i;
    tback[1].r = q__1.r, tback[1].i = q__1.i;
    q__5.r = a[2].r * 2.f, q__5.i = a[2].i * 2.f;
    q__4.r = q__5.r + b[2].r, q__4.i = q__5.i + b[2].i;
    q__3.r = q__4.r * 1.6666666666666667f, q__3.i = q__4.i * 
	    1.6666666666666667f;
    q__2.r = a[1].r - q__3.r, q__2.i = a[1].i - q__3.i;
    q__1.r = rtmp * q__2.r, q__1.i = rtmp * q__2.i;
    tback[2].r = q__1.r, tback[2].i = q__1.i;
    i__1 = *numang;
    for (j = 1; j <= i__1; ++j) {
/*                                                    ** Eq. R/A.11,12 */
	i__2 = j;
	i__3 = j;
	q__4.r = xmu[i__3] * b[1].r, q__4.i = xmu[i__3] * b[1].i;
	q__3.r = a[1].r + q__4.r, q__3.i = a[1].i + q__4.i;
	i__4 = j;
	q__7.r = xmu[i__4] * a[2].r, q__7.i = xmu[i__4] * a[2].i;
/* Computing 2nd power */
	r__2 = xmu[j];
	r__1 = r__2 * r__2 * 2.f - 1.f;
	q__8.r = r__1 * b[2].r, q__8.i = r__1 * b[2].i;
	q__6.r = q__7.r + q__8.r, q__6.i = q__7.i + q__8.i;
	q__5.r = q__6.r * 1.6666666666666667f, q__5.i = q__6.i * 
		1.6666666666666667f;
	q__2.r = q__3.r + q__5.r, q__2.i = q__3.i + q__5.i;
	q__1.r = rtmp * q__2.r, q__1.i = rtmp * q__2.i;
	s1[i__2].r = q__1.r, s1[i__2].i = q__1.i;
	i__2 = j;
	i__3 = j;
	q__4.r = xmu[i__3] * a[1].r, q__4.i = xmu[i__3] * a[1].i;
	q__3.r = b[1].r + q__4.r, q__3.i = b[1].i + q__4.i;
	i__4 = j;
	q__7.r = xmu[i__4] * b[2].r, q__7.i = xmu[i__4] * b[2].i;
/* Computing 2nd power */
	r__2 = xmu[j];
	r__1 = r__2 * r__2 * 2.f - 1.f;
	q__8.r = r__1 * a[2].r, q__8.i = r__1 * a[2].i;
	q__6.r = q__7.r + q__8.r, q__6.i = q__7.i + q__8.i;
	q__5.r = q__6.r * 1.6666666666666667f, q__5.i = q__6.i * 
		1.6666666666666667f;
	q__2.r = q__3.r + q__5.r, q__2.i = q__3.i + q__5.i;
	q__1.r = rtmp * q__2.r, q__1.i = rtmp * q__2.i;
	s2[i__2].r = q__1.r, s2[i__2].i = q__1.i;
/* L10: */
    }
/*                                     ** Recover actual Mie coefficients */
/* Computing 3rd power */
    r__2 = *xx;
    r__1 = r__2 * (r__2 * r__2);
    q__1.r = r__1 * a[1].r, q__1.i = r__1 * a[1].i;
    a[1].r = q__1.r, a[1].i = q__1.i;
/* Computing 3rd power */
    r__2 = *xx;
    r__1 = r__2 * (r__2 * r__2);
    q__1.r = r__1 * a[2].r, q__1.i = r__1 * a[2].i;
    a[2].r = q__1.r, a[2].i = q__1.i;
/* Computing 3rd power */
    r__2 = *xx;
    r__1 = r__2 * (r__2 * r__2);
    q__1.r = r__1 * b[1].r, q__1.i = r__1 * b[1].i;
    b[1].r = q__1.r, b[1].i = q__1.i;
/* Computing 3rd power */
    r__2 = *xx;
    r__1 = r__2 * (r__2 * r__2);
    q__1.r = r__1 * b[2].r, q__1.i = r__1 * b[2].i;
    b[2].r = q__1.r, b[2].i = q__1.i;
    return 0;
} /* small1_ */

int small2_(real *xx, complex *cior, logical *calcqe, 
	integer *numang, real *xmu, real *qext, real *qsca, real *gqsc, 
	complex *sforw, complex *sback, complex *s1, complex *s2, complex *
	tforw, complex *tback, complex *a, complex *b)
{
    /* System generated locals */
    integer i__1, i__2, i__3;
    real r__1, r__2, r__3, r__4, r__5, r__6, r__7, r__8, r__9, r__10, r__11, 
	    r__12;
    complex q__1, q__2, q__3, q__4, q__5, q__6, q__7, q__8, q__9, q__10, 
	    q__11, q__12, q__13, q__14, q__15, q__16, q__17, q__18, q__19, 
	    q__20, q__21;

    /* Builtin functions */
    /*
    void pow_ci(complex *, complex *, integer *), c_div(complex *, complex *, 
	    complex *);
    double r_imag(complex *);
    void r_cnjg(complex *, complex *);
    */

    /* Local variables */
    static integer j;
    static complex ctmp;
    static real rtmp;
    static complex ciorsq;

/*       Small-particle limit of Mie quantities for general refractive */
/*       index ( Mie series truncated after 2 terms ) */
/*        A,B       First two Mie coefficients, with numerator and */
/*                  denominator expanded in powers of XX ( a factor */
/*                  of XX**3 is missing but is restored before return */
/*                  to calling program ) */
/*        CIORSQ    Square of refractive index */
/*     .. Parameters .. */
/*     .. */
/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Array Arguments .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. Intrinsic Functions .. */
/*     .. */
/*     .. Statement Functions .. */
/*     .. */
/*     .. Statement Function definitions .. */
/*     .. */
    /* Parameter adjustments */
    --b;
    --a;
    --tback;
    --tforw;
    --s2;
    --s1;
    --xmu;

    /* Function Body */
    pow_ci(&q__1, cior, &c__2);
    ciorsq.r = q__1.r, ciorsq.i = q__1.i;
    q__2.r = ciorsq.r - 1.f, q__2.i = ciorsq.i;
    q__1.r = q__2.r * 0.f - q__2.i * .66666666666666663f, q__1.i = q__2.r * 
	    .66666666666666663f + q__2.i * 0.f;
    ctmp.r = q__1.r, ctmp.i = q__1.i;
/*                                           ** Eq. R42a */
/* Computing 2nd power */
    r__2 = *xx;
    r__1 = 1.f - r__2 * r__2 * .1f;
    q__6.r = ciorsq.r / 350.f, q__6.i = ciorsq.i / 350.f;
    q__5.r = q__6.r + .0035714285714285713f, q__5.i = q__6.i;
/* Computing 4th power */
    r__4 = *xx, r__4 *= r__4;
    r__3 = r__4 * r__4;
    q__4.r = r__3 * q__5.r, q__4.i = r__3 * q__5.i;
    q__3.r = r__1 + q__4.r, q__3.i = q__4.i;
    q__2.r = ctmp.r * q__3.r - ctmp.i * q__3.i, q__2.i = ctmp.r * q__3.i + 
	    ctmp.i * q__3.r;
    q__10.r = ciorsq.r + 2.f, q__10.i = ciorsq.i;
    q__13.r = ciorsq.r * .7f, q__13.i = ciorsq.i * .7f;
    q__12.r = 1.f - q__13.r, q__12.i = -q__13.i;
/* Computing 2nd power */
    r__6 = *xx;
    r__5 = r__6 * r__6;
    q__11.r = r__5 * q__12.r, q__11.i = r__5 * q__12.i;
    q__9.r = q__10.r + q__11.r, q__9.i = q__10.i + q__11.i;
    pow_ci(&q__18, &ciorsq, &c__2);
    q__17.r = q__18.r / 175.f, q__17.i = q__18.i / 175.f;
    q__19.r = ciorsq.r * .275f, q__19.i = ciorsq.i * .275f;
    q__16.r = q__17.r - q__19.r, q__16.i = q__17.i - q__19.i;
    q__15.r = q__16.r + .25f, q__15.i = q__16.i;
/* Computing 4th power */
    r__8 = *xx, r__8 *= r__8;
    r__7 = r__8 * r__8;
    q__14.r = r__7 * q__15.r, q__14.i = r__7 * q__15.i;
    q__8.r = q__9.r - q__14.r, q__8.i = q__9.i - q__14.i;
/* Computing 3rd power */
    r__10 = *xx;
    r__9 = r__10 * (r__10 * r__10);
    q__21.r = r__9 * ctmp.r, q__21.i = r__9 * ctmp.i;
/* Computing 2nd power */
    r__12 = *xx;
    r__11 = 1.f - r__12 * r__12 * .1f;
    q__20.r = r__11 * q__21.r, q__20.i = r__11 * q__21.i;
    q__7.r = q__8.r + q__20.r, q__7.i = q__8.i + q__20.i;
    c_div(&q__1, &q__2, &q__7);
    a[1].r = q__1.r, a[1].i = q__1.i;
/*                                           ** Eq. R42b */
/* Computing 2nd power */
    r__2 = *xx;
    r__1 = r__2 * r__2 / 30.f;
    q__3.r = r__1 * ctmp.r, q__3.i = r__1 * ctmp.i;
    q__7.r = ciorsq.r / 35.f, q__7.i = ciorsq.i / 35.f;
    q__6.r = q__7.r - .071428571428571425f, q__6.i = q__7.i;
/* Computing 2nd power */
    r__4 = *xx;
    r__3 = r__4 * r__4;
    q__5.r = r__3 * q__6.r, q__5.i = r__3 * q__6.i;
    q__4.r = q__5.r + 1.f, q__4.i = q__5.i;
    q__2.r = q__3.r * q__4.r - q__3.i * q__4.i, q__2.i = q__3.r * q__4.i + 
	    q__3.i * q__4.r;
    q__11.r = ciorsq.r / 15.f, q__11.i = ciorsq.i / 15.f;
    q__10.r = q__11.r - .16666666666666666f, q__10.i = q__11.i;
/* Computing 2nd power */
    r__6 = *xx;
    r__5 = r__6 * r__6;
    q__9.r = r__5 * q__10.r, q__9.i = r__5 * q__10.i;
    q__8.r = 1.f - q__9.r, q__8.i = -q__9.i;
    c_div(&q__1, &q__2, &q__8);
    b[1].r = q__1.r, b[1].i = q__1.i;
/*                                           ** Eq. R42c */
/* Computing 2nd power */
    r__2 = *xx;
    r__1 = r__2 * r__2 * .1f;
    q__3.r = r__1 * ctmp.r, q__3.i = r__1 * ctmp.i;
/* Computing 2nd power */
    r__4 = *xx;
    r__3 = 1.f - r__4 * r__4 / 14.f;
    q__2.r = r__3 * q__3.r, q__2.i = r__3 * q__3.i;
    q__6.r = ciorsq.r * 2.f, q__6.i = ciorsq.i * 2.f;
    q__5.r = q__6.r + 3.f, q__5.i = q__6.i;
    q__9.r = ciorsq.r / 7.f, q__9.i = ciorsq.i / 7.f;
    q__8.r = q__9.r - .5f, q__8.i = q__9.i;
/* Computing 2nd power */
    r__6 = *xx;
    r__5 = r__6 * r__6;
    q__7.r = r__5 * q__8.r, q__7.i = r__5 * q__8.i;
    q__4.r = q__5.r - q__7.r, q__4.i = q__5.i - q__7.i;
    c_div(&q__1, &q__2, &q__4);
    a[2].r = q__1.r, a[2].i = q__1.i;
/*                                           ** Eq. R40a */
/* Computing 4th power */
    r__1 = *xx, r__1 *= r__1;
/* Computing 2nd power */
    r__2 = a[1].r;
/* Computing 2nd power */
    r__3 = (real)r_imag(&a[1]);
/* Computing 2nd power */
    r__4 = b[1].r;
/* Computing 2nd power */
    r__5 = (real)r_imag(&b[1]);
/* Computing 2nd power */
    r__6 = a[2].r;
/* Computing 2nd power */
    r__7 = (real)r_imag(&a[2]);
    *qsca = r__1 * r__1 * 6.f * (r__2 * r__2 + r__3 * r__3 + (r__4 * r__4 + 
	    r__5 * r__5) + (r__6 * r__6 + r__7 * r__7) * 1.6666666666666667f);
/*                                           ** Eq. R40b */
    *qext = *qsca;
    if (*calcqe) {
	q__2.r = a[1].r + b[1].r, q__2.i = a[1].i + b[1].i;
	q__3.r = a[2].r * 1.6666666666666667f, q__3.i = a[2].i * 
		1.6666666666666667f;
	q__1.r = q__2.r + q__3.r, q__1.i = q__2.i + q__3.i;
	*qext = *xx * 6.f * q__1.r;
    }
/*                                           ** Eq. R40c */
/* Computing 4th power */
    r__1 = *xx, r__1 *= r__1;
    q__3.r = a[2].r + b[1].r, q__3.i = a[2].i + b[1].i;
    r_cnjg(&q__2, &q__3);
    q__1.r = a[1].r * q__2.r - a[1].i * q__2.i, q__1.i = a[1].r * q__2.i + a[
	    1].i * q__2.r;
    *gqsc = r__1 * r__1 * 6.f * q__1.r;
/* Computing 3rd power */
    r__1 = *xx;
    rtmp = r__1 * (r__1 * r__1) * 1.5f;
    q__3.r = a[1].r + b[1].r, q__3.i = a[1].i + b[1].i;
    q__4.r = a[2].r * 1.6666666666666667f, q__4.i = a[2].i * 
	    1.6666666666666667f;
    q__2.r = q__3.r + q__4.r, q__2.i = q__3.i + q__4.i;
    q__1.r = rtmp * q__2.r, q__1.i = rtmp * q__2.i;
    sforw->r = q__1.r, sforw->i = q__1.i;
    q__3.r = a[1].r - b[1].r, q__3.i = a[1].i - b[1].i;
    q__4.r = a[2].r * 1.6666666666666667f, q__4.i = a[2].i * 
	    1.6666666666666667f;
    q__2.r = q__3.r - q__4.r, q__2.i = q__3.i - q__4.i;
    q__1.r = rtmp * q__2.r, q__1.i = rtmp * q__2.i;
    sback->r = q__1.r, sback->i = q__1.i;
    q__3.r = a[2].r * 1.6666666666666667f, q__3.i = a[2].i * 
	    1.6666666666666667f;
    q__2.r = b[1].r - q__3.r, q__2.i = b[1].i - q__3.i;
    q__1.r = rtmp * q__2.r, q__1.i = rtmp * q__2.i;
    tforw[1].r = q__1.r, tforw[1].i = q__1.i;
    q__3.r = a[2].r * 3.3333333333333335f, q__3.i = a[2].i * 
	    3.3333333333333335f;
    q__2.r = a[1].r + q__3.r, q__2.i = a[1].i + q__3.i;
    q__1.r = rtmp * q__2.r, q__1.i = rtmp * q__2.i;
    tforw[2].r = q__1.r, tforw[2].i = q__1.i;
    tback[1].r = tforw[1].r, tback[1].i = tforw[1].i;
    q__3.r = a[2].r * 3.3333333333333335f, q__3.i = a[2].i * 
	    3.3333333333333335f;
    q__2.r = a[1].r - q__3.r, q__2.i = a[1].i - q__3.i;
    q__1.r = rtmp * q__2.r, q__1.i = rtmp * q__2.i;
    tback[2].r = q__1.r, tback[2].i = q__1.i;
    i__1 = *numang;
    for (j = 1; j <= i__1; ++j) {
/*                                      ** Eq. R40d,e */
	i__2 = j;
	q__5.r = a[2].r * 1.6666666666666667f, q__5.i = a[2].i * 
		1.6666666666666667f;
	q__4.r = b[1].r + q__5.r, q__4.i = b[1].i + q__5.i;
	i__3 = j;
	q__3.r = xmu[i__3] * q__4.r, q__3.i = xmu[i__3] * q__4.i;
	q__2.r = a[1].r + q__3.r, q__2.i = a[1].i + q__3.i;
	q__1.r = rtmp * q__2.r, q__1.i = rtmp * q__2.i;
	s1[i__2].r = q__1.r, s1[i__2].i = q__1.i;
	i__2 = j;
	i__3 = j;
	q__4.r = xmu[i__3] * a[1].r, q__4.i = xmu[i__3] * a[1].i;
	q__3.r = b[1].r + q__4.r, q__3.i = b[1].i + q__4.i;
	q__6.r = a[2].r * 1.6666666666666667f, q__6.i = a[2].i * 
		1.6666666666666667f;
/* Computing 2nd power */
	r__2 = xmu[j];
	r__1 = r__2 * r__2 * 2.f - 1.f;
	q__5.r = r__1 * q__6.r, q__5.i = r__1 * q__6.i;
	q__2.r = q__3.r + q__5.r, q__2.i = q__3.i + q__5.i;
	q__1.r = rtmp * q__2.r, q__1.i = rtmp * q__2.i;
	s2[i__2].r = q__1.r, s2[i__2].i = q__1.i;
/* L10: */
    }
/*                                     ** Recover actual Mie coefficients */
/* Computing 3rd power */
    r__2 = *xx;
    r__1 = r__2 * (r__2 * r__2);
    q__1.r = r__1 * a[1].r, q__1.i = r__1 * a[1].i;
    a[1].r = q__1.r, a[1].i = q__1.i;
/* Computing 3rd power */
    r__2 = *xx;
    r__1 = r__2 * (r__2 * r__2);
    q__1.r = r__1 * a[2].r, q__1.i = r__1 * a[2].i;
    a[2].r = q__1.r, a[2].i = q__1.i;
/* Computing 3rd power */
    r__2 = *xx;
    r__1 = r__2 * (r__2 * r__2);
    q__1.r = r__1 * b[1].r, q__1.i = r__1 * b[1].i;
    b[1].r = q__1.r, b[1].i = q__1.i;
    b[2].r = 0.f, b[2].i = 0.f;
    return 0;
} /* small2_ */

int testmi_(logical *compar, real *xx, complex *crefin, real 
	*mimcut, logical *perfct, logical *anyang, integer *nmom, integer *
	ipolzn, integer *numang, real *xmu, real *qext, real *qsca, real *
	gqsc, complex *sforw, complex *sback, complex *s1, complex *s2, 
	complex *tforw, complex *tback, real *pmom, integer *momdim)
{
    /* Initialized data */

    static real testqe = 2.459791f;
    static real testqs = 1.235144f;
    static real testgq = 1.139235f;
    static complex testsf = {61.49476f,-3.177994f};
    static complex testsb = {1.493434f,.2963657f};
    static complex tests1 = {-.154838f,-1.128972f};
    static complex tests2 = {.05669755f,.5425681f};
    static complex testtf[2] = { {12.95238f,-136.6436f},{48.54238f,133.4656f} 
	    };
    static complex testtb[2] = { {41.88414f,-15.57833f},{43.37758f,-15.28196f}
	     };
    static real testpm[2] = { 227.1975f,183.6898f };
    static real accur = 1e-4f;

    /* System generated locals */
    integer pmom_dim1, pmom_offset, i__1, i__2, i__3, i__4;
    real r__1, r__2, r__3, r__4, r__5, r__6, r__7;
    complex q__1, q__2;

    /* Builtin functions */
    /*
    double r_imag(complex *);
    void c_div(complex *, complex *, complex *);
    double c_abs(complex *);
    */

    /* Local variables */
    static integer m, n;
    static logical ok, prnt[2];
    static real xxsav;
    static logical calcmo[4];
    static complex cresav;
    static real mimsav;
    static logical persav, anysav;
    static integer iposav, nmosav, numsav;
    static real xmusav;

/*         Set up to run test case when  COMPAR = False;  when  = True, */
/*         compare Mie code test case results with correct answers */
/*         and abort if even one result is inaccurate. */
/*         The test case is :  Mie size parameter = 10 */
/*                             refractive index   = 1.5 - 0.1 i */
/*                             scattering angle = 140 degrees */
/*                             1 Sekera moment */
/*         Results for this case may be found among the test cases */
/*         at the end of reference (1). */
/*         *** NOTE *** When running on some computers, esp. in single */
/*         precision, the Accur criterion below may have to be relaxed. */
/*         However, if Accur must be set larger than 10**-3 for some */
/*         size parameters, your computer is probably not accurate */
/*         enough to do Mie computations for those size parameters. */
/*     Routines called :  ERRMSG, MIPRNT, TSTBAD */
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
/*     .. Statement Functions .. */
/*     .. */
    /* Parameter adjustments */
    --xmu;
    --s1;
    --s2;
    --tforw;
    --tback;
    pmom_dim1 = *momdim - 0 + 1;
    pmom_offset = 0 + pmom_dim1;
    pmom -= pmom_offset;

    /* Function Body */
/*     .. */
/*     .. Statement Function definitions .. */
/*     .. */
    if (! (*compar)) {
/*                                   ** Save certain user input values */
	xxsav = *xx;
	cresav.r = crefin->r, cresav.i = crefin->i;
	mimsav = *mimcut;
	persav = *perfct;
	anysav = *anyang;
	nmosav = *nmom;
	iposav = *ipolzn;
	numsav = *numang;
	xmusav = xmu[1];
/*                                   ** Reset input values for test case */
	*xx = 10.f;
	crefin->r = 1.5f, crefin->i = -.1f;
	*mimcut = 0.f;
	*perfct = FALSE_;
	*anyang = TRUE_;
	*nmom = 1;
	*ipolzn = -1;
	*numang = 1;
	xmu[1] = -.7660444f;
    } else {
/*                                    ** Compare test case results with */
/*                                    ** correct answers and abort if bad */
	ok = TRUE_;
	if ((r__1 = (*qext - testqe) / testqe, dabs(r__1)) > accur) {
	    r__3 = (r__2 = (real)(*qext - testqe) / testqe, (real)dabs(r__2));
	    ok = sasfit_f2c_tstbad_("QEXT", &r__3, (ftnlen)4);
	}
	if ((r__1 = (*qsca - testqs) / testqs, dabs(r__1)) > accur) {
	    r__3 = (r__2 = (real)(*qsca - testqs) / testqs, (real)dabs(r__2));
	    ok = sasfit_f2c_tstbad_("QSCA", &r__3, (ftnlen)4);
	}
	if ((r__1 = (*gqsc - testgq) / testgq, dabs(r__1)) > accur) {
	    r__3 = (r__2 = (real)(*gqsc - testgq) / testgq, (real)dabs(r__2));
	    ok = sasfit_f2c_tstbad_("GQSC", &r__3, (ftnlen)4);
	}
	r__1 = sforw->r;
	r__2 = testsf.r;
	r__4 = (real)r_imag(sforw);
	r__5 = (real)r_imag(&testsf);
	if ((r__3 = (r__1 - r__2) / r__2, dabs(r__3)) > accur || (r__6 = (
		r__4 - r__5) / r__5, dabs(r__6)) > accur) {
	    q__2.r = sforw->r - testsf.r, q__2.i = sforw->i - testsf.i;
	    c_div(&q__1, &q__2, &testsf);
	    r__7 = (real)c_abs(&q__1);
	    ok = sasfit_f2c_tstbad_("SFORW", &r__7, (ftnlen)5);
	}
	r__1 = sback->r;
	r__2 = testsb.r;
	r__4 = (real)r_imag(sback);
	r__5 = (real)r_imag(&testsb);
	if ((r__3 = (r__1 - r__2) / r__2, dabs(r__3)) > accur || (r__6 = (
		r__4 - r__5) / r__5, dabs(r__6)) > accur) {
	    q__2.r = sback->r - testsb.r, q__2.i = sback->i - testsb.i;
	    c_div(&q__1, &q__2, &testsb);
	    r__7 = (real)c_abs(&q__1);
	    ok = sasfit_f2c_tstbad_("SBACK", &r__7, (ftnlen)5);
	}
	r__1 = s1[1].r;
	r__2 = tests1.r;
	r__4 = (real)r_imag(&s1[1]);
	r__5 = (real)r_imag(&tests1);
	if ((r__3 = (r__1 - r__2) / r__2, dabs(r__3)) > accur || (r__6 = (
		r__4 - r__5) / r__5, dabs(r__6)) > accur) {
	    q__2.r = s1[1].r - tests1.r, q__2.i = s1[1].i - tests1.i;
	    c_div(&q__1, &q__2, &tests1);
	    r__7 = (real)c_abs(&q__1);
	    ok = sasfit_f2c_tstbad_("S1", &r__7, (ftnlen)2);
	}
	r__1 = s2[1].r;
	r__2 = tests2.r;
	r__4 = (real)r_imag(&s2[1]);
	r__5 = (real)r_imag(&tests2);
	if ((r__3 = (r__1 - r__2) / r__2, dabs(r__3)) > accur || (r__6 = (
		r__4 - r__5) / r__5, dabs(r__6)) > accur) {
	    q__2.r = s2[1].r - tests2.r, q__2.i = s2[1].i - tests2.i;
	    c_div(&q__1, &q__2, &tests2);
	    r__7 = (real)c_abs(&q__1);
	    ok = sasfit_f2c_tstbad_("S2", &r__7, (ftnlen)2);
	}
	for (n = 1; n <= 2; ++n) {
	    i__1 = n;
	    r__1 = tforw[i__1].r;
	    i__2 = n - 1;
	    r__2 = testtf[i__2].r;
	    r__4 = (real)r_imag(&tforw[n]);
	    r__5 = (real)r_imag(&testtf[n - 1]);
	    if ((r__3 = (r__1 - r__2) / r__2, dabs(r__3)) > accur || (r__6 = (
		    r__4 - r__5) / r__5, dabs(r__6)) > accur) {
		i__3 = n;
		i__4 = n - 1;
		q__2.r = tforw[i__3].r - testtf[i__4].r, q__2.i = tforw[i__3]
			.i - testtf[i__4].i;
		c_div(&q__1, &q__2, &testtf[n - 1]);
		r__7 = (real)c_abs(&q__1);
		ok = sasfit_f2c_tstbad_("TFORW", &r__7, (ftnlen)5);
	    }
	    i__1 = n;
	    r__1 = tback[i__1].r;
	    i__2 = n - 1;
	    r__2 = testtb[i__2].r;
	    r__4 = (real)r_imag(&tback[n]);
	    r__5 = (real)r_imag(&testtb[n - 1]);
	    if ((r__3 = (r__1 - r__2) / r__2, dabs(r__3)) > accur || (r__6 = (
		    r__4 - r__5) / r__5, dabs(r__6)) > accur) {
		i__3 = n;
		i__4 = n - 1;
		q__2.r = tback[i__3].r - testtb[i__4].r, q__2.i = tback[i__3]
			.i - testtb[i__4].i;
		c_div(&q__1, &q__2, &testtb[n - 1]);
		r__7 = (real)c_abs(&q__1);
		ok = sasfit_f2c_tstbad_("TBACK", &r__7, (ftnlen)5);
	    }
/* L10: */
	}
	for (m = 0; m <= 1; ++m) {
	    if ((r__1 = (pmom[m + pmom_dim1] - testpm[m]) / testpm[m], dabs(
		    r__1)) > accur) {
		r__3 = (r__2 = (real)(pmom[m + pmom_dim1] - testpm[m]) / testpm[m], (real)dabs(r__2));
		ok = sasfit_f2c_tstbad_("PMOM", &r__3, (ftnlen)4);
	    }
/* L20: */
	}
	if (! ok) {
	    prnt[0] = TRUE_;
	    prnt[1] = TRUE_;
	    calcmo[0] = TRUE_;
	    calcmo[1] = FALSE_;
	    calcmo[2] = FALSE_;
	    calcmo[3] = FALSE_;
	    miprnt_(prnt, xx, perfct, crefin, numang, &xmu[1], qext, qsca, 
		    gqsc, nmom, ipolzn, momdim, calcmo, &pmom[pmom_offset], 
		    sforw, sback, &tforw[1], &tback[1], &s1[1], &s2[1]);
	    sasfit_f2c_errmsg_("MIEV0 -- Self-test failed", &c_true, (ftnlen)25);
	}
/*                                       ** Restore user input values */
	*xx = xxsav;
	crefin->r = cresav.r, crefin->i = cresav.i;
	*mimcut = mimsav;
	*perfct = persav;
	*anyang = anysav;
	*nmom = nmosav;
	*ipolzn = iposav;
	*numang = numsav;
	xmu[1] = xmusav;
    }
    return 0;
} /* testmi_ */

