/*
 * src/sasfit_common/f2c_utils/sasfit_zspow.c
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

#include <gsl/gsl_math.h>
#include "../include/sasfit_utils_f2c.h"

int zspwa_(sasfit_func_zspow_t * fcn, integer * n, real * x, real * fvec, real * xtol, integer * maxfev, 
		integer * ml, integer * mu, real * epsfcn, real * diag, integer * mode, 
		real * factor, integer * nprint, integer * info, integer * nfev, real * fjac, 
		integer * ldfjac, real * r__, integer * lr, real * qtf, real * wa1, real * wa2, 
		real * wa3, real * wa4, real * par);
int uertst_(integer * ier, integer * name__);
int uspkd_(integer * packed, integer * nchars, integer * unpakd, integer * nchmtb);
int ugetio_(integer * iopt, integer * nin, integer * nout);
doublereal snrm2_(integer * n, real * sx, integer * incx);
int zspwb_(sasfit_func_zspow_t * fcn, integer * n, real * x, real * fvec, real * fjac,
                integer * ldfjac, integer * iflag, integer * ml, integer * mu,
                real * epsfcn, real * wa1, real * wa2, real * par);
int zspwc_(integer * n, real * r__, real * lr, real * diag, real * qtb,
                real * delta, real * x, real * wa1, real * wa2);
int zspwd_(integer * m, integer * n, real * a, integer * lda, real * v, real * w);
int zspwe_(integer * m, integer * n, real * s, integer * ls, real * u, real * v, real * w, logical * sing);
int zspwf_(integer * m, integer * n, real * q, integer * ldq, real * wa);
int zspwg_(integer * m, integer * n, real * a, integer * lda, logical * pivot, 
		integer * ipvt, integer * lipvt, real * rdiag, real * acnorm, real * wa);
int zspwd_(integer * m, integer * n, real * a, integer * lda, real * v, real * w);

static real c_b2 = (float).1;
static integer c__6 = 6;
static integer c__1 = 1;
static logical c_false = FALSE_;

/*   IMSL ROUTINE NAME   - ZSPOW */

/* ----------------------------------------------------------------------- */

/*   COMPUTER            - VAX/SINGLE */

/*   LATEST REVISION     - JULY 1, 1983 */

/*   PURPOSE             - SOLVE A SYSTEM OF NONLINEAR EQUATIONS */

/*   USAGE               - CALL ZSPOW (FCN,NSIG,N,ITMAX,PAR,X,FNORM, */
/*                           WK,IER) */

/*   ARGUMENTS    FCN    - THE NAME OF A USER-SUPPLIED SUBROUTINE WHICH */
/*                           EVALUATES THE SYSTEM OF EQUATIONS TO BE */
/*                           SOLVED. FCN MUST BE DECLARED EXTERNAL IN */
/*                           THE CALLING PROGRAM AND MUST HAVE THE */
/*                           FOLLOWING FORM, */
/*                             SUBROUTINE FCN(X,F,N,PAR) */
/*                             REAL X(N),F(N),PAR(1) */
/*                             F(1)= */
/*                              . */
/*                             F(N)= */
/*                             RETURN */
/*                             END */
/*                           GIVEN X(1)...X(N), FCN MUST EVALUATE THE */
/*                           FUNCTIONS F(1)...F(N) WHICH ARE TO BE MADE */
/*                           ZERO. X SHOULD NOT BE ALTERED BY FCN. THE */
/*                           PARAMETERS IN VECTOR PAR (SEE ARGUMENT */
/*                           PAR BELOW) MAY ALSO BE USED IN THE */
/*                           CALCULATION OF F(1)...F(N). */
/*                NSIG   - THE NUMBER OF DIGITS OF ACCURACY DESIRED */
/*                           IN THE COMPUTED ROOT. (INPUT) */
/*                N      - THE NUMBER OF EQUATIONS TO BE SOLVED AND */
/*                           THE NUMBER OF UNKNOWNS. (INPUT) */
/*                ITMAX  - THE MAXIMUM ALLOWABLE NUMBER OF ITERATIONS. */
/*                           (INPUT) THE MAXIMUM NUMBER OF CALLS TO FCN */
/*                           IS ITMAX*(N+1). SUGGESTED VALUE = 200. */
/*                PAR    - PAR CONTAINS A PARAMETER SET WHICH IS */
/*                           PASSED TO THE USER-SUPPLIED FUNCTION FCN. */
/*                           PAR MAY BE USED TO PASS ANY AUXILIARY */
/*                           PARAMETERS NECESSARY FOR COMPUTATION OF */
/*                           THE FUNCTION FCN. (INPUT) */
/*                X      - A VECTOR OF LENGTH N. (INPUT/OUTPUT) ON INPUT, */
/*                           X IS THE INITIAL APPROXIMATION TO THE ROOT. */
/*                           ON OUTPUT, X IS THE BEST APPROXIMATION TO */
/*                           THE ROOT FOUND BY ZSPOW. */
/*                FNORM  - ON OUTPUT, FNORM IS EQUAL TO */
/*                           F(1)**2+...F(N)**2 AT THE POINT X. */
/*                WK     - WORK VECTOR OF LENGTH N*(3*N+15)/2 */
/*                IER    - ERROR PARAMETER. (OUTPUT) */
/*                         TERMINAL ERROR */
/*                           IER = 129 INDICATES THAT THE NUMBER OF */
/*                             CALLS TO FCN HAS EXCEEDED ITMAX*(N+1). */
/*                             THE USER MAY TRY A NEW INITIAL GUESS. */
/*                           IER = 130 INDICATES THAT NSIG IS TOO */
/*                             LARGE.  NO FURTHER IMPROVEMENT IN THE */
/*                             APPROXIMATE SOLUTION X IS POSSIBLE. */
/*                             THE USER SHOULD DECREASE NSIG. */
/*                           IER = 131 INDICATES THAT THE ITERATION */
/*                             HAS NOT MADE GOOD PROGRESS.  THE USER */
/*                             MAY TRY A NEW INITIAL GUESS. */

/*   PRECISION/HARDWARE  - SINGLE AND DOUBLE/H32 */
/*                       - SINGLE/H36,H48,H60 */

/*   REQD. IMSL ROUTINES - SINGLE/UERTST,UGETIO,VBLA=SNRM2,ZSPWA, */
/*                           ZSPWB,ZSPWC,ZSPWD,ZSPWE,ZSPWF,ZSPWG */
/*                       - DOUBLE/UERTST,UGETIO,VBLA=DNRM2,ZSPWA, */
/*                           ZSPWB,ZSPWC,ZSPWD,ZSPWE,ZSPWF,ZSPWG */

/*   NOTATION            - INFORMATION ON SPECIAL NOTATION AND */
/*                           CONVENTIONS IS AVAILABLE IN THE MANUAL */
/*                           INTRODUCTION OR THROUGH IMSL ROUTINE UHELP */

/*   COPYRIGHT           - 1982 BY IMSL, INC. ALL RIGHTS RESERVED. */

/*   WARRANTY            - IMSL WARRANTS ONLY THAT IMSL TESTING HAS BEEN */
/*                           APPLIED TO THIS CODE. NO OTHER WARRANTY, */
/*                           EXPRESSED OR IMPLIED, IS APPLICABLE. */

/* ----------------------------------------------------------------------- */

int zspow_(sasfit_func_zspow_t * fcn, 
		integer * nsig, 
		integer * n, 
		integer * itmax, 
		real * par,
		real * x, 
		real * fnorm,
		real * wk, 
		integer * ier)
{
	/* Initialized data */

	static real factor = (float)100.;
	static real one = (float)1.;
	static real zero = (float)0.;

	/* System generated locals */
	integer i__1;

	/* Builtin functions */
	double pow_ri();

	/* Local variables */
	static integer mode, info, nfev;
	static real xtol;
	static integer i__, j, index;
	static integer index2, ml, lr, mu;
	static real epsfcn;
	static integer maxfev, nprint;

	/*                                  SPECIFICATIONS FOR ARGUMENTS */
	/*                                  SPECIFICATIONS FOR LOCAL VARIABLES */
	/* Parameter adjustments */
	--x;
	--par;
	--wk;

	/* Function Body */
	/*                                  FIRST EXECUTABLE STATEMENT */
	info = 0;
	/*                                  CALL ZSPWA */
	maxfev = *itmax * (*n + 1);
	xtol = (real)pow_ri(&c_b2, nsig);
	ml = *n - 1;
	mu = *n - 1;
	epsfcn = zero;
	mode = 2;
	i__1 = *n;
	for (j = 1; j <= i__1; ++j) 
	{
		wk[j] = one;
	/* L5: */
	}
	nprint = 0;
	lr = *n * (*n + 1) / 2;
	index = *n * 7 + lr;
	zspwa_(fcn, n, &x[1], &wk[*n * 6 + 1], &xtol, &maxfev, &ml, &mu, &epsfcn, 
	&wk[1], &mode, &factor, &nprint, &info, &nfev, &wk[index + 1], n, 
	&wk[*n * 7 + 1], &lr, &wk[*n + 1], &wk[(*n << 1) + 1], &wk[*n * 3 
	+ 1], &wk[(*n << 2) + 1], &wk[*n * 5 + 1], &par[1]);
	if (info == 5) 
	{
		info = 4;
	}
	*fnorm = (float)0.;
	i__1 = *n;
	for (i__ = 1; i__ <= i__1; ++i__) 
	{
		index2 = *n * 6 + i__;
		*fnorm += wk[index2] * wk[index2];
	/* L10: */
	}
	*ier = 0;
	if (info == 2) 
	{
		*ier = 129;
	}
	if (info == 3) 
	{
		*ier = 130;
	}
	if (info == 4) 
	{
		*ier = 131;
	}
	/*
	// if (*ier > 0) {
	// sasfit_err("No real solution for nonlinear equation in the initialization routine calc\n");
	// return 0;
	// uertst_(ier, "ZSPOW ", 6L);
	}
	*/
	return 0;
} 

/*   IMSL ROUTINE NAME   - ZSPWA */

/* ----------------------------------------------------------------------- */

/*   COMPUTER            - VAX/SINGLE */

/*   LATEST REVISION     - JUNE 1, 1982 */

/*   PURPOSE             - NUCLEUS CALLED ONLY BY IMSL SUBROUTINE ZSPOW */

/*   PRECISION/HARDWARE  - SINGLE AND DOUBLE/H32 */
/*                       - SINGLE/H36,H48,H60 */

