/*
 * src/sasfit_common/f2c_utils/sasfit_robertus_utils.c
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
#include <gsl/gsl_sf.h>
#include <gsl/gsl_multiroots.h>
#include "../include/sasfit_utils_f2c.h"

// functions in this file
integer inx_(integer * i__, integer * j);
int fcn_(real * xla, real * f, integer * n, real * par);
doublereal fksi_(integer * n);
int schultz_(doublereal * x0, doublereal * rmean, doublereal * sig);
int setconst_(logical * labdas);
int set_tau__();
doublereal sch_(doublereal * r__, doublereal * fac, doublereal * rm, doublereal * z__);
int lambda_mn_f(const gsl_vector * x, void * params, gsl_vector * f);

doublereal fshell_(doublereal * delrh1, doublereal * delrh2, doublereal * r__, doublereal * d__, doublereal * q);
doublereal flinsphere_(doublereal * delrho, doublereal * r__, doublereal * d__, doublereal * q);
doublereal form_factor__(integer * iform, 
			doublereal * q_vector__, 
			doublereal * radius, 
			integer * nrho, 
			doublereal * rho, 
			integer * nlayer, 
			doublereal * dlayer);
doublereal fsphere_(doublereal * delrho, doublereal * r__, doublereal * q);
doublereal agetval_(doublereal * bl, doublereal * bh);
integer igetval_(integer * il, integer * ih);
int qik_(doublereal * q, doublecomplex * qq);
int struct_(doublereal * q, doublecomplex * s, integer * ifail);
int d_leqt1c__(doublecomplex * a, integer * n, integer * ia, doublecomplex * b, integer * m, integer * ib, integer * ijob, doublereal * wa, integer * ier);


/* Table of constant values */

static logical c_true = TRUE_;
static integer c__1 = 1;
static integer c__2 = 2;
static integer c__3 = 3;
static integer c__5 = 5;
static doublereal c_b85 = 30.75;

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int sasfit_robertus_calc(sasfit_param * param, doublereal * rm, doublereal * sig, doublereal * phi)
{
	/* Format strings */
	//static char fmt_1000[] = "(\002 No real solution : \002,e10.2)";

	/* System generated locals */
	integer i__1;

	/* Local variables */
	static integer nsig, i__, n;
	static real fnorm;
	static integer itmax;
//maximal value for xa array length: Lxal = p*(p+1)/2
#define LXAL 120
//maximal value for wk array length: Lwk = Lxal*(3*Lxal+15)/2
#define LWK 22500

	static real wk[LWK];
	static integer ier;
	static real par[1], xla[LXAL];

    /* Fortran I/O blocks */
//    static cilist io___10 = { 0, 6, 0, fmt_1000, 0 };

	const gsl_multiroot_fsolver_type *gsl_T;
	gsl_multiroot_fsolver *gsl_s;

	int gsl_status;
	size_t gsl_i, gsl_iter = 0;
	gsl_multiroot_function gsl_f;
	gsl_vector *gsl_x;
	size_t gsl_n;

	SASFIT_ASSERT_VAL(param, -1);

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
++*/
/* PURPOSE: calculate labda(i,j) given: */
/*                  idistr, p, rm, sig, tau(i,j), phi */

/* ROUTINES CALLED: */
/*     deal, ZSPOW */

/* FUNCTIONS CALLED : none */


/* GLOBAL variables : */

/* SUBROUTINE variables : */

/* LOCAL variables : */

/* ZSPOW variables : */
/*=======================================================================
==*/
    n = _BLNK__1.p * (_BLNK__1.p + 1) / 2;
    sasfit_robertus_deal(rm, sig, phi, &c_true);
    nsig = 5;
    itmax = 15000;
    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
 	    xla[i__ - 1] = (real) ve_1.x[i__ - 1];
/* L2: */
    }
    zspow_(fcn_, &nsig, &n, &itmax, par, xla, &fnorm, wk, &ier);
    ltrap_1.f_value__ = (doublereal) fnorm;
    i__1 = n;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* L3: */
	ve_1.x[i__ - 1] = (doublereal) xla[i__ - 1];
    }

	if ((ier>0) || (ltrap_1.f_value__ > 1e-4)) {

		gsl_n = n;
		gsl_f.f = &lambda_mn_f;
		gsl_f.n = gsl_n;
		gsl_f.params = par;

		gsl_x = gsl_vector_alloc (gsl_n);
		for (gsl_i = 0; gsl_i < gsl_n; ++gsl_i) {
			gsl_vector_set (gsl_x, gsl_i, xla[gsl_i]);
		}

//		gsl_T = gsl_multiroot_fsolver_hybrids;
		gsl_T = gsl_multiroot_fsolver_dnewton;
		gsl_s = gsl_multiroot_fsolver_alloc (gsl_T, gsl_n);
		gsl_multiroot_fsolver_set (gsl_s, &gsl_f, gsl_x);

		do
		{
			gsl_iter++;
			gsl_status = gsl_multiroot_fsolver_iterate (gsl_s);
			if (gsl_status)   {/* check if solver is stuck */
				if (gsl_status == GSL_EBADFUNC ) 
				{
					sasfit_param_set_err(param, DBGINFO("the iteration encountered a singular point where the function or its derivative evaluated to Inf or NaN."));
				}
				if (gsl_status == GSL_ENOPROG ) 
				{
					sasfit_param_set_err(param, DBGINFO("the iteration is not making any progress, preventing the algorithm from continuing."));
				}
				break;
			}
			gsl_status = gsl_multiroot_test_residual (gsl_s->f, 1e-4);
		}
		while (gsl_status == GSL_CONTINUE && gsl_iter < 500);

/*
		if ((gsl_status == GSL_ENOPROG) || (gsl_status == GSL_EBADFUNC) || (gsl_iter >= 1000)) 
		{
			 if (gsl_iter >= 1000) 
			 {
				 sasfit_param_set_err(param, DBGINFO("solution for nonlinear equation (using gsl multi-root finder) in the initialization routine calc is poor (sum_i |f_i| > 1e-4)"));
			 }
		}
*/
		for (i__ = 1; i__ <= i__1; ++i__) 
		{
			ve_1.x[i__ - 1] = (doublereal) gsl_vector_get(gsl_x,i__ - 1);
		}
		gsl_multiroot_fsolver_free (gsl_s);
		gsl_vector_free (gsl_x);
		return 0;
	}
	return 0;
} 

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int sasfit_robertus_deal(doublereal *rm, doublereal *sig, doublereal *phi, logical *labdas)
{
    /* System generated locals */
    doublereal d__1, d__2;

    /* Local variables */
    static doublereal x0, sr;

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
++*/
/* PURPOSE: deals with frequent initialisations. */

/* CALLED BY: calc */

/* ROUTINES CALLED: */
/*     schultz, setconst */


/* GLOBAL variables : */

/* SUBROUTINE variables : */

/* LOCAL variables : */
/* FUNCTION calls */
/*=======================================================================
==*/
/* x0..... */
/* Computing 2nd power */
    d__1 = *sig;
    sr = d__1 * d__1;
    if (_BLNK__1.idistr == 1) {
/* Computing 3rd power */
	d__1 = *rm, d__2 = d__1;
	x0 = *phi * 6. / _BLNK__1.pi / (d__2 * (d__1 * d__1)) / (sr * 2. + 1.)
		 / (sr + 1.);
    } else if (_BLNK__1.idistr == 3) {
/* Computing 3rd power */
	d__1 = *rm, d__2 = d__1;
	x0 = *phi * 6. / _BLNK__1.pi / (d__2 * (d__1 * d__1)) / (sr * 6. + 1.)
		;
    }
    schultz_(&x0, rm, sig);
    setconst_(labdas);
    return 0;
}


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int set_tau__()
{
    /* System generated locals */
    integer i__1, i__2;

    /* Builtin functions */
//    double atan();

    /* Local variables */
    static integer i__, j;
    static doublereal rm, xi;

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
++*/
/* PURPOSE: Set values for the stickyness parameter tau. This may be done 
*/
/*          two ways. */
/*       1: through curious function relating the particle diameters to a 
*/
/*          stickyness */
/*   or  2: by reading data from a file. (Code is COMMENTED away) */

/* CALLED BY: main, setconst */

/* ROUTINES CALLED: errmsg */


/* GLOBAL variables : */

/* LOCAL variables : */

/* c     character*80  fln, filename */
/* c     integer jchar, ios, iopt */

/* FUNCTION calls : */
/* c    integer igets */
/*=======================================================================
==*/
/* calculate average diameter */
    rm = 0.;
    xi = 0.;
    i__1 = _BLNK__1.p;
    for (i__ = 1; i__ <= i__1; ++i__) {
	rm += pa_1.x[i__ - 1] * pa_1.r__[i__ - 1];
	xi += pa_1.x[i__ - 1];
/* L10: */
    }
    rm /= xi;


/* c100   iopt = 0 */
/* c      write (6,1000) atau,btau */
/* c      read( 5, fmt='(i1)',end=101,err=100) iopt */
/* c101   if (iopt.eq.0) iopt=1 */
/* c      if ( iopt .eq. 1 ) then */
/* c iopt = 1 tau data calculated */
/* c */
    i__1 = _BLNK__1.p;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* L20: */
	_BLNK__1.tau[inx_(&i__, &i__) - 1] = _BLNK__1.btau * (2. / 
		_BLNK__1.pi * atan(_BLNK__1.atau * (pa_1.r__[i__ - 1] - rm)) 
		+ 1.);
    }
    i__1 = _BLNK__1.p;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = _BLNK__1.p;
	for (j = 1; j <= i__2; ++j) {
	    _BLNK__1.tau[inx_(&i__, &j) - 1] = (_BLNK__1.tau[inx_(&i__, &i__) 
		    - 1] + _BLNK__1.tau[inx_(&j, &j) - 1]) * .5;
/* L40: */
	}