/*   REQD. IMSL ROUTINES - SINGLE/VBLA=SNRM2,ZSPWB,ZSPWC,ZSPWD,ZSPWE, */
/*                           ZSPWF,ZSPWG */
/*                       - DOUBLE/VBLA=DNRM2,ZSPWB,ZSPWC,ZSPWD,ZSPWE, */
/*                           ZSPWF,ZSPWG */

/*   NOTATION            - INFORMATION ON SPECIAL NOTATION AND */
/*                           CONVENTIONS IS AVAILABLE IN THE MANUAL */
/*                           INTRODUCTION OR THROUGH IMSL ROUTINE UHELP */

/*   COPYRIGHT           - 1982 BY IMSL, INC. ALL RIGHTS RESERVED. */

/*   WARRANTY            - IMSL WARRANTS ONLY THAT IMSL TESTING HAS BEEN */
/*                           APPLIED TO THIS CODE. NO OTHER WARRANTY, */
/*                           EXPRESSED OR IMPLIED, IS APPLICABLE. */

/* ----------------------------------------------------------------------- */

int zspwa_(sasfit_func_zspow_t * fcn, integer * n, real * x, real * fvec, real * xtol, integer * maxfev, 
		integer * ml, integer * mu, real * epsfcn, real * diag, integer * mode, 
		real * factor, integer * nprint, integer * info, integer * nfev, real * fjac, 
		integer * ldfjac, real * r__, integer * lr, real * qtf, real * wa1, real * wa2, 
		real * wa3, real * wa4, real * par)
{
    /* Initialized data */

    static real spmpar = (float)1.1921e-7;
    static real one = (float)1.;
    static real p1 = (float).1;
    static real p5 = (float).5;
    static real p001 = (float).001;
    static real p0001 = (float)1e-4;
    static real zero = (float)0.;

    /* System generated locals */
    integer fjac_dim1, fjac_offset, i__1, i__2;
    real r__1, r__2, lr_real;

    /* Local variables */
    static logical sing;
    static integer iter;
    static real temp;
    static integer msum;
    static integer i__, j, l, iflag;
    static real delta;
    static logical jeval;
    static integer ncsuc;
    static real ratio, fnorm, pnorm, xnorm;
    static real fnorm1;
    static integer nslow1, nslow2, ncfail;
    static real actred, epsmch, prered;
    static integer jm1, iwa[1];
    static real sum;

/*                                  SPECIFICATIONS FOR ARGUMENTS */
/*                                  SPECIFICATIONS FOR LOCAL VARIABLES */
    /* Parameter adjustments */
    --wa4;
    --wa3;
    --wa2;
    --wa1;
    --qtf;
    --diag;
    --fvec;
    --x;
    fjac_dim1 = *ldfjac;
    fjac_offset = fjac_dim1 + 1;
    fjac -= fjac_offset;
    --r__;
    --par;

    /* Function Body */
/*                                  EPSMCH IS THE MACHINE PRECISION. */
/*                                  FIRST EXECUTABLE STATEMENT */
    epsmch = spmpar;
    *info = 0;
    iflag = 0;
    *nfev = 0;
/*                                  CHECK THE INPUT PARAMETERS FOR */
/*                                  ERRORS. */
    if (*n <= 0 || *xtol < zero || *maxfev <= 0 || *ml < 0 || *mu < 0 || *
	    factor <= zero || *ldfjac < *n || *lr < *n * (*n + 1) / 2) {
	goto L150;
    }
    if (*mode != 2) {
	goto L10;
    }
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	if (diag[j] <= zero) {
	    goto L150;
	}
/* L5: */
    }
L10:
/*                                  EVALUATE THE FUNCTION AT THE STARTING 
*/
/*                                  POINT AND CALCULATE ITS NORM. */
    iflag = 1;
    (*fcn)(&x[1], &fvec[1], n, &par[1]);
    *nfev = 1;
    if (iflag < 0) {
	goto L150;
    }
    fnorm = (real)snrm2_(n, &fvec[1], &c__1);
/*                                  DETERMINE THE NUMBER OF CALLS TO FCN 
*/
/*                                  NEEDED TO COMPUTE THE JACOBIAN */
/*                                  MATRIX. */

/* Computing MIN */
    i__1 = *ml + *mu + 1;
    msum = min(i__1,*n);

/*                                  INITIALIZE ITERATION COUNTER AND */
/*                                  MONITORS. */
    iter = 1;
    ncsuc = 0;
    ncfail = 0;
    nslow1 = 0;
    nslow2 = 0;
/*                                  BEGINNING OF THE OUTER LOOP. */
L15:
    jeval = TRUE_;
/*                                  CALCULATE THE JACOBIAN MATRIX. */
    iflag = 2;
    zspwb_(fcn, n, &x[1], &fvec[1], &fjac[fjac_offset], ldfjac, &iflag, ml, 
	    mu, epsfcn, &wa1[1], &wa2[1], &par[1]);
    *nfev += msum;
    if (iflag < 0) {
	goto L150;
    }
/*                                  COMPUTE THE QR FACTORIZATION OF THE */
/*                                  JACOBIAN. */
    zspwg_(n, n, &fjac[fjac_offset], ldfjac, &c_false, iwa, &c__1, &wa1[1], &
	    wa2[1], &wa3[1]);
/*                                  ON THE FIRST ITERATION AND IF MODE IS 
*/
/*                                  1, SCALE ACCORDING TO THE NORMS OF */
/*                                  THE COLUMNS OF THE INITIAL JACOBIAN. 
*/
    if (iter != 1) {
	goto L35;
    }
    if (*mode == 2) {
	goto L25;
    }
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	diag[j] = wa2[j];
	if (wa2[j] == zero) {
	    diag[j] = one;
	}
/* L20: */
    }
L25:
/*                                  ON THE FIRST ITERATION, CALCULATE THE 
*/
/*                                  NORM OF THE SCALED X AND INITIALIZE */
/*                                  THE STEP BOUND DELTA. */
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	wa3[j] = diag[j] * x[j];
/* L30: */
    }
    xnorm = (real)snrm2_(n, &wa3[1], &c__1);
    delta = *factor * xnorm;
    if (delta == zero) {
	delta = *factor;
    }
L35:
/*                                  FORM (Q TRANSPOSE)*FVEC AND STORE IN 
*/
/*                                  QTF. */
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	qtf[i__] = fvec[i__];
/* L40: */
    }
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	if (fjac[j + j * fjac_dim1] == zero) {
	    goto L55;
	}
	sum = zero;
	i__2 = *n;
	for (i__ = j; i__ <= i__2; ++i__) {
	    sum += fjac[i__ + j * fjac_dim1] * qtf[i__];
/* L45: */
	}
	temp = -sum / fjac[j + j * fjac_dim1];
	i__2 = *n;
	for (i__ = j; i__ <= i__2; ++i__) {
	    qtf[i__] += fjac[i__ + j * fjac_dim1] * temp;
/* L50: */
	}
L55:
/* L60: */
	;
    }
/*                                  COPY THE TRIANGULAR FACTOR OF THE QR 
*/
/*                                  FACTORIZATION INTO R. */
    sing = FALSE_;
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	l = j;
	jm1 = j - 1;
	if (jm1 < 1) {
	    goto L70;
	}
	i__2 = jm1;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    r__[l] = fjac[i__ + j * fjac_dim1];
	    l = l + *n - i__;
/* L65: */
	}
L70:
	r__[l] = wa1[j];
	if (wa1[j] == zero) {
	    sing = TRUE_;
	}
/* L75: */
    }
/*                                  ACCUMULATE THE ORTHOGONAL FACTOR IN */
/*                                  FJAC. */
    zspwf_(n, n, &fjac[fjac_offset], ldfjac, &wa1[1]);
/*                                  RESCALE IF NECESSARY. */
    if (*mode == 2) {
	goto L85;
    }
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
/* Computing MAX */
	r__1 = diag[j], r__2 = wa2[j];
	diag[j] = (real)dmax(r__1,r__2);
/* L80: */
    }
L85:
/*                                  BEGINNING OF THE INNER LOOP. */
L90:
/*                                  IF REQUESTED, CALL FCN TO ENABLE */
/*                                  PRINTING OF ITERATES. */
    if (*nprint <= 0) {
	goto L95;
    }
    iflag = 0;
    if (iflag < 0) {
	goto L150;
    }
L95:
/*                                  DETERMINE THE DIRECTION P. */
    lr_real = (real)*lr;
    zspwc_(n, &r__[1], &lr_real, &diag[1], &qtf[1], &delta, &wa1[1], &wa2[1], &wa3[1]);
/*                                  STORE THE DIRECTION P AND X + P. */
/*                                  CALCULATE THE NORM OF P. */
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	wa1[j] = -wa1[j];
	wa2[j] = x[j] + wa1[j];
	wa3[j] = diag[j] * wa1[j];
/* L100: */
    }
    pnorm = (real)snrm2_(n, &wa3[1], &c__1);
/*                                  ON THE FIRST ITERATION, ADJUST THE */
/*                                  INITIAL STEP BOUND. */
    if (iter == 1) {
	delta = (real)dmin(delta,pnorm);
    }
/*                                  EVALUATE THE FUNCTION AT X + P AND */
/*                                  CALCULATE ITS NORM. */
    iflag = 1;
    (*fcn)(&wa2[1], &wa4[1], n, &par[1]);
    ++(*nfev);
    if (iflag < 0) {
	goto L150;
    }
    fnorm1 = (real)snrm2_(n, &wa4[1], &c__1);
/*                                  COMPUTE THE SCALED ACTUAL REDUCTION. 
*/
    actred = -one;
    if (fnorm1 < fnorm) {
/* Computing 2nd power */
	r__1 = fnorm1 / fnorm;
	actred = one - r__1 * r__1;
    }
/*                                  COMPUTE THE SCALED PREDICTED */
/*                                  REDUCTION. */
    l = 1;
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	sum = zero;
	i__2 = *n;
	for (j = i__; j <= i__2; ++j) {
	    sum += r__[l] * wa1[j];
	    ++l;
/* L105: */
	}
	wa3[i__] = qtf[i__] + sum;
/* L110: */
    }
    temp = (real)snrm2_(n, &wa3[1], &c__1);
    prered = one;
    if (temp < fnorm) {
/* Computing 2nd power */
	r__1 = temp / fnorm;
	prered = one - r__1 * r__1;
    }
/*                                  COMPUTE THE RATIO OF THE ACTUAL TO */
/*                                  THE PREDICTED REDUCTION. */
    ratio = zero;
    if (prered > zero) {
	ratio = actred / prered;
    }
/*                                  UPDATE THE STEP BOUND. */
    if (ratio >= p1) {
	goto L115;
    }
    ncsuc = 0;
    ++ncfail;
    delta = p5 * delta;
    goto L120;
L115:
    ncfail = 0;
    ++ncsuc;
    if (ratio >= p5 || ncsuc > 1) {
/* Computing MAX */
	r__1 = delta, r__2 = pnorm / p5;
	delta = (real)dmax(r__1,r__2);
    }
    if ((r__1 = ratio - one, dabs(r__1)) <= p1) {
	delta = pnorm / p5;
    }
L120:
/*                                  TEST FOR SUCCESSFUL ITERATION. */
    if (ratio < p0001) {
	goto L130;
    }
/*                                  SUCCESSFUL ITERATION. UPDATE X, FVEC, 
*/
/*                                  AND THEIR NORMS. */
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	x[j] = wa2[j];
	wa2[j] = diag[j] * x[j];
	fvec[j] = wa4[j];
/* L125: */
    }
    xnorm = (real)snrm2_(n, &wa2[1], &c__1);
    fnorm = fnorm1;
    ++iter;
L130:
/*                                  DETERMINE THE PROGRESS OF THE */
/*                                  ITERATION. */
    ++nslow1;
    if (actred >= p001) {
	nslow1 = 0;
    }
    if (jeval) {
	++nslow2;
    }
    if (actred >= p1) {
	nslow2 = 0;
    }
/*                                  TEST FOR CONVERGENCE. */
    if (delta <= *xtol * xnorm || fnorm == zero) {
	*info = 1;
    }
    if (*info != 0) {
	goto L150;
    }
/*                                  TESTS FOR TERMINATION AND STRINGENT */
/*                                  TOLERANCES. */
    if (*nfev >= *maxfev) {
	*info = 2;
    }
/* Computing MAX */
    r__1 = p1 * delta;
    if (p1 * dmax(r__1,pnorm) <= epsmch * xnorm) {
	*info = 3;
    }
    if (nslow2 == 5) {
	*info = 4;
    }
    if (nslow1 == 10) {
	*info = 5;
    }
    if (*info != 0) {
	goto L150;
    }
/*                                  CRITERION FOR RECALCULATING JACOBIAN 
*/
/*                                  APPROXIMATION BY FORWARD DIFFERENCES. 
*/
    if (ncfail == 2) {
	goto L145;
    }
/*                                  CALCULATE THE RANK ONE MODIFICATION */
/*                                  TO THE JACOBIAN AND UPDATE QTF IF */
/*                                  NECESSARY. */
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	sum = zero;
	i__2 = *n;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    sum += fjac[i__ + j * fjac_dim1] * wa4[i__];
/* L135: */
	}
	wa2[j] = (sum - wa3[j]) / pnorm;
	wa1[j] = diag[j] * (diag[j] * wa1[j] / pnorm);
	if (ratio >= p0001) {
	    qtf[j] = sum;
	}
/* L140: */
    }
/*                                  COMPUTE THE QR FACTORIZATION OF THE */
/*                                  UPDATED JACOBIAN. */
    zspwe_(n, n, &r__[1], lr, &wa1[1], &wa2[1], &wa3[1], &sing);
    zspwd_(n, n, &fjac[fjac_offset], ldfjac, &wa2[1], &wa3[1]);
    zspwd_(&c__1, n, &qtf[1], &c__1, &wa2[1], &wa3[1]);
/*                                  END OF THE INNER LOOP. */
    jeval = FALSE_;
    goto L90;
L145:
/*                                  END OF THE OUTER LOOP. */
    goto L15;
L150:
/*                                  TERMINATION, EITHER NORMAL OR USER */
/*                                  IMPOSED. */
    if (iflag < 0) {
	*info = iflag;
    }
    iflag = 0;
    return 0;
} /* zspwa_ */


/*   IMSL ROUTINE NAME   - UERTST */

/* ----------------------------------------------------------------------- */

/*   COMPUTER            - VAX/SINGLE */

/*   LATEST REVISION     - JUNE 1, 1982 */

/*   PURPOSE             - PRINT A MESSAGE REFLECTING AN ERROR CONDITION */

/*   USAGE               - CALL UERTST (IER,NAME) */

/*   ARGUMENTS    IER    - ERROR PARAMETER. (INPUT) */
/*                           IER = I+J WHERE */
/*                             I = 128 IMPLIES TERMINAL ERROR MESSAGE, */
/*                             I =  64 IMPLIES WARNING WITH FIX MESSAGE, */
/*                             I =  32 IMPLIES WARNING MESSAGE. */
/*                             J = ERROR CODE RELEVANT TO CALLING */
/*                                 ROUTINE. */
/*                NAME   - A CHARACTER STRING OF LENGTH SIX PROVIDING */
/*                           THE NAME OF THE CALLING ROUTINE. (INPUT) */

/*   PRECISION/HARDWARE  - SINGLE/ALL */

/*   REQD. IMSL ROUTINES - UGETIO,USPKD */

/*   NOTATION            - INFORMATION ON SPECIAL NOTATION AND */
/*                           CONVENTIONS IS AVAILABLE IN THE MANUAL */
/*                           INTRODUCTION OR THROUGH IMSL ROUTINE UHELP */

/*   REMARKS      THE ERROR MESSAGE PRODUCED BY UERTST IS WRITTEN */
/*                TO THE STANDARD OUTPUT UNIT. THE OUTPUT UNIT */
/*                NUMBER CAN BE DETERMINED BY CALLING UGETIO AS */
/*                FOLLOWS..   CALL UGETIO(1,NIN,NOUT). */
/*                THE OUTPUT UNIT NUMBER CAN BE CHANGED BY CALLING */
/*                UGETIO AS FOLLOWS.. */
/*                                NIN = 0 */
/*                                NOUT = NEW OUTPUT UNIT NUMBER */
/*                                CALL UGETIO(3,NIN,NOUT) */
/*                SEE THE UGETIO DOCUMENT FOR MORE DETAILS. */

/*   COPYRIGHT           - 1982 BY IMSL, INC. ALL RIGHTS RESERVED. */

/*   WARRANTY            - IMSL WARRANTS ONLY THAT IMSL TESTING HAS BEEN */
/*                           APPLIED TO THIS CODE. NO OTHER WARRANTY, */
/*                           EXPRESSED OR IMPLIED, IS APPLICABLE. */

/* ----------------------------------------------------------------------- */