/* L30: */
    }
/* c */
/* c iopt = 2 tau data from file ! */
/* c      else */
/* %%%%% */
/* c 50      filename = ' ' */
/* c         write(6,1020) */
/* c         if (igets(5,filename).eq.0) goto 100 */
/* c         if (filename.eq.' ') filename='tau.dat' */
/* c         jchar    = index(filename,' ') - 1 */
/* c         fln(1:jchar) = filename(1:jchar) */
/* c         open(unit=1,file=fln(1:jchar),blocksize=2048,recl=32766, */
/* c     -        form='formatted',status='old',iostat=ios) */
/* c         if (ios.ne.0) then */
/* c             call errmsg ('Error: File does not exist') */
/* c             goto 50 */
/* c         else */
/* c             write(6,1010) fln(1:jchar) */
/* c             read(unit=1,fmt=1040) p */
/* c             read (unit=1,fmt=*,err=999) ((tau(inx(i,j)),i=1,p),j=1,p)
 */
/* c         do 20 i=1,p */
/* c            do 21 j=1,p */
/* c               read(unit=1,fmt=*,err=999) tau(inx(i,j)) */
/* c 21         continue */
/* c 20      continue */
/* c             close(unit=1) */
/* c         endif */
/* c      endif */
    return 0;
/* 999   call errmsg(' Error read tau-data ') */
/*      close (unit=1) */
/*      goto 50 */
/* L1000: */
/* L1010: */
/* L1020: */
/* L1040: */
} 

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int setconst_(logical * labdas)
{
    /* System generated locals */
    integer i__1, i__2;
    doublereal d__1;

    /* Builtin functions */
    //double sqrt();

    /* Local variables */
    static integer i__, j;
    static doublereal b0, bla, dla, som;
    static doublereal alab;

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
++*/
/* PURPOSE: calculate constants belong to a set of x(i) and r(i) */
/*          results in common blocks  /fu/, /ri/, /ve/, /be/ */

/* CALLED BY: deal */

/* ROUTINES CALLED  : set_tau */

/* FUNCTIONS CALLED : fksi, inx */


/* GLOBAL variables : */


/* LOCAL variables  : */
/* FUNCTION calls */
/*=======================================================================
==*/
    fu_1.k2 = fksi_(&c__2);
    fu_1.k3 = fksi_(&c__3);
    fu_1.h__ = 1. - fu_1.k3;
/* Computing 2nd power */
    d__1 = fu_1.h__;
    fu_1.h2 = d__1 * d__1;
    fu_1.pi12 = _BLNK__1.pi / 12.;
    fu_1.pi6 = _BLNK__1.pi / 6.;
    i__1 = _BLNK__1.p;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = _BLNK__1.p;
	for (j = 1; j <= i__2; ++j) {
	    ri_1.r1[i__ + j * 20 - 21] = (pa_1.r__[i__ - 1] + pa_1.r__[j - 1])
		     * .5;
/* Computing 2nd power */
	    d__1 = ri_1.r1[i__ + j * 20 - 21];
	    ri_1.r2[i__ + j * 20 - 21] = d__1 * d__1;
	    ri_1.rr[i__ + j * 20 - 21] = pa_1.r__[i__ - 1] * pa_1.r__[j - 1];
/* L100: */
	}
/* L200: */
    }

/* r(i)'s are calcultated now calculate tau-array */

    set_tau__();
    if (*labdas) {
	bla = fu_1.k3 / fu_1.h__ + _BLNK__1.tau[inx_(&c__1, &c__1) - 1];
/* Computing 2nd power */
	d__1 = bla;
	dla = d__1 * d__1 - fu_1.k3 / 3. * (fu_1.k3 * .5 + 1.) / fu_1.h2;
	if (dla >= 0.) {
	    alab = 6. / fu_1.k3 * (bla - sqrt(dla));
	} else {
	    alab = 6. / fu_1.k3 * bla;
	}
	for (i__ = 1; i__ <= 200; ++i__) {
	    ve_2.la[i__ - 1] = alab;
/* L300: */
	}
    }

    i__1 = _BLNK__1.p;
    for (i__ = 1; i__ <= i__1; ++i__) {
	som = 0.;
	i__2 = _BLNK__1.p;
	for (j = 1; j <= i__2; ++j) {
	    som += pa_1.x[j - 1] * ve_2.la[inx_(&i__, &j) - 1] * ri_1.r2[i__ 
		    + j * 20 - 21] * pa_1.r__[j - 1];
/* L350: */
	}
	b0 = pa_1.r__[i__ - 1] * 3. * fu_1.k2 / fu_1.h2 - fu_1.pi6 * som / 
		fu_1.h__;
	be_1.b[i__ - 1] = b0 * -.5 * pa_1.r__[i__ - 1];
/* L400: */
    }
    return 0;
} 

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int schultz_(doublereal * x0, doublereal * rmean, doublereal * sig)
{
    /* System generated locals */
    integer i__1;
    doublereal d__1, d__2, d__3, d__4;

    /* Local variables */
    static integer i__;
    static doublereal r__, z__, dr;
    static doublereal rm, zp, fac;

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
++*/
/* PURPOSE : */
/*     The number densities of particles with diameter r(i) are given */
/*     by the size distribution (determined by 'idistr') */
/* -->    result calculation passed to x(i) and r(i) in /pa/ */

/* !!! Normal distribution not yet implemented !!! */
/* idistr : */
/*  1  Schulz */
/*  2  Normal */
/*              The diameter run between plus */
/*               and minus three times sigma of the mean. */
/*  3  Bimodal */

/* CALLED BY : deal */

/* ROUTINES CALLED : none */

/* INPUT */
/* ------- */
/* x0    : normalisation ie  phi = ksi(3). */
/* rmean : mean of the distribution. */
/* sig   : relative variance of the distribution. */


/* GLOBAL variables : */
/*        r1(i) is everywhere else r(i) */

/* SUBROUTINE variables : */

/* LOCAL variables : */

/* FUNCTION calls : */
/* dgamma : library gamma-function */
/* sch    : schulz distribution function */
/*=======================================================================
==*/
    rm = *rmean;

/* pathological cases p=1 or sig = 0 */
    if (_BLNK__1.p == 1 || *sig == (float)0.) {
	i__1 = _BLNK__1.p;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    pa_2.r1[i__ - 1] = rm;
	    pa_2.x[i__ - 1] = *x0 / (doublereal) _BLNK__1.p;
/* L11: */
	}
	return 0;
    }

/* more interesting cases p > 1 , sig <> 0 */

    if (_BLNK__1.idistr == 1) {
/* SCHULZ option */
/* normalisation of Schulz distributionfunction */

/*                        (Z+1) */
/*                   (Z+1) */
/*     fac =     -------------------- */
/*             Gamma(Z+1) * rm * exp(Z+1) */

/* Computing 2nd power */
	d__1 = 1. / *sig;
	z__ = d__1 * d__1 - 1.;
	zp = z__ + 1.;
	if (*sig < .25) {
/* Computing 2nd power */
	    d__1 = zp;
/* Computing 3rd power */
	    d__2 = zp, d__3 = d__2;
/* Computing 4th power */
	    d__4 = zp, d__4 *= d__4;
	    fac = sqrt(zp / 2. / _BLNK__1.pi) / rm / (.083333333333333329 / 
		    zp + 1. + .003472222222222222 / (d__1 * d__1) - 
		    .0026813271604938273 / (d__3 * (d__2 * d__2)) - 
		    2.2947209362139917e-4 / (d__4 * d__4));
/* approximation of Gamma function for large arguments */
/*        Zie Handbook of Chemistry and Physics, Gamma function */
	} else {

/* exact expression for Gamma function for small arguments */
	    fac = exp(zp * log(zp) - zp) / rm / gsl_sf_gamma(zp);
	}

/* !!! for large sigma distribution is skewed ---> */

	dr = *sig * 6. * rm / (doublereal) (_BLNK__1.p - 1);
	r__ = rm - *sig * 3. * rm;
	if (r__ < 0.) {
	    r__ = dr / 2.;
	}
	i__1 = _BLNK__1.p;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    pa_2.r1[i__ - 1] = r__;
	    pa_2.x[i__ - 1] = *x0 * sch_(&r__, &fac, &rm, &z__) * dr;
	    r__ += dr;
/* L200: */
	}
	return 0;
/* end of SCHULZ option */
    }

/*      if (idistr.eq.2) then */
/* NORMAL distribution */
/* end of NORMAL distribution */
/*      return */
/*      endif */

    if (_BLNK__1.idistr == 3) {
/* BIMODAL option */
	pa_2.x[0] = *x0 * .5;
	pa_2.x[1] = pa_2.x[0];
	pa_2.r1[0] = rm - rm * *sig;
	pa_2.r1[1] = rm + rm * *sig;
/* end of BIMODAL option */
	return 0;
    }
    return 0; // what should the function return here ?
} 

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
doublereal sch_(doublereal *r__, doublereal *fac, doublereal *rm, doublereal *z__)
{
    /* System generated locals */
    doublereal ret_val;

    /* Builtin functions */
//    double log(), exp();

    /* Local variables */
    static doublereal rf;

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
++*/
/* PURPOSE : */
/*     Calculate given fac, rm & z the */
/*     value of the schultz distribution function f(x) at x = r */

/* CALLED BY :  schultz */


/* SUBROUTINE variables : */

/* LOCAL variables  : */
/*=======================================================================
==*/
    rf = *r__ / *rm;
    ret_val = *fac * exp(*z__ * log(rf) + (*z__ + 1.) * (1. - rf));
    return ret_val;
} 

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
doublereal fksi_(n)
integer *n;
{
    /* System generated locals */
    integer i__1;
    doublereal ret_val;

    /* Builtin functions */
//    double pow_di();

    /* Local variables */
    static integer i__;

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
++*/
/* PURPOSE: Calculate worldfamous ksi2 and ksi3 from x(i) and r(i), i=1..p
 */
/*     (See: Vrij 1979 (17) ) */

/* CALLED BY: setconst */

/* ROUTINES CALLED: none */


/* GLOBAL variables : */

/* SUBROUTINE variables : */

/* LOCAL variables : */
/*=======================================================================
==*/
    ret_val = 0.;
    i__1 = _BLNK__1.p;
    for (i__ = 1; i__ <= i__1; ++i__) {
/* L100: */
	ret_val += pa_1.x[i__ - 1] * pow_di(&pa_1.r__[i__ - 1], n);
    }
    ret_val = ret_val * _BLNK__1.pi / 6.;
    return ret_val;
} 

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int fcn_(real *xla, real *f, integer *n, real *par)
{
    /* System generated locals */
    integer i__1, i__2, i__3;

    /* Local variables */
    static doublereal a[20];
    static integer i__, j, k;
    static doublereal b0;
    static integer iq;
    static doublereal xc[20], som;

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
++*/
/* PURPOSE : */
/*    This function is the obedient victim of the IMSL routine ZSPOW */
/*    It is a sad compromise between readability and speed. */
/*    In f(i) the Perram&Smith 1975 (14) equations are held. */

/* CALLED BY : */
/*    ZSPOW ( in calc ) */

/* ROUTINES CALLED :  none */

/* FUNCTIONS CALLED : inx */

/* GLOBAL variables : */

/* SUBROUTINE variables : */
/*      real    aux(15) */

/* LOCAL variables  : */
/* FUNCTION calls */
/*=======================================================================
==*/
/*     In the following loop the coefficients as used in Perram&Smith */
/*     1975 (9) are calculated. The b(i)'s are put in a common block */
/*     as these are used later for calculation of the structure factor. */

    /* Parameter adjustments */
    --par;
    --f;
    --xla;

    /* Function Body */
    i__1 = _BLNK__1.p;
    for (i__ = 1; i__ <= i__1; ++i__) {
	som = 0.;
	i__2 = _BLNK__1.p;
	for (j = 1; j <= i__2; ++j) {
	    som += pa_1.x[j - 1] * (doublereal) xla[inx_(&i__, &j)] * ri_1.r2[
		    i__ + j * 20 - 21] * pa_1.r__[j - 1];
/* L100: */
	}
	xc[i__ - 1] = som * fu_1.pi6;
	b0 = pa_1.r__[i__ - 1] * 3. * fu_1.k2 / fu_1.h2 - xc[i__ - 1] / 
		fu_1.h__;
	a[i__ - 1] = 1. / fu_1.h__ + b0;
	be_1.b[i__ - 1] = b0 * -.5 * pa_1.r__[i__ - 1];
/* L200: */
    }
/* ------ */

/*      write(6,1015) */
/* 1015 format('$Concentrations : ') */
/*      write(6,1016) (x(i),i=1,5) */
/* 1016 format(' ',5e14.7) */
/*      write(6,1018) */
/* 1018 format('$Radii          : ') */
/*      write(6,1019) (r(i),i=1,5) */
/* 1019 format(' ',5e14.7) */
    iq = 0;
    i__1 = _BLNK__1.p;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = i__;
	for (j = 1; j <= i__2; ++j) {
	    som = 0.;
	    i__3 = _BLNK__1.p;
	    for (k = 1; k <= i__3; ++k) {
		som += pa_1.x[k - 1] * (doublereal) xla[inx_(&j, &k)] * 
			ri_1.r2[j + k * 20 - 21] / ri_1.r1[i__ + j * 20 - 21] 
			* (a[i__ - 1] * -.5 * ri_1.rr[i__ + k * 20 - 21] - 
			be_1.b[i__ - 1] * pa_1.r__[k - 1] + (doublereal) xla[
			inx_(&i__, &k)] * ri_1.r2[i__ + k * 20 - 21] / 12.);
/* L300: */
	    }
	    ++iq;
	    f[iq] = (real) (som * fu_1.pi6 + be_1.b[i__ - 1] / ri_1.r1[i__ + 
		    j * 20 - 21] + a[i__ - 1] - _BLNK__1.tau[inx_(&i__, &j) - 
		    1] * (doublereal) xla[inx_(&i__, &j)]);
/* L400: */
	}
/* L500: */
    }
/*      write(6,1004) n */
/*     do 15 ih=1,5 */
/*        do 14 jh=1,5 */
/* 14          aux(jh)=xla(inx(ih,jh)) */
/*             write (6,1013) (aux(jh),jh=1,5) */
/* 1013             format (' ',5e14.6) */
/* 15  continue */
/*      write (6,1003) */
/*      write (6,1002) (f(i),i=1,15) */
/* 1002 format(' ',5e14.7,/,' ',5e14.7,/,' ',5e14.7) */
/* 1003 format('$Equations : ') */
/* 1004 format('$labdas : ',i4) */
    return 0;
} 

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
integer inx_(integer * i__, integer * j)
{
    /* System generated locals */
    integer ret_val;

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
++*/
/*     Because ZSPOW insists an array of variables while labda really is 
*/
/*     a matrix inx simulates a pxp-matrix. */
/*     The array v contains offsets - symmetry in maintained. */
/*     E.g. v1=0, v2=1, v3=3 etc. (Zie SUBROUTINE setconst) */


/* GLOBAL variables : */

/* SUBROUTINE variables : */
/*=======================================================================
==*/
    if (*i__ < *j) {
	goto L100;
    }
    ret_val = ve_2.v[*i__ - 1] + *j;
    return ret_val;
L100:
    ret_val = ve_2.v[*j - 1] + *i__;
    return ret_val;
} /* inx_ */

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int sasfit_robertus_show_tau()
{
    static char fmt_1003[] = "(9(1x,f6.2))";

    integer i__1, i__2;


    static integer ih, jh;
    static doublereal aux[9];

    static cilist io___51 = { 0, 6, 0, fmt_1003, 0 };


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
++*/
/* PURPOSE : display current values of tau(i,j) */

/* CALLED BY: main */

/* ROUTINES CALLED : none */


/* GLOBAL variables : */

/* LOCAL variables : */
/* FUNCTION calls */
/*=======================================================================
==*/
    i__1 = _BLNK__1.p;
    for (ih = 1; ih <= i__1; ++ih) {
	i__2 = _BLNK__1.p;
	for (jh = 1; jh <= i__2; ++jh) {
/* L14: */
	    aux[jh - 1] = _BLNK__1.tau[inx_(&ih, &jh) - 1];
	}
	s_wsfe(&io___51);
	i__2 = _BLNK__1.p;
	for (jh = 1; jh <= i__2; ++jh) {
	    do_fio(&c__1, (char *)&aux[jh - 1], (ftnlen)sizeof(doublereal));
	}
	e_wsfe();
/* L15: */
    }
    return 0; // what to we return here ?
}


int lambda_mn_f(const gsl_vector * x, void * params, gsl_vector * f)
{
  real xal[120];
  real FCNres[120];
  integer n;
  int i; 

  
  n = x->size;

  for (i=0;i<n;i++) {
	  xal[i] = (real) gsl_vector_get(x,i); 
  }
  fcn_(xal,FCNres,&n,params);
  for (i=0;i<n;i++) {
	  gsl_vector_set(f,i, (double) FCNres[i]); 
  }
  return GSL_SUCCESS;
}