int uertst_(integer * ier, integer * name__)
{
    /* Initialized data */

    static struct {
	char e_1[24];
	integer e_2;
	} equiv_20 = { {'U', ' ', ' ', ' ', 'E', ' ', ' ', ' ', 'R', ' ', ' ',
		 ' ', 'S', ' ', ' ', ' ', 'E', ' ', ' ', ' ', 'T', ' ', ' ', 
		' '}, 0 };

#define namset ((integer *)&equiv_20)

    static struct {
	char e_1[24];
	integer e_2;
	} equiv_21 = { {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
		 ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 
		' '}, 0 };

#define nameq ((integer *)&equiv_21)

    static integer level = 4;
    static integer ieqdf = 0;
    static struct {
	char e_1[4];
	integer e_2;
	} equiv_22 = { {'=', ' ', ' ', ' '}, 0 };

#define ieq (*(integer *)&equiv_22)


    /* Format strings */
    static char fmt_35[] = "(\002 *** TERMINAL ERROR\002,10x,\002(IER = \002\
,i3,\002) FROM IMSL ROUTINE \002,6a1,a1,6a1)";
    static char fmt_40[] = "(\002 *** WARNING WITH FIX ERROR\002,2x,\002(IER\
 = \002,i3,\002) FROM IMSL ROUTINE \002,6a1,a1,6a1)";
    static char fmt_45[] = "(\002 *** WARNING ERROR\002,11x,\002(IER = \002,\
i3,\002) FROM IMSL ROUTINE \002,6a1,a1,6a1)";
    static char fmt_50[] = "(\002 *** UNDEFINED ERROR\002,9x,\002(IER = \002\
,i5,\002) FROM IMSL ROUTINE \002,6a1,a1,6a1)";

    /* Local variables */
    static integer nmtb, i__;
    static integer levold, namupk[6];
    static integer iounit, nin;

    /* Fortran I/O blocks */
    static cilist io___10 = { 0, 0, 0, fmt_35, 0 };
    static cilist io___11 = { 0, 0, 0, fmt_35, 0 };
    static cilist io___12 = { 0, 0, 0, fmt_40, 0 };
    static cilist io___13 = { 0, 0, 0, fmt_40, 0 };
    static cilist io___14 = { 0, 0, 0, fmt_45, 0 };
    static cilist io___15 = { 0, 0, 0, fmt_45, 0 };
    static cilist io___18 = { 0, 0, 0, fmt_50, 0 };
    static cilist io___19 = { 0, 0, 0, fmt_50, 0 };


/*                                  SPECIFICATIONS FOR ARGUMENTS */
/*                                  SPECIFICATIONS FOR LOCAL VARIABLES */
    /* Parameter adjustments */
    --name__;

    /* Function Body */
/*                                  UNPACK NAME INTO NAMUPK */
/*                                  FIRST EXECUTABLE STATEMENT */
    uspkd_(&name__[1], &c__6, namupk, &nmtb);
/*                                  GET OUTPUT UNIT NUMBER */
    ugetio_(&c__1, &nin, &iounit);
/*                                  CHECK IER */
    if (*ier > 999) {
	goto L25;
    }
    if (*ier < -32) {
	goto L55;
    }
    if (*ier <= 128) {
	goto L5;
    }
    if (level < 1) {
	goto L30;
    }
/*                                  PRINT TERMINAL MESSAGE */
    if (ieqdf == 1) {
	io___10.ciunit = iounit;
	s_wsfe(&io___10);
	do_fio(&c__1, (char *)&(*ier), (ftnlen)sizeof(integer));
	do_fio(&c__6, (char *)&nameq[0], (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&ieq, (ftnlen)sizeof(integer));
	do_fio(&c__6, (char *)&namupk[0], (ftnlen)sizeof(integer));
	e_wsfe();
    }
    if (ieqdf == 0) {
	io___11.ciunit = iounit;
	s_wsfe(&io___11);
	do_fio(&c__1, (char *)&(*ier), (ftnlen)sizeof(integer));
	do_fio(&c__6, (char *)&namupk[0], (ftnlen)sizeof(integer));
	e_wsfe();
    }
    goto L30;
L5:
    if (*ier <= 64) {
	goto L10;
    }
    if (level < 2) {
	goto L30;
    }
/*                                  PRINT WARNING WITH FIX MESSAGE */
    if (ieqdf == 1) {
	io___12.ciunit = iounit;
	s_wsfe(&io___12);
	do_fio(&c__1, (char *)&(*ier), (ftnlen)sizeof(integer));
	do_fio(&c__6, (char *)&nameq[0], (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&ieq, (ftnlen)sizeof(integer));
	do_fio(&c__6, (char *)&namupk[0], (ftnlen)sizeof(integer));
	e_wsfe();
    }
    if (ieqdf == 0) {
	io___13.ciunit = iounit;
	s_wsfe(&io___13);
	do_fio(&c__1, (char *)&(*ier), (ftnlen)sizeof(integer));
	do_fio(&c__6, (char *)&namupk[0], (ftnlen)sizeof(integer));
	e_wsfe();
    }
    goto L30;
L10:
    if (*ier <= 32) {
	goto L15;
    }
/*                                  PRINT WARNING MESSAGE */
    if (level < 3) {
	goto L30;
    }
    if (ieqdf == 1) {
	io___14.ciunit = iounit;
	s_wsfe(&io___14);
	do_fio(&c__1, (char *)&(*ier), (ftnlen)sizeof(integer));
	do_fio(&c__6, (char *)&nameq[0], (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&ieq, (ftnlen)sizeof(integer));
	do_fio(&c__6, (char *)&namupk[0], (ftnlen)sizeof(integer));
	e_wsfe();
    }
    if (ieqdf == 0) {
	io___15.ciunit = iounit;
	s_wsfe(&io___15);
	do_fio(&c__1, (char *)&(*ier), (ftnlen)sizeof(integer));
	do_fio(&c__6, (char *)&namupk[0], (ftnlen)sizeof(integer));
	e_wsfe();
    }
    goto L30;
L15:
/*                                  CHECK FOR UERSET CALL */
    for (i__ = 1; i__ <= 6; ++i__) {
	if (namupk[i__ - 1] != namset[i__ - 1]) {
	    goto L25;
	}
/* L20: */
    }
    levold = level;
    level = *ier;
    *ier = levold;
    if (level < 0) {
	level = 4;
    }
    if (level > 4) {
	level = 4;
    }
    goto L30;
L25:
    if (level < 4) {
	goto L30;
    }
/*                                  PRINT NON-DEFINED MESSAGE */
    if (ieqdf == 1) {
	io___18.ciunit = iounit;
	s_wsfe(&io___18);
	do_fio(&c__1, (char *)&(*ier), (ftnlen)sizeof(integer));
	do_fio(&c__6, (char *)&nameq[0], (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&ieq, (ftnlen)sizeof(integer));
	do_fio(&c__6, (char *)&namupk[0], (ftnlen)sizeof(integer));
	e_wsfe();
    }
    if (ieqdf == 0) {
	io___19.ciunit = iounit;
	s_wsfe(&io___19);
	do_fio(&c__1, (char *)&(*ier), (ftnlen)sizeof(integer));
	do_fio(&c__6, (char *)&namupk[0], (ftnlen)sizeof(integer));
	e_wsfe();
    }
L30:
    ieqdf = 0;
    return 0;

/*                                  SAVE P FOR P = R CASE */
/*                                    P IS THE PAGE NAMUPK */
/*                                    R IS THE ROUTINE NAMUPK */
L55:
    ieqdf = 1;
    for (i__ = 1; i__ <= 6; ++i__) {
/* L60: */
	nameq[i__ - 1] = namupk[i__ - 1];
    }
/* L65: */
    return 0;
} /* uertst_ */

#undef ieq
#undef nameq
#undef namset


/*   IMSL ROUTINE NAME   - USPKD */

/* ----------------------------------------------------------------------- */

/*   COMPUTER            - VAX/SINGLE */

/*   LATEST REVISION     - NOVEMBER 1, 1984 */

/*   PURPOSE             - NUCLEUS CALLED BY IMSL ROUTINES THAT HAVE */
/*                           CHARACTER STRING ARGUMENTS */

/*   USAGE               - CALL USPKD  (PACKED,NCHARS,UNPAKD,NCHMTB) */

/*   ARGUMENTS    PACKED - CHARACTER STRING TO BE UNPACKED.(INPUT) */
/*                NCHARS - LENGTH OF PACKED. (INPUT)  SEE REMARKS. */
/*                UNPAKD - INTEGER ARRAY TO RECEIVE THE UNPACKED */
/*                         REPRESENTATION OF THE STRING. (OUTPUT) */
/*                NCHMTB - NCHARS MINUS TRAILING BLANKS. (OUTPUT) */

/*   PRECISION/HARDWARE  - SINGLE/ALL */

/*   REQD. IMSL ROUTINES - NONE */

/*   REMARKS  1.  USPKD UNPACKS A CHARACTER STRING INTO AN INTEGER ARRAY */
/*                IN (A1) FORMAT. */
/*            2.  UP TO 129 CHARACTERS MAY BE USED.  ANY IN EXCESS OF */
/*                THAT ARE IGNORED. */

/*   COPYRIGHT           - 1984 BY IMSL, INC.  ALL RIGHTS RESERVED. */

/*   WARRANTY            - IMSL WARRANTS ONLY THAT IMSL TESTING HAS BEEN */
/*                           APPLIED TO THIS CODE.  NO OTHER WARRANTY, */
/*                           EXPRESSED OR IMPLIED, IS APPLICABLE. */

/* ----------------------------------------------------------------------- */
int uspkd_(integer * packed, integer * nchars, integer * unpakd, integer * nchmtb)
{
    /* Initialized data */

    static struct {
	char e_1[4];
	integer e_2;
	} equiv_9 = { {' ', ' ', ' ', ' '}, 0 };

#define iblank (*(integer *)&equiv_9)


    /* Format strings */
    static char fmt_150[] = "(4a1)";

    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__, n, dummy[4], nc, ni, nn;

    /* Fortran I/O blocks */
    static cilist io___5 = { 0, 0, 0, fmt_150, 0 };


/*                                  SPECIFICATIONS FOR ARGUMENTS */

    /* Parameter adjustments */
    --unpakd;
    --packed;

    /* Function Body */
/*                                  INITIALIZE NCHMTB */
    *nchmtb = 0;
/*                                  RETURN IF NCHARS IS LE ZERO */
    if (*nchars <= 0) {
	return 0;
    }
/*                                  SET NC=NUMBER OF CHARS TO BE DECODED 
*/
    nc = min(129,*nchars);

/* pc >>>> INTERNAL READ INSTEAD OF DECODE */
    ni = 1;
    dummy[0] = packed[ni];
L999:
    io___5.ciunit = dummy[0];
    s_rsfe(&io___5);
    i__1 = ni << 2;
    for (i__ = (ni << 2) - 3; i__ <= i__1; ++i__) {
	do_fio(&c__1, (char *)&unpakd[i__], (ftnlen)sizeof(integer));
    }
    e_rsfe();
    if (i__ <= nc) {
	++ni;
	dummy[0] = packed[ni];
	goto L999;
    }
/* ORIGINAL IMSL :      DECODE (NC,150,PACKED) (UNPAKD(I),I=1,NC) */

/*                                  CHECK UNPAKD ARRAY AND SET NCHMTB */
/*                                  BASED ON TRAILING BLANKS FOUND */
    i__1 = nc;
    for (n = 1; n <= i__1; ++n) {
	nn = nc - n + 1;
	if (unpakd[nn] != iblank) {
	    goto L210;
	}
/* L200: */
    }
    nn = 0;
L210:
    *nchmtb = nn;
    return 0;
} /* uspkd_ */

#undef iblank

/*   IMSL ROUTINE NAME   - UGETIO */

/* ----------------------------------------------------------------------- */

/*   COMPUTER            - VAX/SINGLE */

/*   LATEST REVISION     - JUNE 1, 1981 */

/*   PURPOSE             - TO RETRIEVE CURRENT VALUES AND TO SET NEW */
/*                           VALUES FOR INPUT AND OUTPUT UNIT */
/*                           IDENTIFIERS. */

/*   USAGE               - CALL UGETIO(IOPT,NIN,NOUT) */

/*   ARGUMENTS    IOPT   - OPTION PARAMETER. (INPUT) */
/*                           IF IOPT=1, THE CURRENT INPUT AND OUTPUT */
/*                           UNIT IDENTIFIER VALUES ARE RETURNED IN NIN */
/*                           AND NOUT, RESPECTIVELY. */
/*                           IF IOPT=2, THE INTERNAL VALUE OF NIN IS */
/*                           RESET FOR SUBSEQUENT USE. */
/*                           IF IOPT=3, THE INTERNAL VALUE OF NOUT IS */
/*                           RESET FOR SUBSEQUENT USE. */
/*                NIN    - INPUT UNIT IDENTIFIER. */
/*                           OUTPUT IF IOPT=1, INPUT IF IOPT=2. */
/*                NOUT   - OUTPUT UNIT IDENTIFIER. */
/*                           OUTPUT IF IOPT=1, INPUT IF IOPT=3. */

/*   PRECISION/HARDWARE  - SINGLE/ALL */

/*   REQD. IMSL ROUTINES - NONE REQUIRED */

/*   NOTATION            - INFORMATION ON SPECIAL NOTATION AND */
/*                           CONVENTIONS IS AVAILABLE IN THE MANUAL */
/*                           INTRODUCTION OR THROUGH IMSL ROUTINE UHELP */

/*   REMARKS      EACH IMSL ROUTINE THAT PERFORMS INPUT AND/OR OUTPUT */
/*                OPERATIONS CALLS UGETIO TO OBTAIN THE CURRENT UNIT */
/*                IDENTIFIER VALUES. IF UGETIO IS CALLED WITH IOPT=2 OR */
/*                IOPT=3, NEW UNIT IDENTIFIER VALUES ARE ESTABLISHED. */
/*                SUBSEQUENT INPUT/OUTPUT IS PERFORMED ON THE NEW UNITS. */

/*   COPYRIGHT           - 1978 BY IMSL, INC. ALL RIGHTS RESERVED. */

/*   WARRANTY            - IMSL WARRANTS ONLY THAT IMSL TESTING HAS BEEN */
/*                           APPLIED TO THIS CODE. NO OTHER WARRANTY, */
/*                           EXPRESSED OR IMPLIED, IS APPLICABLE. */

/* ----------------------------------------------------------------------- */

int ugetio_(integer * iopt, integer * nin, integer * nout)
{
    /* Initialized data */

    static integer nind = 5;
    static integer noutd = 6;

/*                                  SPECIFICATIONS FOR ARGUMENTS */
/*                                  SPECIFICATIONS FOR LOCAL VARIABLES */
/*                                  FIRST EXECUTABLE STATEMENT */
    if (*iopt == 3) {
	goto L10;
    }
    if (*iopt == 2) {
	goto L5;
    }
    if (*iopt != 1) {
	goto L9005;
    }
    *nin = nind;
    *nout = noutd;
    goto L9005;
L5:
    nind = *nin;
    goto L9005;
L10:
    noutd = *nout;
L9005:
    return 0;
} /* ugetio_ */

/*   IMSL ROUTINE NAME   - VBLA=SNRM2 */

/* ----------------------------------------------------------------------- */

/*   COMPUTER            - VAX/SINGLE */

/*   LATEST REVISION     - JANUARY 1, 1978 */

/*   PURPOSE             - COMPUTE THE EUCLIDEAN LENGTH OR L2 NORM */
/*                           OF A SINGLE PRECISION VECTOR */

/*   USAGE               - FUNCTION SNRM2 (N,SX,INCX) */

/*   ARGUMENTS    SNRM2  - SQUARE ROOT OF THE SUM FROM I=1 TO N OF */
/*                           X(I)**2. (OUTPUT) */
/*                           X(I) REFERS TO A SPECIFIC ELEMENT OF SX. */
/*                           SEE INCX ARGUMENT DESCRIPTION. */
/*                N      - LENGTH OF VECTOR X. (INPUT) */
/*                SX     - REAL VECTOR OF LENGTH N*INCX. (INPUT) */
/*                INCX   - DISPLACEMENT BETWEEN ELEMENTS OF SX. (INPUT) */
/*                           X(I) IS DEFINED TO BE SX(1+(I-1)*INCX). */
/*                           INCX MUST BE GREATER THAN ZERO. */

/*   PRECISION/HARDWARE  - SINGLE/ALL */

/*   REQD. IMSL ROUTINES - NONE REQUIRED */

/*   NOTATION            - INFORMATION ON SPECIAL NOTATION AND */
/*                           CONVENTIONS IS AVAILABLE IN THE MANUAL */
/*                           INTRODUCTION OR THROUGH IMSL ROUTINE UHELP */

/*   COPYRIGHT           - 1978 BY IMSL, INC. ALL RIGHTS RESERVED. */

/*   WARRANTY            - IMSL WARRANTS ONLY THAT IMSL TESTING HAS BEEN */
/*                           APPLIED TO THIS CODE. NO OTHER WARRANTY, */
/*                           EXPRESSED OR IMPLIED, IS APPLICABLE. */

/* ----------------------------------------------------------------------- */

doublereal snrm2_(integer * n, real * sx, integer * incx)
{
    /* Initialized data */

    static real zero = (float)0.;
    static real one = (float)1.;
    static real cutlo = (float)4.441e-16;
    static real cuthi = (float)1.304e19;

    /* Format strings */
    static char fmt_15[] = "";
    static char fmt_20[] = "";
    static char fmt_35[] = "";
    static char fmt_40[] = "";

    /* System generated locals */
    integer i__1, i__2;
    real ret_val, r__1;

    /* Local variables */
    static real xmax;
    static integer next, i__, j, nn;
    static real hitest, sum;

    /* Assigned format variables */
    static char *next_fmt;

/*                                  SPECIFICATIONS FOR ARGUMENTS */
/*                                  SPECIFICATIONS FOR LOCAL VARIABLES */
    /* Parameter adjustments */
    --sx;

    /* Function Body */
/*                                  FIRST EXECUTABLE STATEMENT */

    if (*n > 0) {
	goto L5;
    }
    ret_val = zero;
    goto L70;

L5:
    next = 0;
    next_fmt = fmt_15;
    sum = zero;
    nn = *n * *incx;
/*                                  BEGIN MAIN LOOP */
    i__ = 1;
L10:
    switch ((int)next) {
	case 0: goto L15;
	case 1: goto L20;
	case 2: goto L35;
	case 3: goto L40;
    }
L15:
    if ((r__1 = sx[i__], dabs(r__1)) > cutlo) {
	goto L55;
    }
    next = 1;
    next_fmt = fmt_20;
    xmax = zero;
/*                                  PHASE 1. SUM IS ZERO */
L20:
    if (sx[i__] == zero) {
	goto L65;
    }
    if ((r__1 = sx[i__], dabs(r__1)) > cutlo) {
	goto L55;
    }
/*                                  PREPARE FOR PHASE 2. */
    next = 2;
    next_fmt = fmt_35;
    goto L30;
/*                                  PREPARE FOR PHASE 4. */
L25:
    i__ = j;
    next = 3;
    next_fmt = fmt_40;
    sum = sum / sx[i__] / sx[i__];
L30:
    xmax = (r__1 = sx[i__], (real)dabs(r__1));
    goto L45;
/*                                  PHASE 2. SUM IS SMALL. SCALE TO */
/*                                    AVOID DESTRUCTIVE UNDERFLOW. */
L35:
    if ((r__1 = sx[i__], dabs(r__1)) > cutlo) {
	goto L50;
    }
/*                                  COMMON CODE FOR PHASES 2 AND 4. IN */
/*                                    PHASE 4 SUM IS LARGE. SCALE TO */
/*                                    AVOID OVERFLOW. */
L40:
    if ((r__1 = sx[i__], dabs(r__1)) <= xmax) {
	goto L45;
    }
/* Computing 2nd power */
    r__1 = xmax / sx[i__];
    sum = one + sum * (r__1 * r__1);
    xmax = (r__1 = sx[i__], (real)dabs(r__1));
    goto L65;

L45:
/* Computing 2nd power */
    r__1 = sx[i__] / xmax;
    sum += r__1 * r__1;
    goto L65;
/*                                  PREPARE FOR PHASE 3. */
L50:
    sum = sum * xmax * xmax;
/*                                  FOR REAL OR D.P. SET HITEST = */
/*                                    CUTHI/N FOR COMPLEX SET HITEST = */
/*                                    CUTHI/(2*N) */
L55:
    hitest = cuthi / (real) (*n);
/*                                  PHASE 3. SUM IS MID-RANGE. NO */
/*                                    SCALING. */
    i__1 = nn;
    i__2 = *incx;
    for (j = i__; i__2 < 0 ? j >= i__1 : j <= i__1; j += i__2) {
	if ((r__1 = sx[j], dabs(r__1)) >= hitest) {
	    goto L25;
	}
/* L60: */
/* Computing 2nd power */
	r__1 = sx[j];
	sum += r__1 * r__1;
    }
    ret_val = (real)sqrt(sum);
    goto L70;

L65:
    i__ += *incx;
    if (i__ <= nn) {
	goto L10;
    }
/*                                  END OF MAIN LOOP. COMPUTE SQUARE */
/*                                    ROOT AND ADJUST FOR SCALING. */
    ret_val = xmax * (real)sqrt(sum);
L70:
    return ret_val;
} /* snrm2_ */

/*   IMSL ROUTINE NAME   - ZSPWB */

/* ----------------------------------------------------------------------- */

/*   COMPUTER            - VAX/SINGLE */

/*   LATEST REVISION     - JUNE 1, 1982 */

/*   PURPOSE             - NUCLEUS CALLED ONLY BY IMSL SUBROUTINE ZSPOW */

/*   PRECISION/HARDWARE  - SINGLE AND DOUBLE/H32 */
/*                       - SINGLE/H36,H48,H60 */

/*   REQD. IMSL ROUTINES - NONE REQUIRED */

/*   NOTATION            - INFORMATION ON SPECIAL NOTATION AND */
/*                           CONVENTIONS IS AVAILABLE IN THE MANUAL */
/*                           INTRODUCTION OR THROUGH IMSL ROUTINE UHELP */

/*   COPYRIGHT           - 1982 BY IMSL, INC. ALL RIGHTS RESERVED. */

/*   WARRANTY            - IMSL WARRANTS ONLY THAT IMSL TESTING HAS BEEN */
/*                           APPLIED TO THIS CODE. NO OTHER WARRANTY, */
/*                           EXPRESSED OR IMPLIED, IS APPLICABLE. */

/* ----------------------------------------------------------------------- */

int zspwb_(sasfit_func_zspow_t * fcn, integer * n, real * x, real * fvec, real * fjac, 
		integer * ldfjac, integer * iflag, integer * ml, integer * mu, 
		real * epsfcn, real * wa1, real * wa2, real * par)
{
    /* Initialized data */

    static real spmpar = (float)1.1921e-7;
    static real zero = (float)0.;

    /* System generated locals */
    integer fjac_dim1, fjac_offset, i__1, i__2, i__3, i__4;
    real r__1;

    /* Local variables */
    static real temp;
    static integer msum;
    static real h__;
    static integer i__, j, k;
    static real epsmch, eps;

/*                                  SPECIFICATIONS FOR ARGUMENTS */
/*                                  SPECIFICATIONS FOR LOCAL VARIABLES */
    /* Parameter adjustments */
    --wa2;
    --wa1;
    --fvec;
    --x;
    fjac_dim1 = *ldfjac;
    fjac_offset = fjac_dim1 + 1;
    fjac -= fjac_offset;
    --par;

    /* Function Body */
/*                                  EPSMCH IS THE MACHINE PRECISION. */
/*                                  FIRST EXECUTABLE STATEMENT */
    epsmch = spmpar;
    eps = (real)sqrt((dmax(*epsfcn,epsmch)));
    msum = *ml + *mu + 1;
    if (msum < *n) {
	goto L20;
    }
/*                                  COMPUTATION OF DENSE APPROXIMATE */
/*                                  JACOBIAN. */
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	temp = x[j];
	h__ = eps * (real)dabs(temp);
	if (h__ == zero) {
	    h__ = eps;
	}
	x[j] = temp + h__;
	(*fcn)(&x[1], &wa1[1], n, &par[1]);
	if (*iflag < 0) {
	    goto L15;
	}
	x[j] = temp;
	i__2 = *n;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    fjac[i__ + j * fjac_dim1] = (wa1[i__] - fvec[i__]) / h__;
/* L5: */
	}
/* L10: */
    }
L15:
    goto L50;
L20:
/*                                  COMPUTATION OF BANDED APPROXIMATE */
/*                                  JACOBIAN. */
    i__1 = msum;
    for (k = 1; k <= i__1; ++k) {
	i__2 = *n;
	i__3 = msum;
	for (j = k; i__3 < 0 ? j >= i__2 : j <= i__2; j += i__3) {
	    wa2[j] = x[j];
	    h__ = eps * (r__1 = wa2[j], (real)dabs(r__1));
	    if (h__ == zero) {
		h__ = eps;
	    }
	    x[j] = wa2[j] + h__;
/* L25: */
	}
	(*fcn)(&x[1], &wa1[1], n, &par[1]);
	if (*iflag < 0) {
	    goto L45;
	}
	i__3 = *n;
	i__2 = msum;
	for (j = k; i__2 < 0 ? j >= i__3 : j <= i__3; j += i__2) {
	    x[j] = wa2[j];
	    h__ = eps * (r__1 = wa2[j], (real)dabs(r__1));
	    if (h__ == zero) {
		h__ = eps;
	    }
	    i__4 = *n;
	    for (i__ = 1; i__ <= i__4; ++i__) {
		fjac[i__ + j * fjac_dim1] = zero;
		if (i__ >= j - *mu && i__ <= j + *ml) {
		    fjac[i__ + j * fjac_dim1] = (wa1[i__] - fvec[i__]) / h__;
		}
/* L30: */
	    }
/* L35: */
	}
/* L40: */
    }
L45:
L50:
    return 0;
} /* zspwb_ */

/*   IMSL ROUTINE NAME   - ZSPWC */

/* ----------------------------------------------------------------------- */

/*   COMPUTER            - VAX/SINGLE */

/*   LATEST REVISION     - JUNE 1, 1982 */

/*   PURPOSE             - NUCLEUS CALLED ONLY BY IMSL SUBROUTINE ZSPOW */

/*   PRECISION/HARDWARE  - SINGLE AND DOUBLE/H32 */
/*                       - SINGLE/H36,H48,H60 */

/*   REQD. IMSL ROUTINES - SINGLE/VBLA=SNRM2 */
/*                       - DOUBLE/VBLA=DNRM2 */

/*   NOTATION            - INFORMATION ON SPECIAL NOTATION AND */
/*                           CONVENTIONS IS AVAILABLE IN THE MANUAL */
/*                           INTRODUCTION OR THROUGH IMSL ROUTINE UHELP */

/*   COPYRIGHT           - 1982 BY IMSL, INC. ALL RIGHTS RESERVED. */

/*   WARRANTY            - IMSL WARRANTS ONLY THAT IMSL TESTING HAS BEEN */
/*                           APPLIED TO THIS CODE. NO OTHER WARRANTY, */
/*                           EXPRESSED OR IMPLIED, IS APPLICABLE. */

/* ----------------------------------------------------------------------- */

int zspwc_(integer * n, real * r__, real * lr, real * diag, real * qtb, 
		real * delta, real * x, real * wa1, real * wa2)
{
    /* Initialized data */

    static real spmpar = (float)1.1921e-7;
    static real one = (float)1.;
    static real zero = (float)0.;

    /* System generated locals */
    integer i__1, i__2;
    real r__1, r__2, r__3, r__4;

    /* Local variables */
    static real temp;
    static integer i__, j, k, l;
    static real alpha, bnorm, gnorm, qnorm;
    static integer jj;
    static real epsmch, sgnorm;
    static integer jp1;
    static real sum;

/*                                  SPECIFICATIONS FOR ARGUMENTS */
/*                                  SPECIFICATIONS FOR LOCAL VARIABLES */
    /* Parameter adjustments */
    --wa2;
    --wa1;
    --x;
    --qtb;
    --diag;
    --r__;

    /* Function Body */
/*                                  EPSMCH IS THE MACHINE PRECISION. */
/*                                  FIRST EXECUTABLE STATEMENT */
    epsmch = spmpar;
/*                                  FIRST, CALCULATE THE GAUSS-NEWTON */
/*                                  DIRECTION. */
    jj = *n * (*n + 1) / 2 + 1;
    i__1 = *n;
    for (k = 1; k <= i__1; ++k) {
	j = *n - k + 1;
	jp1 = j + 1;
	jj -= k;
	l = jj + 1;
	sum = zero;
	if (*n < jp1) {
	    goto L10;
	}
	i__2 = *n;
	for (i__ = jp1; i__ <= i__2; ++i__) {
	    sum += r__[l] * x[i__];
	    ++l;
/* L5: */
	}
L10:
	temp = r__[jj];
	if (temp != zero) {
	    goto L20;
	}
	l = j;
	i__2 = j;
	for (i__ = 1; i__ <= i__2; ++i__) {
/* Computing MAX */
	    r__2 = temp, r__3 = (r__1 = r__[l], (real)dabs(r__1));
	    temp = (real)dmax(r__2,r__3);
	    l = l + *n - i__;
/* L15: */
	}
	temp = epsmch * temp;
	if (temp == zero) {
	    temp = epsmch;
	}
L20:
	x[j] = (qtb[j] - sum) / temp;
/* L25: */
    }
/*                                  TEST WHETHER THE GAUSS-NEWTON */
/*                                  DIRECTION IS ACCEPTABLE. */
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	wa1[j] = zero;
	wa2[j] = diag[j] * x[j];
/* L30: */
    }
    qnorm = (real)snrm2_(n, &wa2[1], &c__1);
    if (qnorm <= *delta) {
	goto L70;
    }
/*                                  THE GAUSS-NEWTON DIRECTION IS NOT */
/*                                  ACCEPTABLE. NEXT, CALCULATE THE */
/*                                  SCALED GRADIENT DIRECTION. */
    l = 1;
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	temp = qtb[j];
	i__2 = *n;
	for (i__ = j; i__ <= i__2; ++i__) {
	    wa1[i__] += r__[l] * temp;
	    ++l;
/* L35: */
	}
	wa1[j] /= diag[j];
/* L40: */
    }
/*                                  CALCULATE THE NORM OF THE SCALED */
/*                                  GRADIENT AND TEST FOR THE SPECIAL */
/*                                  CASE IN WHICH THE SCALED GRADIENT IS 
*/
/*                                  ZERO. */
    gnorm = (real)snrm2_(n, &wa1[1], &c__1);
    sgnorm = zero;
    alpha = *delta / qnorm;
    if (gnorm == zero) {
	goto L60;
    }
/*                                  CALCULATE THE POINT ALONG THE SCALED 
*/
/*                                  GRADIENT AT WHICH THE QUADRATIC IS */
/*                                  MINIMIZED. */
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	wa1[j] = wa1[j] / gnorm / diag[j];
/* L45: */
    }
    l = 1;
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	sum = zero;
	i__2 = *n;
	for (i__ = j; i__ <= i__2; ++i__) {
	    sum += r__[l] * wa1[i__];
	    ++l;
/* L50: */
	}
	wa2[j] = sum;