int struct_(doublereal * q, doublecomplex * s, integer * ifail)
{
    /* System generated locals */
    integer i__1, i__2, i__3, i__4;
    doublecomplex z__1, z__2, z__3;

    /* Builtin functions */
    void d_cnjg();

    /* Local variables */
    static integer ijob;
    static doublecomplex a[400]	/* was [20][20] */, b[400]	/* was [20][
	    20] */;
    static integer i__, j, k, m, n, ia, ib;
    static doublereal wa[20];
    static doublecomplex qq[400]	/* was [20][20] */;
    static integer ier;
    extern /* Subroutine */ int qik_();
    static doublecomplex som;
    extern /* Subroutine */ int d_leqt1c__();

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
++*/
/* PURPOSE: */
/*   S is the inverse of E-C~(q) [Vrij 1979 (8)] and can be determined */
/*   from Q~. As Q~(-q)=Q~*(q) (*=complex conjugated) we find */
/*   E-C~(q) = Q~+(q)Q~(q) (Q~ times its hermitian conjugated). The */
/*   inverse is determined by the modified to double precision IMSL */
/*   routine D_LEQT1D. */

/* CALLED BY : */
/*     subint */

/* ROUTINES CALLED  : */
/*     D_LEQT1D, qik */


/* GLOBAL variables : */

/* SUBROUTINE variables : */

/* LOCAL variables  : */
/*=======================================================================
==*/
    /* Parameter adjustments */
    s -= 21;

    /* Function Body */
    *ifail = 0;
    qik_(q, qq);
    i__1 = _BLNK__1.p;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = _BLNK__1.p;
	for (j = 1; j <= i__2; ++j) {
	    i__3 = i__ + j * 20 - 21;
	    i__4 = i__ + j * 20 - 21;
	    a[i__3].r = qq[i__4].r, a[i__3].i = qq[i__4].i;
	    i__3 = i__ + j * 20 - 21;
	    b[i__3].r = 0., b[i__3].i = 0.;
	    if (i__ == j) {
		i__3 = i__ + j * 20 - 21;
		b[i__3].r = 1., b[i__3].i = 0.;
	    }
/* L200: */
	}
/* L300: */
    }

    n = _BLNK__1.p;
    ia = 20;
    ib = 20;
    m = _BLNK__1.p;
    ijob = 0;
    d_leqt1c__(a, &n, &ia, b, &m, &ib, &ijob, wa, &ier);
    if (ier != 0) {
	*ifail = 1;
    }
/* Q */
    i__1 = _BLNK__1.p;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = _BLNK__1.p;
	for (j = 1; j <= i__2; ++j) {
	    som.r = 0., som.i = 0.;
	    i__3 = _BLNK__1.p;
	    for (k = 1; k <= i__3; ++k) {
		i__4 = i__ + k * 20 - 21;
		d_cnjg(&z__3, &b[j + k * 20 - 21]);
		z__2.r = b[i__4].r * z__3.r - b[i__4].i * z__3.i, z__2.i = b[
			i__4].r * z__3.i + b[i__4].i * z__3.r;
		z__1.r = som.r + z__2.r, z__1.i = som.i + z__2.i;
		som.r = z__1.r, som.i = z__1.i;
/* L600: */
	    }
/*            write (6,*) i,j,som */
	    i__3 = i__ + j * 20;
	    s[i__3].r = som.r, s[i__3].i = som.i;
/* L500: */
	}
/* L400: */
    }
    return 0;
} /* struct_ */

int sasfit_robertus_subint(sasfit_param * param, doublereal * q, doublereal * ai, doublereal * aint0)
{
	/* System generated locals */
	integer i__1, i__2;
	doublereal d__1, d__2;

	/* Local variables */
	static doublereal a[20];
	static integer i__, j;
	static doublecomplex s[400]	/* was [20][20] */;
	static integer ifail;
	static doublereal p43;
	extern doublereal form_factor__();

	/* Fortran I/O blocks */
	static cilist io___20 = { 0, 6, 0, "(a)", 0 };
	static cilist io___21 = { 0, 6, 0, "(a)", 0 };
	static cilist io___22 = { 0, 6, 0, "(a)", 0 };

	SASFIT_ASSERT_VAL(param, -1);

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
++*/
/* PURPOSE : */
/*      Calculate scattering value including interactions (ai) for the */
/*      system at scat.vector=q, and excluding interactions (aint0) */

/* !! NOTE !! in principle different particle electron density profiles */
/* can be introduced through the function 'form_factor'. Caution should */
/* be taken here as strange results may be introduced. (negative overall 
*/
/* particle radii) */
/* For homogenous particles IFORM=1 (and dlayer(i) = 0 and dzeep=0) */
/* no problems arise. */

/* ROUTINES CALLED : */
/*     struct */

/* FUNCTIONS CALLED : */
/*     form_factor */


/* GLOBAL variables : */

/* SUBROUTINE variables : */

/* LOCAL variables : */
/* FUNCTION calls */
/*=======================================================================
==*/
    *aint0 = (float)0.;
    if (*q < (float)1e-4) {
	p43 = _BLNK__1.pi * 4. / 3.;
	i__1 = _BLNK__1.p;
	for (i__ = 1; i__ <= i__1; ++i__) {
/* Computing 3rd power */
	    d__1 = pa_1.r__[i__ - 1], d__2 = d__1;
	    a[i__ - 1] = p43 * (d__2 * (d__1 * d__1));
/* Computing 2nd power */
	    d__1 = a[i__ - 1];
	    *aint0 += d__1 * d__1 * pa_1.x[i__ - 1];
/* L5: */
	}
    } else {
	i__1 = _BLNK__1.p;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    d__1 = pa_1.r__[i__ - 1] * .5 - deeltje_1.dzeep - 
		    deeltje_1.dlayer[0];
	    a[i__ - 1] = form_factor__(&deeltje_1.iform, q, &d__1, &
		    deeltje_1.nrho, deeltje_1.rho, &deeltje_1.nlayer, 
		    deeltje_1.dlayer);
/* Computing 2nd power */
	    d__1 = a[i__ - 1];
	    *aint0 += d__1 * d__1 * pa_1.x[i__ - 1];
/* L10: */
	}
    }
	struct_(q, s, &ifail);
	if (ifail != 0) 
	{
		if (ifail == 1) 
		{
			sasfit_param_set_err(param, DBGINFO("$SNAG: Complex diagonal.\n$Intens Abort - You lose."));
			s_wsfe(&io___20);
			do_fio(&c__1, "$SNAG: Complex diagonal.", 24L);
			e_wsfe();
		} else if (ifail == 2) 
		{
			sasfit_param_set_err(param, DBGINFO("$SNAG: QQ+ is not pos. def.\n$Intens Abort - You lose."));
			s_wsfe(&io___21);
			do_fio(&c__1, "$SNAG: QQ+ is not pos. def.", 27L);
			e_wsfe();
		} else {
			sasfit_param_set_err(param, DBGINFO("$Intens Abort - You lose."));
			s_wsfe(&io___22);
			do_fio(&c__1, "$Intens Abort - You lose.", 25L);
			e_wsfe();
		}
		return 0;
	}
/* L110: */
    *ai = 0.;
/*      --- intens --- */
    i__1 = _BLNK__1.p;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = _BLNK__1.p;
	for (j = i__ + 1; j <= i__2; ++j) {
	    *ai += (s[i__ + j * 20 - 21].r) * (float)2. * a[i__ - 1] * a[
		    j - 1] * sqrt(pa_1.x[i__ - 1] * pa_1.x[j - 1]);
/*                  As S is hermitian, you can write */
/*                  the sum this way. */
/* L200: */
	}
/* Computing 2nd power */
	d__1 = a[i__ - 1];
	*ai += d__1 * d__1 * (s[i__ + i__ * 20 - 21].r) * pa_1.x[i__ - 1]
		;
/*                  s(i,i) is real, but the machine */
/*                  does not know. */
/* L300: */
    }
    return 0;
} /* subint_ */

int qik_(doublereal * q, doublecomplex * qq)
{
    /* System generated locals */
    integer i__1, i__2, i__3, i__4, i__5;
    doublereal d__1, d__2, d__3, d__4, d__5, d__6, d__7, d__8, d__9, d__10;
    doublecomplex z__1, z__2, z__3, z__4, z__5;

    /* Builtin functions */
    double sin(), cos(), sqrt();

    /* Local variables */
    static integer i__, k;
    static doublereal vrphi[20], vrpsi[20], cq[20], sq[20];
    extern integer inx_();
    static doublereal vrx[20];

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
++*/
/* PURPOSE: */
/*    This routine determines Q~(i,k) as defined in Vrij 1979 (23) but now
*/
/*     with stickyness included. */
/*     His admirerors may note that the variables start with vr. */

/* CALLED BY: struct */

/* ROUTINES CALLED: none */


/* GLOBAL variables : */

/* SUBROUTINE variables : */

/* LOCAL variables  : */
/* FUNCTION calls */
/*=======================================================================
==*/
    /* Parameter adjustments */
    qq -= 21;

    /* Function Body */
    if (*q < 1e-4) {
	i__1 = _BLNK__1.p;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    vrx[i__ - 1] = 0.;
	    sq[i__ - 1] = 0.;
	    cq[i__ - 1] = 1.;
	    vrphi[i__ - 1] = 1.;
	    vrpsi[i__ - 1] = 1.;
/* L30: */
	}
    } else {
	i__1 = _BLNK__1.p;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    vrx[i__ - 1] = pa_1.r__[i__ - 1] * .5 * *q;
	    sq[i__ - 1] = sin(vrx[i__ - 1]);
	    cq[i__ - 1] = cos(vrx[i__ - 1]);
/* Computing 3rd power */
	    d__1 = vrx[i__ - 1], d__2 = d__1;
	    vrphi[i__ - 1] = 3. / (d__2 * (d__1 * d__1)) * (sq[i__ - 1] - vrx[
		    i__ - 1] * cq[i__ - 1]);
	    vrpsi[i__ - 1] = sq[i__ - 1] / vrx[i__ - 1];
/* L10: */
	}
    }

    i__1 = _BLNK__1.p;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = _BLNK__1.p;
	for (k = 1; k <= i__2; ++k) {
	    i__3 = i__ + k * 20;
	    d__1 = fu_1.pi6 * sqrt(pa_1.x[i__ - 1] * pa_1.x[k - 1]);
	    z__3.r = d__1, z__3.i = 0.;
/* Computing 2nd power */
	    d__3 = pa_1.r__[i__ - 1] + pa_1.r__[k - 1];
/* Computing 3rd power */
	    d__4 = pa_1.r__[k - 1], d__5 = d__4;
/* Computing 2nd power */
	    d__6 = pa_1.r__[k - 1];
/* Computing 3rd power */
	    d__7 = pa_1.r__[k - 1], d__8 = d__7;
	    d__2 = ve_2.la[inx_(&i__, &k) - 1] * -.25 * pa_1.r__[k - 1] * (
		    d__3 * d__3) * vrpsi[k - 1] + d__5 * (d__4 * d__4) / 
		    fu_1.h__ * vrphi[k - 1] + d__6 * d__6 * pa_1.r__[i__ - 1] 
		    / fu_1.h__ * 3. * vrpsi[k - 1] - d__8 * (d__7 * d__7) * 
		    be_1.b[i__ - 1] / pa_1.r__[i__ - 1] * 2. * vrphi[k - 1];
/* Computing 2nd power */
	    d__10 = pa_1.r__[k - 1];
	    d__9 = -pa_1.r__[i__ - 1] * (d__10 * d__10) / fu_1.h__ * vrx[k - 
		    1] * vrphi[k - 1];
	    z__4.r = d__2, z__4.i = d__9;
	    z__2.r = z__3.r * z__4.r - z__3.i * z__4.i, z__2.i = z__3.r * 
		    z__4.i + z__3.i * z__4.r;
	    i__4 = i__ - 1;
	    i__5 = i__ - 1;
	    z__5.r = cq[i__4], z__5.i = sq[i__5];
	    z__1.r = z__2.r * z__5.r - z__2.i * z__5.i, z__1.i = z__2.r * 
		    z__5.i + z__2.i * z__5.r;
	    qq[i__3].r = z__1.r, qq[i__3].i = z__1.i;

/*                       This term is obtained by treating Perram&
Smith */
/*                       1975 (9) the way Vrij 1979 (14) does */

/*                       = cdexp(vrx(i)) */
/* L100: */
	}
/*                       Now we've got Z(i,k).... */
	i__2 = i__ + i__ * 20;
	i__3 = i__ + i__ * 20;
	z__1.r = qq[i__3].r + 1., z__1.i = qq[i__3].i + 0.;
	qq[i__2].r = z__1.r, qq[i__2].i = z__1.i;
/*                                           .... and Q~(i,k)! */
/* L200: */
    }
    return 0;
} /* qik_ */


integer igetval_(integer * il, integer * ih)
{
    /* Format strings */
    static char fmt_20[] = "(\002 Value : \002,$)";
    static char fmt_30[] = "(\002 \002,i3,\002 < value < \002,i4,\002. \002\
/\002 Again  : \002,$)";

    /* System generated locals */
    integer ret_val, i__1;

    /* Fortran I/O blocks */
    static cilist io___47 = { 0, 6, 0, fmt_20, 0 };
    static cilist io___48 = { 1, 5, 0, 0, 0 };
    static cilist io___49 = { 0, 6, 0, fmt_30, 0 };


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
++*/
/*      implicit none */
/*=======================================================================
==*/
L11:
    s_wsfe(&io___47);
    e_wsfe();
L10:
    i__1 = s_rsle(&io___48);
    if (i__1 != 0) {
	goto L11;
    }
    i__1 = do_lio(&c__3, &c__1, (char *)&ret_val, (ftnlen)sizeof(integer));
    if (i__1 != 0) {
	goto L11;
    }
    i__1 = e_rsle();
    if (i__1 != 0) {
	goto L11;
    }
    if (ret_val >= *il && ret_val <= *ih) {
	return ret_val;
    }
    s_wsfe(&io___49);
    do_fio(&c__1, (char *)&(*il), (ftnlen)sizeof(integer));
    do_fio(&c__1, (char *)&(*ih), (ftnlen)sizeof(integer));
    e_wsfe();
    if (TRUE_) {
	goto L10;
    }
    return ret_val;
} /* igetval_ */