/* L55: */
    }
    temp = (real)snrm2_(n, &wa2[1], &c__1);
    sgnorm = gnorm / temp / temp;
/*                                  TEST WHETHER THE SCALED GRADIENT */
/*                                  DIRECTION IS ACCEPTABLE. */
    alpha = zero;
    if (sgnorm >= *delta) {
	goto L60;
    }
/*                                  THE SCALED GRADIENT DIRECTION IS NOT 
*/
/*                                  ACCEPTABLE. FINALLY, CALCULATE THE */
/*                                  POINT ALONG THE DOGLEG AT WHICH THE */
/*                                  QUADRATIC IS MINIMIZED. */
    bnorm = (real)snrm2_(n, &qtb[1], &c__1);
    temp = bnorm / gnorm * (bnorm / qnorm) * (sgnorm / *delta);
/* Computing 2nd power */
    r__1 = sgnorm / *delta;
/* Computing 2nd power */
    r__2 = temp - *delta / qnorm;
/* Computing 2nd power */
    r__3 = *delta / qnorm;
/* Computing 2nd power */
    r__4 = sgnorm / *delta;
    temp = temp - (real)(*delta / qnorm * (r__1 * r__1) + sqrt(r__2 * r__2 + (one - 
	    r__3 * r__3) * (one - r__4 * r__4)));
/* Computing 2nd power */
    r__1 = sgnorm / *delta;
    alpha = *delta / qnorm * (one - r__1 * r__1) / temp;
L60:
/*                                  FORM APPROPRIATE CONVEX COMBINATION */
/*                                  OF THE GAUSS-NEWTON DIRECTION AND THE 
*/
/*                                  SCALED GRADIENT DIRECTION. */
    temp = (real)((one - alpha) * dmin(sgnorm,*delta));
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	x[j] = temp * wa1[j] + alpha * x[j];
/* L65: */
    }
L70:
    return 0;
} /* zspwc_ */

/*   IMSL ROUTINE NAME   - ZSPWE */

/* ----------------------------------------------------------------------- */

/*   COMPUTER            - VAX/SINGLE */

/*   LATEST REVISION     - JUNE 1, 1982 */

/*   PURPOSE             - NUCLEUS CALLED ONLY BY IMSL SUBROUTINE ZSPOW */

/*   PRECISION/HARDWARE  - SINGLE AND DOUBLE/H32 */
/*                       - SINGLE/H36,H48,H60 */

/*   REQD. IMSL ROUTINES - NONE REQUIRED */

/*   NOTATION            - INFORMATION ON SPECIAL NOTATION AND */
/*                           CONVENTIONS IS AVAILABLE IN THE MANUAL */
/*                           INTRODUCTION OR THROUGH IMSL ROUTINE UHELP */

/*   COPYRIGHT           - 1982 BY IMSL, INC. ALL RIGHTS RESERVED. */

/*   WARRANTY            - IMSL WARRANTS ONLY THAT IMSL TESTING HAS BEEN */
/*                           APPLIED TO THIS CODE. NO OTHER WARRANTY, */
/*                           EXPRESSED OR IMPLIED, IS APPLICABLE. */

/* ----------------------------------------------------------------------- */