doublereal agetval_(doublereal * bl, doublereal * bh)
{
    /* Format strings */
    static char fmt_20[] = "(\002 Value: \002,$)";
    static char fmt_30[] = "(\002 \002,f6.2,\002 <= value <= \002,f6.2,\002 \
. \002/\002 Again  : \002,$)";

    /* System generated locals */
    integer i__1;
    doublereal ret_val;

    /* Fortran I/O blocks */
    static cilist io___50 = { 0, 6, 0, fmt_20, 0 };
    static cilist io___51 = { 1, 5, 0, 0, 0 };
    static cilist io___52 = { 0, 6, 0, fmt_30, 0 };

L11:
    s_wsfe(&io___50);
    e_wsfe();
L10:
    i__1 = s_rsle(&io___51);
    if (i__1 != 0) {
	goto L11;
    }
    i__1 = do_lio(&c__5, &c__1, (char *)&ret_val, (ftnlen)sizeof(doublereal));
    if (i__1 != 0) {
	goto L11;
    }
    i__1 = e_rsle();
    if (i__1 != 0) {
	goto L11;
    }
    if (ret_val >= *bl && ret_val <= *bh) {
	return ret_val;
    }
    s_wsfe(&io___52);
    do_fio(&c__1, (char *)&(*bl), (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&(*bh), (ftnlen)sizeof(doublereal));
    e_wsfe();
    if (TRUE_) {
	goto L10;
    }
    return ret_val;
} 

doublereal fsphere_(doublereal * delrho, doublereal * r__, doublereal * q)
{
    /* System generated locals */
    doublereal ret_val;

    /* Builtin functions */
    double atan(), sin(), cos();

    /* Local variables */
    static doublereal pi, rq;

/* FUNCTION CALCULATES THE SCATTERED AMPLITUDE FROM A HOMOGENEOUS */
/* SPHERE OF RADIUS R AT SCATTERING VECTOR Q */

/* GLOBAL : */
/* LOCAL  : */

    pi = atan(1.) * 4.;
    rq = *r__ * *q;
    if (*q < 1e-7) {
	ret_val = pi * 4. * *delrho * *r__ * *r__ * *r__ / 3.;
    } else {
	ret_val = pi * 4. * *delrho * (sin(rq) - rq * cos(rq)) / (*q * *q * *
		q);
    }
    return ret_val;
} 

doublereal form_factor__(integer * iform, 
			doublereal * q_vector__, 
			doublereal * radius, 
			integer * nrho, 
			doublereal * rho, 
			integer * nlayer, 
			doublereal * dlayer)
{
    /* System generated locals */
    doublereal ret_val, d__1, d__2, d__3, d__4, d__5, d__6, d__7, d__8, d__9, 
	    d__10;
	doublereal Q,r,Dr;

    /* Builtin functions */
//    double atan(), pow_dd();

    /* Local variables */
    static doublereal y, yzero, pi;
    extern doublereal fshell_(), flinsphere_();
    static doublereal ppi;
    extern doublereal fsphere_();

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
++*/
/*       IMPLICIT NONE */
/* NRHO,NLAYER: Number of RHO(),DLAYER() */
/* RHO(I)     : Electron densities (MAX = 12) */
/* DLAYER(I)  : Transition layer distances (MAX = 10) */

/*    Q_VECTOR     : SCATTERING ANGLE */
/*    RADIUS       : MEAN OF THE GAUS DISTRIBUTION */

/*  ROUTINES CALLED : */
/* ================== */
/*                 SCATTERED AMPLITUDE (NOT INTENSITY) OF */
/*   FSPHERE    :  HOMOGENOUS SPHERE */
/*   FLINSPHERE :  DIFFUSE SPHERE (LINEAR DECREASING DENSITY) */
/*   FSHELL     :  SHELL */

/* LOCAL  : */

/*=======================================================================
==*/
    /* Parameter adjustments */
    --dlayer;
    --rho;

	pi = atan(1.) * 4.;
    ppi = pi * 4. / 3.;
	if (*iform == -1) {
		Q = (*q_vector__);
		r = (*radius);
		Dr = dlayer[1];
		yzero = 0.0;
		if (r==0) {
			y = 0;
		} else {
			yzero = (rho[1] - rho[2])* ppi*pow((r-Dr),3.0);
			if (Q==0) {
				y = yzero;
			} else {
		        y =  (rho[1] - rho[2])* ppi*3.*(sin(Q*(r-Dr)) - Q*(r-Dr)*cos(Q*(r-Dr)))/pow(Q,3.0);
			}
		}
		if (Dr!=0) {
           yzero = yzero + rho[2] * ppi*pow(r,3.0);
		   if (Q==0) {
			   y = yzero;
		   } else {
		       y=y+ rho[2] * ppi*3.*(sin(Q*r) - Q*r*cos(Q*r))/pow(Q,3.0);
		   }
		}
		yzero=1.0;
	}

    /* Function Body */
    if (*iform == 1) {
	d__1 = rho[1] - rho[2];
	y = fsphere_(&d__1, radius, q_vector__);
/* Computing 3rd power */
	d__1 = *radius, d__2 = d__1;
	yzero = (rho[1] - rho[2]) * (d__2 * (d__1 * d__1));
    } else if (*iform == 2) {
	d__1 = rho[1] - rho[2];
	y = flinsphere_(&d__1, radius, &dlayer[1], q_vector__);
/* Computing 3rd power */
	d__1 = *radius, d__2 = d__1;
/* Computing 4th power */
	d__3 = *radius + dlayer[1], d__3 *= d__3;
/* Computing 4th power */
	d__4 = *radius, d__4 *= d__4;
/* Computing 3rd power */
	d__5 = *radius + dlayer[1], d__6 = d__5;
/* Computing 3rd power */
	d__7 = *radius, d__8 = d__7;
	yzero = (rho[1] - rho[2]) * (d__2 * (d__1 * d__1)) + (rho[1] - rho[2])
		 * (float).75 / dlayer[1] * (d__3 * d__3 - d__4 * d__4) + (
		rho[1] - *radius * (rho[1] - rho[2]) / dlayer[1]) * (d__6 * (
		d__5 * d__5) - d__8 * (d__7 * d__7));
    } else if (*iform == 3) {
	d__1 = rho[1] - rho[2];
	d__2 = rho[2] - rho[3];
	y = fshell_(&d__1, &d__2, radius, &dlayer[1], q_vector__);
/* Computing 3rd power */
	d__1 = *radius, d__2 = d__1;
/* Computing 3rd power */
	d__3 = *radius + dlayer[1], d__4 = d__3;
	yzero = d__2 * (d__1 * d__1) * (rho[1] - rho[2]) + d__4 * (d__3 * 
		d__3) * (rho[2] - rho[3]);
    } else if (*iform == 4) {
	d__1 = rho[2] - rho[3];
	d__2 = rho[2] - rho[1];
	y = flinsphere_(&d__1, radius, &dlayer[1], q_vector__) - fsphere_(&
		d__2, radius, q_vector__);
/* Computing 3rd power */
	d__1 = *radius, d__2 = d__1;
/* Computing 4th power */
	d__3 = *radius + dlayer[1], d__3 *= d__3;
/* Computing 4th power */
	d__4 = *radius, d__4 *= d__4;
/* Computing 3rd power */
	d__5 = *radius + dlayer[1], d__6 = d__5;
/* Computing 3rd power */
	d__7 = *radius, d__8 = d__7;
	yzero = (rho[1] - rho[2]) * (d__2 * (d__1 * d__1)) + (rho[2] - rho[3])
		 * pow_dd(radius, &c_b85) * (rho[2] - rho[3]) / dlayer[1] * (
		d__3 * d__3 - d__4 * d__4) + (rho[2] - *radius * (rho[2] - 
		rho[3]) / dlayer[1]) * (d__6 * (d__5 * d__5) - d__8 * (d__7 * 
		d__7));
    } else if (*iform == 5) {
	d__1 = rho[1] - rho[2];
	d__2 = rho[2] - rho[3];
	d__3 = *radius + dlayer[1];
	y = flinsphere_(&d__1, radius, &dlayer[1], q_vector__) + fsphere_(&
		d__2, &d__3, q_vector__);
/* Computing 3rd power */
	d__1 = *radius, d__2 = d__1;
/* Computing 4th power */
	d__3 = *radius + dlayer[1], d__3 *= d__3;
/* Computing 4th power */
	d__4 = *radius, d__4 *= d__4;
/* Computing 3rd power */
	d__5 = *radius + dlayer[1], d__6 = d__5;
/* Computing 3rd power */
	d__7 = *radius, d__8 = d__7;
/* Computing 3rd power */
	d__9 = *radius + dlayer[1], d__10 = d__9;
	yzero = (rho[1] - rho[2]) * (d__2 * (d__1 * d__1)) + (rho[1] - rho[2])
		 * (float).75 / dlayer[1] * (d__3 * d__3 - d__4 * d__4) + (
		rho[1] - *radius * (rho[1] - rho[2]) / dlayer[1]) * (d__6 * (
		d__5 * d__5) - d__8 * (d__7 * d__7)) + (rho[2] - rho[3]) * (
		d__10 * (d__9 * d__9));
    } else if (*iform == 6) {
	d__1 = rho[1] - rho[2];
	d__2 = rho[2] - rho[3];
	d__3 = *radius + dlayer[1];
	y = fsphere_(&d__1, radius, q_vector__) + flinsphere_(&d__2, &d__3, &
		dlayer[2], q_vector__);
	yzero = 1.;
    } else if (*iform == 7) {
	d__1 = rho[1] - rho[2];
	d__2 = rho[2] - rho[3];
	d__3 = *radius + dlayer[1];
	y = flinsphere_(&d__1, radius, &dlayer[1], q_vector__) + flinsphere_(&
		d__2, &d__3, &dlayer[2], q_vector__);
	yzero = 1.;
    } else if (*iform == 8) {
	d__1 = rho[1] - rho[2];
	d__2 = rho[2] - rho[3];
	d__3 = *radius + dlayer[1] + dlayer[2];
	y = flinsphere_(&d__1, radius, &dlayer[1], q_vector__) + fsphere_(&
		d__2, &d__3, q_vector__);
	yzero = 1.;
    } else if (*iform == 9) {
	d__1 = rho[1] - rho[2];
	d__2 = rho[2] - rho[3];
	d__3 = *radius + dlayer[1] + dlayer[2];
	y = flinsphere_(&d__1, radius, &dlayer[1], q_vector__) + flinsphere_(&
		d__2, &d__3, &dlayer[3], q_vector__);
	yzero = 1.;
    } else if (*iform == 10) {
	d__1 = rho[1] - rho[2];
	d__2 = rho[2] - rho[3];
	d__3 = *radius + dlayer[1];
	d__4 = rho[3] - rho[4];
	d__5 = *radius + dlayer[1] + dlayer[2];
	y = fsphere_(&d__1, radius, q_vector__) + fsphere_(&d__2, &d__3, 
		q_vector__) + fsphere_(&d__4, &d__5, q_vector__);
/* Computing 3rd power */
	d__1 = *radius, d__2 = d__1;
/* Computing 3rd power */
	d__3 = *radius + dlayer[1], d__4 = d__3;
/* Computing 3rd power */
	d__5 = *radius + dlayer[1] + dlayer[2], d__6 = d__5;
	yzero = (rho[1] - rho[2]) * (d__2 * (d__1 * d__1)) + (rho[2] - rho[3])
		 * (d__4 * (d__3 * d__3)) + (rho[3] - rho[4]) * (d__6 * (d__5 
		* d__5));
    }
    ret_val = y;
/*      FORM_FACTOR = Y / YZERO / PPI */
    return ret_val;
} 

doublereal flinsphere_(doublereal * delrho, doublereal * r__, doublereal * d__, doublereal * q)
{
    /* System generated locals */
    doublereal ret_val;

    /* Builtin functions */
//    double atan(), cos(), sin();

    /* Local variables */
    static doublereal x, y, pi;

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
*/
/*        IMPLICIT NONE */
/* FUNCTION CALCULATES THE SCATTERED AMPLITUDE FROM A HOMOGENEOUS */
/* SPHERE OF RADIUS R , WITH A LINEAR DECREASING ELECTRON DENSITY */
/* OVER A DISTANCE D , AT SCATTERING VECTOR Q */

    pi = atan(1.) * 4.;
    x = *r__ * *q;
    y = (*r__ + *d__) * *q;
    ret_val = pi * 4. * *delrho / (*q * *q * *q) * (((cos(y) - cos(x)) * 2. - 
	    x * sin(x) + y * sin(y)) / (x - y));
    return ret_val;
} /* flinsphere_ */

doublereal fshell_(doublereal * delrh1, doublereal * delrh2, doublereal * r__, doublereal * d__, doublereal * q)
{
    /* System generated locals */
    doublereal ret_val, d__1;

    /* Local variables */
    extern doublereal fsphere_();

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
++*/
/*        IMPLICIT NONE */
/* FUNCTION CALCULATES THE SCATTERED AMPLITUDE FROM A SHELL PARTICLE */
/* WITH INNER RADIUS R, OUTER RADIUS R+D AT SCATTERING VECTOR Q. */


    d__1 = *r__ + *d__;
    ret_val = fsphere_(delrh1, r__, q) + fsphere_(delrh2, &d__1, q);
    return ret_val;
} 


/* THIS ROUTINE NOW WORKS IN DOUBLE PRECISION !!!! */
/* IT'S NAME IS THERFORE CHANGED IN D_LEQT1C */
/*   IMSL ROUTINE NAME   - D_LEQT1C */
/*                                                                      LE1C00
20*/
/*-----------------------------------------------------------------------LE1C0
030*/
/*                                                                      LE1C00
40*/
/*  COMPUTER            - VAX/SINGLE                                    LE1C00
50*/
/*                                                                      LE1C00
60*/
/*  LATEST REVISION     - JANUARY 1, 1978                               LE1C00
70*/
/*                                                                      LE1C00
80*/
/*  PURPOSE             - MATRIX DECOMPOSITION, LINEAR EQUATION         LE1C00
90*/
/*                          SOLUTION - SPACE ECONOMIZER SOLUTION -      LE1C01
00*/
/*                          COMPLEX MATRICES                            LE1C01
10*/
/*                                                                      LE1C01
20*/
/*  USAGE               - CALL D_LEQT1C (A,N,IA,B,M,IB,IJOB,WA,IER)     LE1C01
30*/
/*                                                                      LE1C01
40*/
/*  ARGUMENTS    A      - INPUT COMPLEX N BY N MATRIX CONTAINING THE    LE1C01
50*/
/*                          COMPLEX COEFFICIENTS OF THE EQUATION AX = B.LE1C01
60*/
/*                        ON OUTPUT, A CONTAINS THE L-U DECOMPOSITION OFLE1C01
70*/
/*                          A ROWWISE PERMUTATION OF THE INPUT MATRIX A.LE1C01
80*/
/*               N      - ORDER OF MATRIX A. (INPUT)                    LE1C01
90*/
/*               IA     - ROW DIMENSION OF MATRIX A EXACTLY AS          LE1C02
00*/
/*                          SPECIFIED IN THE DIMENSION STATEMENT IN THE LE1C02
10*/
/*                          CALLING PROGRAM. (INPUT)                    LE1C02
20*/
/*               B      - INPUT COMPLEX N BY M MATRIX CONTAINING THE    LE1C02
30*/
/*                          M COMPLEX VALUED RIGHT HAND SIDES OF THE    LE1C02
40*/
/*                          EQUATION AX = B.                            LE1C02
50*/
/*                        ON OUTPUT, THE SOLUTION MATRIX X REPLACES B.  LE1C02
60*/
/*                          IF IJOB=1, B IS NOT USED.                   LE1C02
70*/
/*               M      - NUMBER OF RIGHT HAND SIDES (COLUMNS IN B).    LE1C02
80*/
/*                          (INPUT)                                     LE1C02
90*/
/*               IB     - ROW DIMENSION OF MATRIX B EXACTLY AS SPECIFIEDLE1C03
00*/
/*                          IN THE DIMENSION STATEMENT IN THE CALLING   LE1C03
10*/
/*                          PROGRAM. (INPUT)                            LE1C03
20*/
/*               IJOB   - INPUT OPTION PARAMETER.  IJOB=I IMPLIES WHEN  LE1C03
30*/
/*                          I=0, FACTOR THE MATRIX AND SOLVE THE        LE1C03
40*/
/*                            EQUATION AX=B.                            LE1C03
50*/
/*                          I=1, FACTOR THE MATRIX A.                   LE1C03
60*/
/*                          I=2, SOLVE THE EQUATION AX=B.  THIS         LE1C03
70*/
/*                            OPTION IMPLIES THAT D_LEQT1C HAS ALREADY  LE1C03
80*/
/*                            BEEN CALLED USING IJOB=0 OR 1 SO THAT     LE1C03
90*/
/*                            THE MATRIX HAS ALREADY BEEN FACTORED.  IN LE1C04
00*/
/*                            THIS CASE OUTPUT MATRIX A MUST HAVE BEEN  LE1C04
10*/
/*                            SAVED FOR REUSE IN THE CALL TO D_LEQT1C.  LE1C04
20*/
/*               WA     - WORK AREA OF LENGTH N CONTAINING THE PIVOT    LE1C04
30*/
/*                          INDICES.                                    LE1C04
40*/
/*               IER    - ERROR PARAMETER. (OUTPUT)                     LE1C04
50*/
/*                        TERMINAL ERROR                                LE1C04
60*/
/*                          IER=129 INDICATES THAT MATRIX A IS          LE1C04
70*/
/*                            ALGORITHMICALLY SINGULAR.  (SEE THE       LE1C04
80*/
/*                            CHAPTER L PRELUDE.)                       LE1C04
90*/
/*                                                                      LE1C05
00*/
/*  PRECISION/HARDWARE  - SINGLE AND DOUBLE/H32                         LE1C05
10*/
/*                      - SINGLE/H36,H48,H60                            LE1C05
20*/
/*                                                                      LE1C05
30*/
/*  REQD. IMSL ROUTINES - UERTST,UGETIO                                 LE1C05
40*/
/*                                                                      LE1C05
50*/
/*  NOTATION            - INFORMATION ON SPECIAL NOTATION AND           LE1C05
60*/
/*                          CONVENTIONS IS AVAILABLE IN THE MANUAL      LE1C05
70*/
/*                          INTRODUCTION OR THROUGH IMSL ROUTINE UHELP  LE1C05
80*/
/*                                                                      LE1C05
90*/
/*  REMARKS  1.  WHEN IJOB=1, ARGUMENTS B, M AND IB ARE NOT USED BY     LE1C06
00*/
/*               D_LEQT1C.                                              LE1C06
10*/
/*           2.  INPUT MATRIX A IS DESTROYED WHEN IJOB=0 OR 1. WHEN     LE1C06
20*/
/*               IJOB=0 OR 2, B IS REPLACED WITH THE SOLUTION X.        LE1C06
30*/
/*           3.  D_LEQT1C CAN BE USED TO COMPUTE THE INVERSE OF A COMPLELE1C06
40*/
/*               XMATRIX. THIS IS DONE BY CALLING LEQT1C WITH M=N,      LE1C06
50*/
/*               B=THE N BY N IDENTITY MATRIX AND IJOB=0. WHEN N IS     LE1C06
60*/
/*               LARGE, IT MAY BE MORE PRACTICAL TO COMPUTE THE INVERSE LE1C06
70*/
/*               A COLUMN AT A TIME. TO DO THIS, FIRST CALL LEQT1C WITH LE1C06
80*/
/*               IJOB=1 TO FACTOR A. MAKE SUCCEEDING CALLS WITH M=1, B  LE1C06
90*/
/*               =A COLUMN OF THE IDENTITY MATRIX AND IJOB=2. B WILL BE LE1C07
00*/
/*               REPLACED BY THE CORRESPONDING COLUMN OF A INVERSE.     LE1C07
10*/
/*           4.  THE DETERMINANT OF A CAN BE COMPUTED AFTER LEQT1C HAS  LE1C07
20*/
/*               BEEN CALLED AS FOLLOWS                                 LE1C07
30*/
/*                                                                      LE1C07
40*/
/*                 DET = (1.0,0.0)                                      LE1C07
50*/
/*                 DO 5 I = 1,N                                         LE1C07
60*/
/*                    IPVT = WA(I)                                      LE1C07
70*/
/*                    IF (IPVT .NE. I) DET = -DET                       LE1C07
80*/
/*                    DET = DET*A(I,I)                                  LE1C07
90*/
/*               5 CONTINUE                                             LE1C08
00*/
/*                                                                      LE1C08
10*/
/*  COPYRIGHT           - 1978 BY IMSL, INC. ALL RIGHTS RESERVED.       LE1C08
20*/
/*                                                                      LE1C08
30*/
/*  WARRANTY            - IMSL WARRANTS ONLY THAT IMSL TESTING HAS BEEN LE1C08
40*/
/*                          APPLIED TO THIS CODE. NO OTHER WARRANTY,    LE1C08
50*/
/*                          EXPRESSED OR IMPLIED, IS APPLICABLE.        LE1C08
60*/
/*                                                                      LE1C08
70*/
/*-----------------------------------------------------------------------LE1C0
880*/
/*                                                                      LE1C08
90*/

int d_leqt1c__(doublecomplex * a, integer * n, integer * ia, doublecomplex * b, integer * m, integer * ib, integer * ijob, doublereal * wa, integer * ier)
{
    /* Initialized data */

    static doublereal zero = 0.;
    static doublereal one = 1.;

    /* System generated locals */
    integer a_dim1, a_offset, b_dim1, b_offset, i__1, i__2, i__3, i__4, i__5;
    doublecomplex z__1, z__2;
    static doublecomplex equiv_0[1];

    /* Local variables */
    static integer imax;
    static doublecomplex temp;
    static integer i__, j, k;
    static doublereal p, q;
#define t ((doublereal *)equiv_0)
    static integer n1;
    static doublereal rn;
    static integer iw, im1, jm1, jp1;
    static doublereal big;
#define sum (equiv_0)

/*                                 SPECIFICATIONS FOR ARGUMENTS         LE
1C0910*/
/*                                 SPECIFICATIONS FOR LOCAL VARIABLES   LE
1C0950*/
    /* Parameter adjustments */
    --wa;
    a_dim1 = *ia;
    a_offset = a_dim1 + 1;
    a -= a_offset;
    b_dim1 = *ib;
    b_offset = b_dim1 + 1;
    b -= b_offset;

    /* Function Body */
/*                                 INITIALIZATION                       LE
1C1010*/
/*                                 FIRST EXECUTABLE STATEMENT           LE
1C1020*/
    *ier = 0;
    if (*ijob == 2) {
	goto L75;
    }
    rn = (doublereal) (*n);
/*                                 FIND EQUILIBRATION FACTORS           LE
1C1060*/
    i__1 = *n;
    for (i__ = 1; i__ <= i__1; ++i__) {
	big = zero;
	i__2 = *n;
	for (j = 1; j <= i__2; ++j) {
	    i__3 = i__ + j * a_dim1;
	    temp.r = a[i__3].r, temp.i = a[i__3].i;
	    p = z_abs(&temp);
	    if (p > big) {
		big = p;
	    }
/* L5: */
	}
	if (big == zero) {
	    goto L105;
	}
	wa[i__] = one / big;
/* L10: */
    }
/*                                 L-U DECOMPOSITION                    LE
1C1170*/
    i__1 = *n;
    for (j = 1; j <= i__1; ++j) {
	jm1 = j - 1;
	if (jm1 < 1) {
	    goto L25;
	}
/*                                 COMPUTE U(I,J), I=1,...,J-1        
  LE1C1210*/
	i__2 = jm1;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    i__3 = i__ + j * a_dim1;
	    sum->r = a[i__3].r, sum->i = a[i__3].i;
	    im1 = i__ - 1;
	    if (im1 < 1) {
		goto L20;
	    }
	    i__3 = im1;
	    for (k = 1; k <= i__3; ++k) {
		i__4 = i__ + k * a_dim1;
		i__5 = k + j * a_dim1;
		z__2.r = a[i__4].r * a[i__5].r - a[i__4].i * a[i__5].i, 
			z__2.i = a[i__4].r * a[i__5].i + a[i__4].i * a[i__5]
			.r;
		z__1.r = sum->r - z__2.r, z__1.i = sum->i - z__2.i;
		sum->r = z__1.r, sum->i = z__1.i;
/* L15: */
	    }
	    i__3 = i__ + j * a_dim1;
	    a[i__3].r = sum->r, a[i__3].i = sum->i;
L20:
	    ;
	}
L25:
	p = zero;
/*                                 COMPUTE U(J,J) AND L(I,J), I=J+1,..
.,LE1C1320*/
	i__2 = *n;
	for (i__ = j; i__ <= i__2; ++i__) {
	    i__3 = i__ + j * a_dim1;
	    sum->r = a[i__3].r, sum->i = a[i__3].i;
	    if (jm1 < 1) {
		goto L40;
	    }
	    i__3 = jm1;
	    for (k = 1; k <= i__3; ++k) {
		i__4 = i__ + k * a_dim1;
		i__5 = k + j * a_dim1;
		z__2.r = a[i__4].r * a[i__5].r - a[i__4].i * a[i__5].i, 
			z__2.i = a[i__4].r * a[i__5].i + a[i__4].i * a[i__5]
			.r;
		z__1.r = sum->r - z__2.r, z__1.i = sum->i - z__2.i;
		sum->r = z__1.r, sum->i = z__1.i;
/* L35: */
	    }
	    i__3 = i__ + j * a_dim1;
	    a[i__3].r = sum->r, a[i__3].i = sum->i;
L40:
	    q = wa[i__] * z_abs(sum);
	    if (p >= q) {
		goto L45;
	    }
	    p = q;
	    imax = i__;
L45:
	    ;
	}
/*                                 TEST FOR ALGORITHMIC SINGULARITY   
  LE1C1450*/
	q = rn + p;
	if (q == rn) {
	    goto L105;
	}
	if (j == imax) {
	    goto L60;
	}
/*                                 INTERCHANGE ROWS J AND IMAX        
  LE1C1490*/
	i__2 = *n;
	for (k = 1; k <= i__2; ++k) {
	    i__3 = imax + k * a_dim1;
	    temp.r = a[i__3].r, temp.i = a[i__3].i;
	    i__3 = imax + k * a_dim1;
	    i__4 = j + k * a_dim1;
	    a[i__3].r = a[i__4].r, a[i__3].i = a[i__4].i;
	    i__3 = j + k * a_dim1;
	    a[i__3].r = temp.r, a[i__3].i = temp.i;
/* L50: */
	}
	wa[imax] = wa[j];
L60:
	wa[j] = (doublereal) imax;
	jp1 = j + 1;
	if (jp1 > *n) {
	    goto L70;
	}
/*                                 DIVIDE BY PIVOT ELEMENT U(J,J)     
  LE1C1590*/
	i__2 = j + j * a_dim1;
	temp.r = a[i__2].r, temp.i = a[i__2].i;
	i__2 = *n;
	for (i__ = jp1; i__ <= i__2; ++i__) {
	    i__3 = i__ + j * a_dim1;
	    z_div(&z__1, &a[i__ + j * a_dim1], &temp);
	    a[i__3].r = z__1.r, a[i__3].i = z__1.i;
/* L65: */
	}
L70:
	;
    }
L75:
    if (*ijob == 1) {
	goto L9005;
    }
    i__1 = *m;
    for (k = 1; k <= i__1; ++k) {
/*                                 SOLVE UX = Y FOR X                 
  LE1C1670*/
	iw = 0;
	i__2 = *n;
	for (i__ = 1; i__ <= i__2; ++i__) {
	    imax = (integer) wa[i__];
	    i__3 = imax + k * b_dim1;
	    sum->r = b[i__3].r, sum->i = b[i__3].i;
	    i__3 = imax + k * b_dim1;
	    i__4 = i__ + k * b_dim1;
	    b[i__3].r = b[i__4].r, b[i__3].i = b[i__4].i;
	    if (iw == 0) {
		goto L85;
	    }
	    im1 = i__ - 1;
	    i__3 = im1;
	    for (j = iw; j <= i__3; ++j) {
		i__4 = i__ + j * a_dim1;
		i__5 = j + k * b_dim1;
		z__2.r = a[i__4].r * b[i__5].r - a[i__4].i * b[i__5].i, 
			z__2.i = a[i__4].r * b[i__5].i + a[i__4].i * b[i__5]
			.r;
		z__1.r = sum->r - z__2.r, z__1.i = sum->i - z__2.i;
		sum->r = z__1.r, sum->i = z__1.i;
/* L80: */
	    }
	    goto L88;
L85:
	    if (t[0] != zero || t[1] != zero) {
		iw = i__;
	    }
L88:
	    i__3 = i__ + k * b_dim1;
	    b[i__3].r = sum->r, b[i__3].i = sum->i;
/* L90: */
	}
/*                                 SOLVE LY = B FOR Y                 
  LE1C1820*/
	n1 = *n + 1;
	i__2 = *n;
	for (iw = 1; iw <= i__2; ++iw) {
	    i__ = n1 - iw;
	    jp1 = i__ + 1;
	    i__3 = i__ + k * b_dim1;
	    sum->r = b[i__3].r, sum->i = b[i__3].i;
	    if (jp1 > *n) {
		goto L98;
	    }
	    i__3 = *n;
	    for (j = jp1; j <= i__3; ++j) {
		i__4 = i__ + j * a_dim1;
		i__5 = j + k * b_dim1;
		z__2.r = a[i__4].r * b[i__5].r - a[i__4].i * b[i__5].i, 
			z__2.i = a[i__4].r * b[i__5].i + a[i__4].i * b[i__5]
			.r;
		z__1.r = sum->r - z__2.r, z__1.i = sum->i - z__2.i;
		sum->r = z__1.r, sum->i = z__1.i;
/* L95: */
	    }
L98:
	    i__3 = i__ + k * b_dim1;
	    z_div(&z__1, sum, &a[i__ + i__ * a_dim1]);
	    b[i__3].r = z__1.r, b[i__3].i = z__1.i;
/* L100: */
	}
/* L103: */
    }
    goto L9005;
/*                                 ALGORITHMIC SINGULARITY              LE
1C1960*/
L105:
    *ier = 129;
/* L9000: */
/*                                 PRINT ERROR                          LE
1C1990*/
//    uertst_(ier, "LEQT1C", 6L);
L9005:
    return 0;
}

#undef sum
#undef t