int zspwe_(integer * m, integer * n, real * s, integer * ls, real * u, real * v, real * w, logical * sing)
{
    /* Initialized data */

    static real giant = (float)1.7e38;
    static real one = (float)1.;
    static real p5 = (float).5;
    static real p25 = (float).25;
    static real zero = (float)0.;

    /* System generated locals */
    integer i__1, i__2;
    real r__1, r__2;

    /* Local variables */
    static real temp, temp1, temp2, temp3, temp4;
    static integer i__, j, l, jj, nm1, nmj;
    static real tau;

/*                                  SPECIFICATIONS FOR ARGUMENTS */
/*                                  SPECIFICATIONS FOR LOCAL VARIABLES */
    /* Parameter adjustments */
    --w;
    --u;
    --v;
    --s;

    /* Function Body */
/*                                  INITIALIZE THE DIAGONAL ELEMENT */
/*                                  POINTER. */
/*                                  FIRST EXECUTABLE STATEMENT */
    jj = *n * ((*m << 1) - *n + 1) / 2 - (*m - *n);
/*                                  MOVE THE NONTRIVIAL PART OF THE LAST 
*/
/*                                  COLUMN OF S INTO W. */
    l = jj;
    i__1 = *m;
    for (i__ = *n; i__ <= i__1; ++i__) {
	w[i__] = s[l];
	++l;
/* L5: */
    }
/*                                  ROTATE THE VECTOR V INTO A MULTIPLE */
/*                                  OF THE N-TH UNIT VECTOR IN SUCH A WAY 
*/
/*                                  THAT A SPIKE IS INTRODUCED INTO W. */
    nm1 = *n - 1;
    if (nm1 < 1) {
	goto L35;
    }
    i__1 = nm1;
    for (nmj = 1; nmj <= i__1; ++nmj) {
	j = *n - nmj;
	jj -= *m - j + 1;
	w[j] = zero;
	if (v[j] == zero) {
	    goto L25;
	}
/*                                  DETERMINE A GIVENS ROTATION WHICH 
*/
/*                                  ELIMINATES THE J-TH ELEMENT OF V. 
*/
	if ((r__1 = v[*n], dabs(r__1)) >= (r__2 = v[j], dabs(r__2))) {
	    goto L10;
	}
	temp2 = v[*n] / v[j];
/* Computing 2nd power */
	r__1 = temp2;
	temp3 = (real)(p5 / sqrt(p25 + p25 * (r__1 * r__1)));
	temp1 = temp3 * temp2;
	tau = one;
	if (dabs(temp1) * giant > one) {
	    tau = one / temp1;
	}
	goto L15;
L10:
	temp4 = v[j] / v[*n];
/* Computing 2nd power */
	r__1 = temp4;
	temp1 = (real)(p5 / sqrt(p25 + p25 * (r__1 * r__1)));
	temp3 = temp1 * temp4;
	tau = temp3;
L15:
/*                                  APPLY THE TRANSFORMATION TO V AND 
*/
/*                                  STORE THE INFORMATION NECESSARY TO
 */
/*                                  RECOVER THE GIVENS ROTATION. */
	v[*n] = temp3 * v[j] + temp1 * v[*n];
	v[j] = tau;
/*                                  APPLY THE TRANSFORMATION TO S AND 
*/
/*                                  EXTEND THE SPIKE IN W. */
	l = jj;
	i__2 = *m;
	for (i__ = j; i__ <= i__2; ++i__) {
	    temp = temp1 * s[l] - temp3 * w[i__];
	    w[i__] = temp3 * s[l] + temp1 * w[i__];
	    s[l] = temp;
	    ++l;
/* L20: */
	}
L25:
/* L30: */
	;
    }
L35:
/*                                  ADD THE SPIKE FROM THE RANK 1 UPDATE 
*/
/*                                  TO W. */
    i__1 = *m;
    for (i__ = 1; i__ <= i__1; ++i__) {
	w[i__] += v[*n] * u[i__];
/* L40: */
    }
/*                                  ELIMINATE THE SPIKE. */
    *sing = FALSE_;
    if (nm1 < 1) {
	goto L70;
    }
    i__1 = nm1;
    for (j = 1; j <= i__1; ++j) {
	if (w[j] == zero) {
	    goto L60;
	}
/*                                  DETERMINE A GIVENS ROTATION WHICH 
*/
/*                                  ELIMINATES THE J-TH ELEMENT OF THE
 */
/*                                  SPIKE. */
	if ((r__1 = s[jj], dabs(r__1)) >= (r__2 = w[j], dabs(r__2))) {
	    goto L45;
	}
	temp2 = s[jj] / w[j];
/* Computing 2nd power */
	r__1 = temp2;
	temp3 = (real)(p5 / sqrt(p25 + p25 * (r__1 * r__1)));
	temp1 = temp3 * temp2;
	tau = one;
	if (dabs(temp1) * giant > one) {
	    tau = one / temp1;
	}
	goto L50;
L45:
	temp4 = w[j] / s[jj];
/* Computing 2nd power */
	r__1 = temp4;
	temp1 = (real)(p5 / sqrt(p25 + p25 * (r__1 * r__1)));
	temp3 = temp1 * temp4;
	tau = temp3;
L50:
/*                                  APPLY THE TRANSFORMATION TO S AND 
*/
/*                                  REDUCE THE SPIKE IN W. */
	l = jj;
	i__2 = *m;
	for (i__ = j; i__ <= i__2; ++i__) {
	    temp = temp1 * s[l] + temp3 * w[i__];
	    w[i__] = -temp3 * s[l] + temp1 * w[i__];
	    s[l] = temp;
	    ++l;
/* L55: */
	}
/*                                  STORE THE INFORMATION NECESSARY TO
 */
/*                                  RECOVER THE GIVENS ROTATION. */
	w[j] = tau;
L60:
/*                                  TEST FOR ZERO DIAGONAL ELEMENTS IN
 */
/*                                  THE OUTPUT S. */
	if (s[jj] == zero) {
	    *sing = TRUE_;
	}
	jj += *m - j + 1;
/* L65: */
    }
L70:
/*                                  MOVE W BACK INTO THE LAST COLUMN OF */
/*                                  THE OUTPUT S. */
    l = jj;
    i__1 = *m;
    for (i__ = *n; i__ <= i__1; ++i__) {
	s[l] = w[i__];
	++l;
/* L75: */
    }
    if (s[jj] == zero) {
	*sing = TRUE_;
    }
    return 0;
} /* zspwe_ */

/*   IMSL ROUTINE NAME   - ZSPWF */

/* ----------------------------------------------------------------------- */

/*   COMPUTER            - VAX/SINGLE */

/*   LATEST REVISION     - JUNE 1, 1982 */

/*   PURPOSE             - NUCLEUS CALLED ONLY BY IMSL SUBROUTINE ZSPOW */

/*   PRECISION/HARDWARE  - SINGLE AND DOUBLE/H32 */
/*                       - SINGLE/H36,H48,H60 */

/*   REQD. IMSL ROUTINES - NONE REQUIRED */

/*   NOTATION            - INFORMATION ON SPECIAL NOTATION AND */
/*                           CONVENTIONS IS AVAILABLE IN THE MANUAL */
/*                           INTRODUCTION OR THROUGH IMSL ROUTINE UHELP */

/*   COPYRIGHT           - 1982 BY IMSL, INC. ALL RIGHTS RESERVED. */

/*   WARRANTY            - IMSL WARRANTS ONLY THAT IMSL TESTING HAS BEEN */
/*                           APPLIED TO THIS CODE. NO OTHER WARRANTY, */
/*                           EXPRESSED OR IMPLIED, IS APPLICABLE. */

/* ----------------------------------------------------------------------- */

int zspwf_(integer * m, integer * n, real * q, integer * ldq, real * wa)
{
    /* Initialized data */

    static real one = (float)1.;
    static real zero = (float)0.;

    /* System generated locals */
    integer q_dim1, q_offset, i__1, i__2, i__3;

    /* Local variables */
    static real temp;
    static integer i__, j, k, l, minmn, jm1, np1;
    static real sum;

/*                                  SPECIFICATIONS FOR ARGUMENTS */
/*                                  SPECIFICATIONS FOR LOCAL VARIABLES */
    /* Parameter adjustments */
    --wa;
    q_dim1 = *ldq;
    q_offset = q_dim1 + 1;
    q -= q_offset;

    /* Function Body */
/*                                  ZERO OUT UPPER TRIANGLE OF Q IN THE */
/*                                  FIRST MIN(M,N) COLUMNS. */
/*                                  FIRST EXECUTABLE STATEMENT */
    minmn = min(*m,*n);
    if (minmn < 2) {
	goto L15;
    }
    i__1 = minmn;
    for (j = 2; j <= i__1; ++j) {
	jm1 = j - 1;
	i__2 = jm1;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    q[i__ + j * q_dim1] = zero;
/* L5: */
	}
/* L10: */
    }
L15:
/*                                  INITIALIZE REMAINING COLUMNS TO THOSE 
*/
/*                                  OF THE IDENTITY MATRIX. */
    np1 = *n + 1;
    if (*m < np1) {
	goto L30;
    }
    i__1 = *m;
    for (j = np1; j <= i__1; ++j) {
	i__2 = *m;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    q[i__ + j * q_dim1] = zero;
/* L20: */
	}
	q[j + j * q_dim1] = one;
/* L25: */
    }
L30:
/*                                  ACCUMULATE Q FROM ITS FACTORED FORM. 
*/
    i__1 = minmn;
    for (l = 1; l <= i__1; ++l) {
	k = minmn - l + 1;
	i__2 = *m;
	for (i__ = k; i__ <= i__2; ++i__) {
	    wa[i__] = q[i__ + k * q_dim1];
	    q[i__ + k * q_dim1] = zero;
/* L35: */
	}
	q[k + k * q_dim1] = one;
	if (wa[k] == zero) {
	    goto L55;
	}
	i__2 = *m;
	for (j = k; j <= i__2; ++j) {
	    sum = zero;
	    i__3 = *m;
	    for (i__ = k; i__ <= i__3; ++i__) {
		sum += q[i__ + j * q_dim1] * wa[i__];
/* L40: */
	    }
	    temp = sum / wa[k];
	    i__3 = *m;
	    for (i__ = k; i__ <= i__3; ++i__) {
		q[i__ + j * q_dim1] -= temp * wa[i__];
/* L45: */
	    }
/* L50: */
	}
L55:
/* L60: */
	;
    }
    return 0;
} /* zspwf_ */

/*   IMSL ROUTINE NAME   - ZSPWG */

/* ----------------------------------------------------------------------- */

/*   COMPUTER            - VAX/SINGLE */

/*   LATEST REVISION     - JUNE 1, 1982 */

/*   PURPOSE             - NUCLEUS CALLED ONLY BY IMSL SUBROUTINE ZSPOW */

/*   PRECISION/HARDWARE  - SINGLE AND DOUBLE/H32 */
/*                       - SINGLE/H36,H48,H60 */

/*   REQD. IMSL ROUTINES - SINGLE/VBLA=SNRM2 */
/*                       - DOUBLE/VBLA=DNRM2 */

/*   NOTATION            - INFORMATION ON SPECIAL NOTATION AND */
/*                           CONVENTIONS IS AVAILABLE IN THE MANUAL */
/*                           INTRODUCTION OR THROUGH IMSL ROUTINE UHELP */

/*   COPYRIGHT           - 1982 BY IMSL, INC. ALL RIGHTS RESERVED. */

/*   WARRANTY            - IMSL WARRANTS ONLY THAT IMSL TESTING HAS BEEN */
/*                           APPLIED TO THIS CODE. NO OTHER WARRANTY, */
/*                           EXPRESSED OR IMPLIED, IS APPLICABLE. */

/* ----------------------------------------------------------------------- */

int zspwg_(integer * m, integer * n, real * a, integer * lda, logical * pivot, 
		integer * ipvt, integer * lipvt, real * rdiag, real * acnorm, real * wa)
{
    /* Initialized data */

    static real spmpar = (float)1.1921e-7;
    static real one = (float)1.;
    static real p05 = (float).05;
    static real zero = (float)0.;

    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2, i__3;
    real r__1, r__2, r__3;

    /* Local variables */
    static integer kmax;
    static real temp;
    static integer i__, j, k, minmn;
    static real epsmch, ajnorm;
    static integer jp1;
    static real sum;

/*                                  SPECIFICATIONS FOR ARGUMENTS */
/*                                  SPECIFICATIONS FOR LOCAL VARIABLES */
    /* Parameter adjustments */
    --wa;
    --acnorm;
    --rdiag;
    a_dim1 = *lda;
    a_offset = a_dim1 + 1;
    a -= a_offset;
    --ipvt;

    /* Function Body */
/*                                  EPSMCH IS THE MACHINE PRECISION. */
/*                                  FIRST EXECUTABLE STATEMENT */
    epsmch = spmpar;
/*                                  COMPUTE THE INITIAL COLUMN NORMS AND 
*/
/*                                  INITIALIZE SEVERAL ARRAYS. */
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	acnorm[j] = (real)snrm2_(m, &a[j * a_dim1 + 1], &c__1);
	rdiag[j] = acnorm[j];
	wa[j] = rdiag[j];
	if (*pivot) {
	    ipvt[j] = j;
	}
/* L5: */
    }
/*                                  REDUCE A TO R WITH HOUSEHOLDER */
/*                                  TRANSFORMATIONS. */
    minmn = min(*m,*n);
    i__1 = minmn;
    for (j = 1; j <= i__1; ++j) {
	if (! (*pivot)) {
	    goto L20;
	}
/*                                  BRING THE COLUMN OF LARGEST NORM I
NTO */
/*                                  THE PIVOT POSITION. */
	kmax = j;
	i__2 = *n;
	for (k = j; k <= i__2; ++k) {
	    if (rdiag[k] > rdiag[kmax]) {
		kmax = k;
	    }
/* L10: */
	}
	if (kmax == j) {
	    goto L20;
	}
	i__2 = *m;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    temp = a[i__ + j * a_dim1];
	    a[i__ + j * a_dim1] = a[i__ + kmax * a_dim1];
	    a[i__ + kmax * a_dim1] = temp;
/* L15: */
	}
	rdiag[kmax] = rdiag[j];
	wa[kmax] = wa[j];
	k = ipvt[j];
	ipvt[j] = ipvt[kmax];
	ipvt[kmax] = k;
L20:
/*                                  COMPUTE THE HOUSEHOLDER */
/*                                  TRANSFORMATION TO REDUCE THE J-TH 
*/
/*                                  COLUMN OF A TO A MULTIPLE OF THE J
-TH */
/*                                  UNIT VECTOR. */
	i__2 = *m - j + 1;
	ajnorm = (real)snrm2_(&i__2, &a[j + j * a_dim1], &c__1);
	if (ajnorm == zero) {
	    goto L50;
	}
	if (a[j + j * a_dim1] < zero) {
	    ajnorm = -ajnorm;
	}
	i__2 = *m;
	for (i__ = j; i__ <= i__2; ++i__) {
	    a[i__ + j * a_dim1] /= ajnorm;
/* L25: */
	}
	a[j + j * a_dim1] += one;
/*                                  APPLY THE TRANSFORMATION TO THE */
/*                                  REMAINING COLUMNS AND UPDATE THE 
*/
/*                                  NORMS. */
	jp1 = j + 1;
	if (*n < jp1) {
	    goto L50;
	}
	i__2 = *n;
	for (k = jp1; k <= i__2; ++k) {
	    sum = zero;
	    i__3 = *m;
	    for (i__ = j; i__ <= i__3; ++i__) {
		sum += a[i__ + j * a_dim1] * a[i__ + k * a_dim1];
/* L30: */
	    }
	    temp = sum / a[j + j * a_dim1];
	    i__3 = *m;
	    for (i__ = j; i__ <= i__3; ++i__) {
		a[i__ + k * a_dim1] -= temp * a[i__ + j * a_dim1];
/* L35: */
	    }
	    if (! (*pivot) || rdiag[k] == zero) {
		goto L40;
	    }
	    temp = a[j + k * a_dim1] / rdiag[k];
/* Computing MAX */
/* Computing 2nd power */
	    r__3 = temp;
	    r__1 = zero, r__2 = one - r__3 * r__3;
	    rdiag[k] *= (real)sqrt((dmax(r__1,r__2)));
/* Computing 2nd power */
	    r__1 = rdiag[k] / wa[k];
	    if (p05 * (r__1 * r__1) > epsmch) {
		goto L40;
	    }
	    i__3 = *m - j;
	    rdiag[k] = (real)snrm2_(&i__3, &a[jp1 + k * a_dim1], &c__1);
	    wa[k] = rdiag[k];
L40:
/* L45: */
	    ;
	}
L50:
	rdiag[j] = -ajnorm;
/* L55: */
    }
    return 0;
} /* zspwg_ */

/*   IMSL ROUTINE NAME   - ZSPWD */

/* ----------------------------------------------------------------------- */

/*   COMPUTER            - VAX/SINGLE */

/*   LATEST REVISION     - JUNE 1, 1982 */

/*   PURPOSE             - NUCLEUS CALLED ONLY BY IMSL SUBROUTINE ZSPOW */

/*   PRECISION/HARDWARE  - SINGLE AND DOUBLE/H32 */
/*                       - SINGLE/H36,H48,H60 */

/*   REQD. IMSL ROUTINES - NONE REQUIRED */

/*   NOTATION            - INFORMATION ON SPECIAL NOTATION AND */
/*                           CONVENTIONS IS AVAILABLE IN THE MANUAL */
/*                           INTRODUCTION OR THROUGH IMSL ROUTINE UHELP */

/*   COPYRIGHT           - 1982 BY IMSL, INC. ALL RIGHTS RESERVED. */

/*   WARRANTY            - IMSL WARRANTS ONLY THAT IMSL TESTING HAS BEEN */
/*                           APPLIED TO THIS CODE. NO OTHER WARRANTY, */
/*                           EXPRESSED OR IMPLIED, IS APPLICABLE. */

/* ----------------------------------------------------------------------- */

int zspwd_(integer * m, integer * n, real * a, integer * lda, real * v, real * w)
{
    /* Initialized data */

    static real one = (float)1.;

    /* System generated locals */
    integer a_dim1, a_offset, i__1, i__2;
    real r__1, r__2;

    /* Local variables */
    static real temp, temp1, temp2;
    static integer i__, j, nm1, nmj;

/*                                  SPECIFICATIONS FOR ARGUMENTS */
/*                                  SPECIFICATIONS FOR LOCAL VARIABLES */
    /* Parameter adjustments */
    --w;
    --v;
    a_dim1 = *lda;
    a_offset = a_dim1 + 1;
    a -= a_offset;

    /* Function Body */
/*                                  APPLY THE FIRST SET OF GIVENS */
/*                                  ROTATIONS TO A. */
/*                                  FIRST EXECUTABLE STATEMENT */
    nm1 = *n - 1;
    if (nm1 < 1) {
	goto L25;
    }
    i__1 = nm1;
    for (nmj = 1; nmj <= i__1; ++nmj) {
	j = *n - nmj;
	if ((r__1 = v[j], dabs(r__1)) > one) {
	    temp1 = one / v[j];
	}
	if ((r__1 = v[j], dabs(r__1)) > one) {
/* Computing 2nd power */
	    r__2 = temp1;
	    temp2 = (real)sqrt(one - r__2 * r__2);
	}
	if ((r__1 = v[j], dabs(r__1)) <= one) {
	    temp2 = v[j];
	}
	if ((r__1 = v[j], dabs(r__1)) <= one) {
/* Computing 2nd power */
	    r__2 = temp2;
	    temp1 = (real)sqrt(one - r__2 * r__2);
	}
	i__2 = *m;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    temp = temp1 * a[i__ + j * a_dim1] - temp2 * a[i__ + *n * a_dim1];
	    a[i__ + *n * a_dim1] = temp2 * a[i__ + j * a_dim1] + temp1 * a[
		    i__ + *n * a_dim1];
	    a[i__ + j * a_dim1] = temp;
/* L5: */
	}
/* L10: */
    }
/*                                  APPLY THE SECOND SET OF GIVENS */
/*                                  ROTATIONS TO A. */
    i__1 = nm1;
    for (j = 1; j <= i__1; ++j) {
	if ((r__1 = w[j], dabs(r__1)) > one) {
	    temp1 = one / w[j];
	}
	if ((r__1 = w[j], dabs(r__1)) > one) {
/* Computing 2nd power */
	    r__2 = temp1;
	    temp2 = (real)sqrt(one - r__2 * r__2);
	}
	if ((r__1 = w[j], dabs(r__1)) <= one) {
	    temp2 = w[j];
	}
	if ((r__1 = w[j], dabs(r__1)) <= one) {
/* Computing 2nd power */
	    r__2 = temp2;
	    temp1 = (real)sqrt(one - r__2 * r__2);
	}
	i__2 = *m;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    temp = temp1 * a[i__ + j * a_dim1] + temp2 * a[i__ + *n * a_dim1];
	    a[i__ + *n * a_dim1] = -temp2 * a[i__ + j * a_dim1] + temp1 * a[
		    i__ + *n * a_dim1];
	    a[i__ + j * a_dim1] = temp;
/* L15: */
	}
/* L20: */
    }
L25:
    return 0;
} /* zspwd_ */

