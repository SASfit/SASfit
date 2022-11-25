/*
 * src/sasfit_common/f2c_utils/sasfit_th36.c
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

/* Common Block Declarations */

union {
    struct {
	doublereal q, phi, n, nw, drho1, drho2, sig1, sig2, rmain, zz, rshift,
		 bckgr;
    } _1;
    struct {
	doublereal q, phi, no, nw, drho1, drho2, sig1, sig2, rmain, zz, 
		rshift, bckgr;
    } _2;
} vespar4_;

#define vespar4_1 (vespar4_._1)
#define vespar4_2 (vespar4_._2)

struct {
    integer iadda;
} thiadd_;

#define thiadd_1 thiadd_

doublereal invesicle4a_(doublereal * r0);
doublereal invesicle4_(doublereal * r0, doublereal * n);
doublereal oszi_(doublereal * a, doublereal * b);
doublereal s14aaf_(doublereal * x, integer * ifail);
doublereal s14abf_(doublereal * x, integer * ifail);
doublereal dlgama_(doublereal * x);
integer p01aaf_(integer * ifail, integer * error, doublereal * srname);
int x04aaf_(integer * i__, integer * nerr);

/* Table of constant values */

static integer c__1 = 1;
static doublereal c_b22 = 2.;
static doublereal c_b26 = 3.;
static doublereal c_b27 = 4.;
static integer c__2 = 2;
static integer c__3 = 3;
static integer c__4 = 4;
static integer c__0 = 0;

//doublereal sasfit_th36(real * x, real * pa, char * thnam, char * parnam, integer * npar, integer * ini, ftnlen thnam_len, ftnlen parnam_len)
doublereal sasfit_th36(real * x, sasfit_param * param, char * thnam, char * parnam, integer * npar, integer * ini, ftnlen thnam_len, ftnlen parnam_len)
{
    /* Format strings */
    static char fmt_1[] = "(\002 theory: \002,a8,\002 no of parameters=\002,\
i8,\002 exceeds current max. = \002,i8)";

    /* System generated locals */
    integer i__1;
    real ret_val;

    /* Builtin functions */
    /*
    int s_copy();
    integer s_wsfe(), do_fio(), e_wsfe();
    double sqrt();
    */

    /* Local variables */
    static doublereal bmin, bmax, aint, rhow;
    static integer i__, maxit, nparx;
    static doublereal r0;
    static doublereal eps, rho1, rho2;

    /* Fortran I/O blocks */
    static cilist io___9 = { 0, 6, 0, fmt_1, 0 };

    SASFIT_ASSERT_PTR(param);


/* Q-Vektor */
/* Konzentration Polymer */
/* Anzahl der Schichten */
/* width of n-distribution */
/* SLD   Wasser */
/* SLD   der zentralen Schicht (1) und */
/* Dicke der zentralen Schicht (1) und */
/* Main Radius */
/* Z - Schultz distribution */
/* Maximum possible shift */

/* background */

/*     double precision qa,delta */
/*     double precision guess       ! Wert für Integral */

/* Kontrast der zentralen Schicht (1) */
/* Integrationsgrenzen */
/* Fehler */
/*     double precision erracc      ! Tats"achlicher Fehler */
/* Maximale Iterationen */
/* Integral */
/*     double precision epsrel      ! relative error */
/*     double precision errest      ! estimated error */
/*     integer          irule       ! number of points */

/* actual value */

/*     double precision adapint */

/* for parameter get */

/*     initialisation of names and number of parameters */

/* ! transfer of address f */
    /* Parameter adjustments */
    parnam -= 8;
    //--pa;

    /* Function Body */
    if (*ini == 0) {
	s_copy(thnam, "vesicle4", 8L, 8L);
	nparx = 12;
	if (*npar < nparx) {
	    s_wsfe(&io___9);
	    do_fio(&c__1, thnam, 8L);
	    do_fio(&c__1, (char *)&nparx, (ftnlen)sizeof(integer));
	    do_fio(&c__1, (char *)&(*npar), (ftnlen)sizeof(integer));
	    e_wsfe();
	    ret_val = (float)0.;
	    return ret_val;
	}
	*npar = nparx;
	s_copy(parnam + 8, "phi", 8L, 3L);
/* Concentration Polymer */
	s_copy(parnam + 16, "n", 8L, 1L);
/* Number of layers */
	s_copy(parnam + 24, "nw", 8L, 2L);
/* width of n-distribution */
	s_copy(parnam + 32, "rhoW", 8L, 4L);
/* Scattering length density water */
	s_copy(parnam + 40, "rho1", 8L, 4L);
/* Scattering length density 'core' */
	s_copy(parnam + 48, "rho2", 8L, 4L);
/* Scattering length density 'shell' */
	s_copy(parnam + 56, "sig1", 8L, 4L);
/* Thickness 'core' */
	s_copy(parnam + 64, "sig2", 8L, 4L);
/* Thickness 'shell' */
	s_copy(parnam + 72, "Rmain", 8L, 5L);
/* Main Radius (distance between layers) */
	s_copy(parnam + 80, "zz", 8L, 2L);
/* Schultz Z */
	s_copy(parnam + 88, "Rshift", 8L, 6L);
/* Maximum possible shift */
	s_copy(parnam + 96, "bckgr", 8L, 5L);
/* background */
	ret_val = (float)0.;
	return ret_val;

/*     actual numerical body of function */

    } else {
	vespar4_1.q = *x;
	/*
	vespar4_1.phi = (doublereal) pa[1];
	vespar4_1.n = (doublereal) pa[2];
	vespar4_1.nw = (doublereal) pa[3];
	rhow = (doublereal) pa[4];
	rho1 = (doublereal) pa[5];
	rho2 = (doublereal) pa[6];
	vespar4_1.sig1 = (doublereal) pa[7];
	vespar4_1.sig2 = (doublereal) pa[8];
	vespar4_1.rmain = (doublereal) pa[9];
	vespar4_1.zz = (doublereal) pa[10];
	vespar4_1.rshift = (doublereal) pa[11];
	vespar4_1.bckgr = (doublereal) pa[12];
	*/
	vespar4_1.phi 		= (doublereal) param->p[MAXPAR-2];
	vespar4_1.n 		= (doublereal) param->p[0];
	vespar4_1.nw 		= (doublereal) param->p[1];
	rhow 			= (doublereal) param->p[2];
	rho1 			= (doublereal) param->p[3];
	rho2 			= (doublereal) param->p[4];
	vespar4_1.sig1 		= (doublereal) param->p[5];
	vespar4_1.sig2 		= (doublereal) param->p[6];
	vespar4_1.rmain 	= (doublereal) param->p[7];
	vespar4_1.zz 		= (doublereal) param->p[8];
	vespar4_1.rshift	= (doublereal) param->p[9];
	vespar4_1.bckgr 	= (doublereal) param->p[MAXPAR-1];

	vespar4_1.n = max(1.,vespar4_1.n);

	vespar4_1.drho1 = rho1 - rhow;
	vespar4_1.drho2 = rho2 - rhow;

	if (vespar4_1.zz <= -.99) {
	    ret_val = (real) invesicle4a_(&vespar4_1.rmain);
	} else {
	    if (abs(vespar4_1.rshift) < abs(vespar4_1.rmain) * .5) {
		vespar4_1.rshift = abs(vespar4_1.rmain) * .5;
	    }
/*       delta = 1.177410023d0 * (sig1+sig2) */
/*       qa    = abs(q*Rmain) */
/*       guess  = phi *4d-24*Pi*Rmain**2d0/ 6d0*(((2d0*n+3d0)*n+1d
0)*n) */
/*    x         *(drho1*sig1*exp(-.5d0*q*q*sig1*sig1) */
/*    x          +drho2*sig2*exp(-.5d0*q*q*sig2*sig2)*2d0*cos(delt
a*q)) */
/*    x   /(1d0+qa*qa) */
/*    x   *( n*(n+0.5d0)*(n+1d0)/6d0 + 0.25d0* */
/*    x     (cos(2d0*qa*(n+1d0))*(-(n+1d0)/(sin(qa)**2d0)+(n+1d0)
**2d0) */
/*    x     +sin(2d0*qa*(n+1d0))*(-(n+1d0)**2d0+0.5d0/(sin(qa)**2d
0) ) */
/*    x                         /tan(qa) ) ) / (qa*qa) */
/*       bmin  = 0.05d0 * Rmain */
/*       bmax  = 4.05d0 * Rmain */
/*       eps   = guess */
/*       maxit = 3000 */
/*       erracc= 0d0 */
/*       aint  = adapint(inVesicle4a,bmin,bmax,eps,maxit,erracc) 
*/
	    aint = 0.;
	    bmin = vespar4_1.rmain / sqrt(vespar4_1.zz + 1.) / 3.;
	    bmax = vespar4_1.rmain / sqrt(vespar4_1.zz + 1.);
	    maxit = 100;
	    eps = (bmax - bmin) / maxit;
	    i__1 = maxit;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		r0 = (i__ - .5) * eps + bmin;
		aint += invesicle4a_(&r0) * eps;
	    }
	    bmin = vespar4_1.rmain * sqrt(vespar4_1.zz + 1.);
	    bmax = vespar4_1.rmain * sqrt(vespar4_1.zz + 1.) * 3.;
	    maxit = 100;
	    eps = (bmax - bmin) / maxit;
	    i__1 = maxit;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		r0 = (i__ - .5) * eps + bmin;
		aint += invesicle4a_(&r0) * eps;
	    }
	    bmin = vespar4_1.rmain / sqrt(vespar4_1.zz + 1.);
	    bmax = vespar4_1.rmain * sqrt(vespar4_1.zz + 1.);
	    maxit = 1000;
	    eps = (bmax - bmin) / maxit;
	    i__1 = maxit;
	    for (i__ = 1; i__ <= i__1; ++i__) {
		r0 = (i__ - .5) * eps + bmin;
		aint += invesicle4a_(&r0) * eps;
	    }
/*       bmin  = Rmain / sqrt(zz+1d0) / 3d0 */
/*       bmax  = Rmain * sqrt(zz+1d0) * 3d0 */
/*       eps   = guess */
/*       epsrel= 1d-3 */
/*       errest= 0d0 */
/*       irule = 1 */
/*       call DQDAG(inVesicle4a,bmin,bmax,eps,epsrel,irule,aint,er
rest) */
	    ret_val = (real) aint;
	}

    }
    return ret_val;
} /* th36_ */


/* ------------------------------------------------------------- */

doublereal invesicle4a_(doublereal * r0)
{
    /* System generated locals */
    integer i__1;
    doublereal ret_val, d__1, d__2;

    /* Builtin functions */
    //double exp();

    /* Local variables */
    static integer n1, n2, n3;
    static doublereal sw, wt;

/* Radius einer Schicht (verteilt) */
/* Q-Vektor */
/* Konzentration Polymer */
/* Anzahl der Schichten */
/* width of n-distribution */
/* Kontrast der zentralen Schicht (1) */
/* Dicke    der zentralen Schicht (1) */
/* Main Radius */
/* Z - Schultz distribution */
/* Maximum possible shift */
/* background */
/* n-variables */
/* weights */
/* -------------------------------------------- */
    if (vespar4_1.nw < .5) {
	n1 = (integer) vespar4_1.n;
	d__1 = (doublereal) n1;
	d__2 = (doublereal) n1 + 1.;
	ret_val = (n1 + 1. - vespar4_1.n) * invesicle4_(r0, &d__1) + (
		vespar4_1.n - n1) * invesicle4_(r0, &d__2);
    } else {
/* Computing MAX */
	i__1 = (integer) (vespar4_1.n - vespar4_1.nw * 2.);
	n1 = max(i__1,1);
	n2 = (integer) (vespar4_1.n + vespar4_1.nw * 2. + 1.);
	ret_val = 0.;
	sw = 0.;
	i__1 = n2;
	for (n3 = n1; n3 <= i__1; ++n3) {
	    wt = exp((n3 - vespar4_1.n) * -.5 * (n3 - vespar4_1.n) / (
		    vespar4_1.nw * vespar4_1.nw));
	    d__1 = (doublereal) n3;
	    ret_val += wt * invesicle4_(r0, &d__1);
	    sw += wt;
	}
	ret_val /= sw;
    }
    return ret_val;
}


/* ------------------------------------------------------------- */

doublereal invesicle4_(doublereal * r0, doublereal * n)
{
    /* System generated locals */
    doublereal ret_val, d__1, d__2, d__3, d__4;

    /* Builtin functions */
    //double exp(), cos(), sin(), pow_dd(), tan(), log();

    /* Local variables */
    static doublereal qmod, keps, vorf, qhpt;
    static doublereal a[31], b[31], k;
    static doublereal delta, k2, k3, qa, az, fv, nn, sv, ke1, ke2, qa2, kn2, 
	    f_r__, k2m1, k2p1;
    static integer ier;
    static doublereal nnn, xpn;

/* Radius einer Schicht (verteilt) */
/* number of layers */
/* Vorfaktor */
/* Formfaktor / sqrt(2Pi) */
/* Strukturfaktor (des geschichteten V */
/* Schultz distribution */
/* Q-Vektor */
/* Konzentration Polymer */
/* Anzahl der Schichten */
/* width of n-distribution */
/* Kontrast der zentralen Schicht (1) */
/* Dicke    der zentralen Schicht (1) */
/* Main Radius */
/* Z - Schultz distribution */
/* Maximum possible shift */
/* background */
/* Abstand Dekoration - zentrale Schic */
/* Arguments */
/* fractions of qarg (mod Pi) */
/* inner structure factor */
/* K^2, K^2-1, K^(n+2) */
/* K^3, K^2+1 */
/* n^2, n^3 */
/* Coefficients of sin-oszillations */
/* arguments    of sin-oszillations */
/* Schultz distribution, error */
/* Schultz distribution, Z+1 */
/* exponent, f_r=exp(xpn) */
/* Gamma */
/* lnGamma */
/* function to calculate sum of sin-os */
/* -------------------------------------------- */
    vorf = vespar4_2.phi * 4e-24 * 3.14159265358979323846264338328 * *r0 * *
	    r0 * 6. / (((*n * 2. + 3.) * *n + 1.) * *n);
    delta = (vespar4_2.sig1 + vespar4_2.sig2) * 1.177410023;
/* form factor */
    fv = vespar4_2.drho1 * vespar4_2.sig1 * exp(vespar4_2.q * -.5 * 
	    vespar4_2.q * vespar4_2.sig1 * vespar4_2.sig1) + vespar4_2.drho2 *
	     vespar4_2.sig2 * exp(vespar4_2.q * -.5 * vespar4_2.q * 
	    vespar4_2.sig2 * vespar4_2.sig2) * 2. * cos(delta * vespar4_2.q);
    qa = (d__1 = vespar4_2.q * *r0, abs(d__1));
/* structure factor */
    qa2 = (d__1 = vespar4_2.q * vespar4_2.rshift * (*r0 / vespar4_2.rmain), 
	    abs(d__1));
    k = (sin(qa2) - qa2 * cos(qa2)) * 3. / (qa2 * qa2 * qa2);
/* inner structu */
    k2 = k * k;
    k3 = k * k2;
    k2m1 = k2 - 1.;
    k2p1 = k2 + 1.;
    d__1 = *n + 2.;
    kn2 = pow_dd(&k, &d__1);
    keps = .001;
    ke1 = keps * 420. / 36. * (((((*n * 4. + 12.) * *n + 13.) * *n + 6.) * *n 
	    + 1.) * *n * *n) / ((((((*n * 10. + 36.) * *n + 21.) * *n - 35.) *
	     *n - 35.) * *n * *n + 4.) * *n);
    ke2 = keps * .5 * ((((*n * 6. + 15.) * *n + 10.) * *n * *n - 1.) * *n) / (
	    ((*n * 6. * *n - 10.) * *n * *n + 4.) * *n);
    qhpt = (integer) (qa / 3.14159265358979323846264338328 + .5) * 
	    3.14159265358979323846264338328;
/* mod Pi */
    qmod = qa - qhpt;
    if (abs(qmod) < 1e-6) {
	if (qmod < 0.) {
	    qmod = -1e-6;
	} else {
	    qmod = 1e-6;
	}
    }
    qa = qhpt + qmod;
    nn = *n * *n;
    nnn = nn * *n;
    if (abs(k) <= ke2) {
	d__1 = sin(qa);
	d__2 = *n + 1.;
	d__3 = *n + 1.;
	d__4 = sin(qa);
	sv = (*n * (*n + .5) * (*n + 1.) / 6. + (cos(qa * 2. * (*n + 1.)) * (
		-(*n + 1.) / pow_dd(&d__1, &c_b22) + pow_dd(&d__2, &c_b22)) + 
		sin(qa * 2. * (*n + 1.)) * (-pow_dd(&d__3, &c_b22) + .5 / 
		pow_dd(&d__4, &c_b22)) / tan(qa)) * .25) / (qa * qa);
    } else {
	if (k < 1. - ke1) {
/*2345678901234567890123456789012345678901234567890123456789012345
678901234567xxxxxxxxx*/
	    a[1] = k2m1 * -6. * k2p1 * ((k2 + 5.) * k2 + 1.) * nn + ((((k2 * 
		    -6. - 12.) * k2 + 48.) * k2 + 48.) * k2 + 6.) * *n + ((((
		    k2 * 3. + 36.) * k2 + 24.) * k2 - 18.) * k2 - 3.);
	    b[1] = qa * (*n * 2. + 1.);
	    a[2] = ((((k2 * 3. - 12.) * k2 - 45.) * k2 - 24.) * k2 - 3.) * nn 
		    + ((((k2 * 6. - 12.) * k2 - 72.) * k2 - 48.) * k2 - 6.) * 
		    *n + ((((k2 * 3. + 18.) * k2 - 24.) * k2 - 36.) * k2 - 3.)
		    ;
	    b[2] = qa * (*n * 2. - 1.);
	    a[5] = ((((k2 * 3. + 24.) * k2 + 45.) * k2 + 12.) * k2 - 3.) * nn 
		    + k2 * 6. * (k2 * 3. + 2.) * *n + k2 * 3. * ((k2 * 4. - 
		    1.) * k2 - 6.);
	    b[5] = qa * (*n * 2. + 3.);
	    a[6] = k2 * 18. * k2 * k2p1 * nn + k2 * 6. * ((k2 * 6. + 3.) * k2 
		    - 2.) * *n + k2 * 3. * ((k2 * 6. + 1.) * k2 - 4.);
	    b[6] = qa * (*n * 2. - 3.);
	    a[9] = k2 * -18. * k2p1 * nn - k2 * 6. * k2 * (k2 * 2. + 3.) * *n 
		    - k2 * 3. * k2;
	    b[9] = qa * (*n * 2. + 5.);
	    a[10] = k2 * 3. * k2 * nn + k2 * 6. * k2 * *n + k2 * 3. * k2;
	    b[10] = qa * (*n * 2. - 5.);
	    a[12] = k2 * -3. * k2 * nn;
	    b[12] = qa * (*n * 2. + 7.);
	    a[25] = k2m1 * 6. * k2p1 * ((k2 + 4.) * k2 + 1.) * nnn + ((k2 - 
		    4.) * k2 - 3.) * 3. * ((k2 * 3. + 8.) * k2 + 1.) * nn + ((
		    ((k2 * 3. - 36.) * k2 - 120.) * k2 - 60.) * k2 - 3.) * *n 
		    + k2 * 42. * ((k2 - 4.) * k2 + 1.);
	    b[25] = qa;
	    a[27] = k2m1 * -2. * k2p1 * (k2m1 * k2 + 1.) * nnn + (((k2 * -3. 
		    + 9.) * k2 * k2 + 3.) * k2 + 3.) * nn + (((-k2 + 7.) * k2 
		    * k2 + 5.) * k2 + 1.) * *n + k2 * 6. * ((k2 * -4. + 11.) *
		     k2 - 4.);
	    b[27] = qa * 3.;
	    a[29] = k2 * -6. * k2m1 * k2p1 * nnn - k2 * 3. * ((k2 - 8.) * k2 
		    - 5.) * nn + k2 * 3. * ((k2 + 8.) * k2 + 3.) * *n + k2 * 
		    6. * (k2m1 * k2 + 1.);
	    b[29] = qa * 5.;
	    a[0] = k3 * 6. * ((k2 * 3. + 10.) * k2 + 5.) * nn + k3 * 6. * ((
		    k2 * 3. + 8.) * k2 + 3.) * *n - k * 12. * k2m1 * ((k2 + 
		    3.) * k2 + 1.);
	    b[0] = qa * 2. * *n;
	    a[4] = k * (((k2 * -12. - 12.) * k2 + 12.) * k2 + 6.) * nn - k * 
		    6. * (k2 * 2. + 3) * (k2m1 * 2. * k2 - 1.) * *n + k * ((
		    k2p1 * -12. * k2 + 36.) * k2 + 12.);
	    b[4] = qa * 2. * (*n - 1.);
	    a[3] = k * ((k2 * -30. - 60.) * k2 - 18.) * nn + k * ((k2 * -42. 
		    - 72.) * k2 - 18.) * *n + k * (((k2 * -12. - 36.) * k2 + 
		    12.) * k2 + 12.);
	    b[3] = qa * 2. * (*n + 1.);
	    a[8] = k3 * -6. * (k2 * 2 + 1) * nn - k3 * 6. * (k2 * 4 + 1) * *n 
		    - k3 * 12. * k2;
	    b[8] = qa * 2. * (*n - 2.);
	    a[7] = k * -6. * (((k2 + 2.) * k2 - 2.) * k2 - 2.) * nn + k3 * 6. 
		    * ((k2 + 4.) * k2 + 2.) * *n + k3 * 12.;
	    b[7] = qa * 2. * (*n + 2.);
	    a[11] = k3 * 6. * (k2 + 2.) * nn + k3 * 6. * k2 * *n;
	    b[11] = qa * 2. * (*n + 3.);
	    a[26] = k * -6. * k2m1 * (k2 * 2. + 1.) * (k2 + 2.) * nnn + k * ((
		    (k2 * -12. + 48.) * k2 + 102.) * k2 + 24.) * nn + k * ((
		    k2 * 66. + 84.) * k2 + 12.) * *n + k3 * 24. * k2p1;
	    b[26] = qa * 2.;
	    a[28] = k * 6. * k2m1 * k2p1 * k2p1 * nnn + k * 6. * k2p1 * ((k2 
		    - 5.) * k2 - 2.) * nn - k * 6. * k2p1 * (k2 * 5. + 1.) * *
		    n - k3 * 12. * k2p1;
	    b[28] = qa * 4.;
	    a[30] = k3 * 2. * k2m1 * nnn - k3 * 6. * nn - k3 * 2. * (k2 + 2.) 
		    * *n;
	    b[30] = qa * 6.;
	    a[13] = kn2 * k2m1 * k * (-72. - *n * 12. * (k2 * 3. + 4.));
	    b[13] = qa * *n;
	    a[15] = kn2 * k2m1 * ((k2 * 3. - 2.) * 12. - *n * 12. * (k2 + 2.))
		    ;
	    b[15] = qa * (*n - 1.);
	    a[14] = kn2 * k2m1 * ((k2 * 2. - 3.) * -12. + *n * 12. * (k2 * 4. 
		    + 3.));
	    b[14] = qa * (*n + 1.);
	    a[17] = kn2 * k2m1 * k * (*n * 6. * (k2 * 4. + 7.) + 48.);
	    b[17] = qa * (*n - 2.);
	    a[16] = kn2 * k2m1 * k * (*n * 12. * (k2 * 2. + 1.) + 48.);
	    b[16] = qa * (*n + 2.);
	    a[19] = kn2 * k2m1 * ((k2 * 4. - 1.) * -6. - *n * 6. * (k2 * 2. - 
		    1.));
	    b[19] = qa * (*n - 3.);
	    a[18] = kn2 * k2m1 * ((k2 - 4.) * 6. - *n * 6. * (k2 * 7. + 4.));
	    b[18] = qa * (*n + 3.);
	    a[21] = kn2 * k2m1 * k * (-12. - *n * 6. * (k2 + 2.));
	    b[21] = qa * (*n - 4.);
	    a[20] = kn2 * k2m1 * k * (-12. - *n * 6. * (k2 - 2.));
	    b[20] = qa * (*n + 4.);
	    a[23] = kn2 * k2m1 * k2 * 6. * (*n + 1.);
	    b[23] = qa * (*n - 5.);
	    a[22] = kn2 * k2m1 * (*n * 6. * (k2 * 2. + 1.) + 6.);
	    b[22] = qa * (*n + 5.);
	    a[24] = kn2 * k2m1 * k * (*n * -6.);
	    b[24] = qa * (*n + 6.);
	    d__1 = sin(qa);
	    d__2 = k2p1 - k * 2. * cos(qa);
	    sv = oszi_(a, b) / (pow_dd(&d__1, &c_b26) * -48. * pow_dd(&d__2, &
		    c_b27) * qa * qa);
	} else {
	    sv = (cos(qa * (*n + .5)) * -.5 * (*n + .5) + sin(qa * (*n + .5)) 
		    * .25 / tan(qa * .5)) / (qa * sin(qa * .5));
	    sv *= sv;
	}
    }
    if (vespar4_2.zz <= -.99) {
	f_r__ = 1.;
    } else {
	ier = -1;
/* Schultz distribution */
	az = vespar4_2.zz + 1.;
	if (az <= 10.) {
	    d__1 = az * *r0 / vespar4_2.rmain;
	    f_r__ = pow_dd(&d__1, &az) / *r0 * exp(-az * *r0 / 
		    vespar4_2.rmain) / s14aaf_(&az, &ier);
	} else {
	    xpn = az * log(az) + vespar4_2.zz * log(*r0) - az * log(
		    vespar4_2.rmain) - s14abf_(&az, &ier) - az * *r0 / 
		    vespar4_2.rmain;
	    xpn = min(37.,xpn);
	    xpn = max(-37.,xpn);
	    f_r__ = exp(xpn);
	}
    }
    ret_val = (vorf * 2. * 3.14159265358979323846264338328 * fv * fv * sv + 
	    vespar4_2.bckgr) * f_r__;
    return ret_val;
}

doublereal oszi_(doublereal * a, doublereal * b)
{
    /* System generated locals */
    doublereal ret_val, d__1;

    /* Local variables */
    static integer case__[31], qitv;
    static doublereal c__[31];
    static integer i__, j, k;
    static doublereal t[20], ca[31], sa[31], cs[31], ss[31], fac;
    static integer sgn;
    static doublereal sgn2;

/* Amplitudes */
/* Arguments */
/* loops */
/* sign */
/* interval */
/* sign */
/* B(i) mod Pi/2 */
/* case sin/cos 0/1 */
/* Cosine values (power of */
/* Sine   values (power of */
/* Terms */
/*     double precision min                    ! minimum C */
/*     double precision KA,KB,KC,KCa,KCs,KSa,KSs ! copied values */
/*     integer          KCase */
/* faculty */
    /* Parameter adjustments */
    --b;
    --a;

    /* Function Body */
    for (i__ = 1; i__ <= 31; ++i__) {
	sgn = 1;
	if (b[i__] < 0.) {
	    sgn = -1;
	}
	b[i__] = (d__1 = b[i__], abs(d__1));
	qitv = (integer) (b[i__] * 2. / 3.14159265358979323846264338328 + .5);
	c__[i__ - 1] = b[i__] - qitv * 1.5707963267948966;
/* arg mod Pi/2 */
	sgn2 = 1. - (qitv / 2 - (qitv / 4 << 1)) * 2.;
/* sign */
	case__[i__ - 1] = qitv - (qitv / 2 << 1);
/* case sin/cos 0/1 */
	ca[i__ - 1] = 1.;
	sa[i__ - 1] = c__[i__ - 1];
	cs[i__ - 1] = sgn2 * sgn;
	ss[i__ - 1] = sgn2 * sgn;
    }
/*      do i=1,30 */
/*        k  = i */
/*        min= 1d2 */
/*        do j=i,31 */
/*          if(abs(C(j)).lt.min) then */
/*            k=j */
/*            min=abs(C(j)) */
/*          endif */
/*        enddo */
/*        KA = A(i) */
/*        KB = B(i) */
/*        KC = C(i) */
/*        KCa= Ca(i) */
/*        KCs= Cs(i) */
/*        KSa= Sa(i) */
/*        KSs= Ss(i) */
/*        KCase = Case(i) */
/*        A(i) = A(k) */
/*        B(i) = B(k) */
/*        C(i) = C(k) */
/*        Ca(i)= Ca(k) */
/*        Cs(i)= Cs(k) */
/*        Sa(i)= Sa(k) */
/*        Ss(i)= Ss(k) */
/*        Case(i) = Case(k) */
/*        A(k) = KA */
/*        B(k) = KB */
/*        C(k) = KC */
/*        Ca(k)= KCa */
/*        Cs(k)= KCs */
/*        Sa(k)= KSa */
/*        Ss(k)= KSs */
/*        Case(k) = KCase */
/*      enddo */
    fac = 1.;
    j = 1;
    for (k = 0; k <= 9; ++k) {
	t[j - 1] = 0.;
	for (i__ = 1; i__ <= 31; ++i__) {
	    if (case__[i__ - 1] == 1) {
		t[j - 1] += cs[i__ - 1] * ca[i__ - 1] * a[i__];
		ca[i__ - 1] *= c__[i__ - 1] * c__[i__ - 1];
		cs[i__ - 1] = -cs[i__ - 1];
	    }
	}
	t[j - 1] /= fac;
	fac *= j;
	++j;
	t[j - 1] = 0.;
	for (i__ = 1; i__ <= 31; ++i__) {
	    if (case__[i__ - 1] == 0) {
		t[j - 1] += ss[i__ - 1] * sa[i__ - 1] * a[i__];
		sa[i__ - 1] *= c__[i__ - 1] * c__[i__ - 1];
		ss[i__ - 1] = -ss[i__ - 1];
	    }
	}
	t[j - 1] /= fac;
	fac *= j;
	++j;
    }
    ret_val = 0.;
    for (j = 1; j <= 20; ++j) {
	ret_val += t[j - 1];
    }
    return ret_val;
}

doublereal s14abf_(doublereal * x, integer * ifail)
{

    /* Local variables */
    real pda_lngam__;

    if (*x <= 0.) {
	*ifail = 1;
	pda_lngam__ = (float)0.;
    } else if (*x > 2.55e305) {
	*ifail = 2;
	pda_lngam__ = (float)0.;
    } else {
	*ifail = 0;
	pda_lngam__ = (real)dlgama_(x);
    }
    return pda_lngam__;
}

/* S    REAL FUNCTION ALGAMA(X) */
doublereal dlgama_(doublereal * x)
{
    /* Initialized data */

    static doublereal one = 1.;
    static doublereal half = .5;
    static doublereal twelve = 12.;
    static doublereal zero = 0.;
    static doublereal four = 4.;
    static doublereal thrhal = 1.5;
    static doublereal two = 2.;
    static doublereal pnt68 = .6796875;
    static doublereal sqrtpi = .9189385332046727417803297;
    static doublereal xbig = 2.55e305;
    static doublereal xinf = 1.79e308;
    static doublereal eps = 2.22e-16;
    static doublereal frtbig = 2.25e76;
    static doublereal d1 = -.5772156649015328605195174;
    static doublereal p1[8] = { 4.945235359296727046734888,
	    201.8112620856775083915565,2290.838373831346393026739,
	    11319.67205903380828685045,28557.24635671635335736389,
	    38484.96228443793359990269,26377.48787624195437963534,
	    7225.813979700288197698961 };
    static doublereal q1[8] = { 67.48212550303777196073036,
	    1113.332393857199323513008,7738.757056935398733233834,
	    27639.87074403340708898585,54993.10206226157329794414,
	    61611.22180066002127833352,36351.27591501940507276287,
	    8785.536302431013170870835 };
    static doublereal d2 = .4227843350984671393993777;
    static doublereal p2[8] = { 4.974607845568932035012064,
	    542.4138599891070494101986,15506.93864978364947665077,
	    184793.2904445632425417223,1088204.76946882876749847,
	    3338152.967987029735917223,5106661.678927352456275255,
	    3074109.054850539556250927 };
    static doublereal q2[8] = { 183.0328399370592604055942,
	    7765.049321445005871323047,133190.3827966074194402448,
	    1136705.821321969608938755,5267964.117437946917577538,
	    13467014.54311101692290052,17827365.30353274213975932,
	    9533095.591844353613395747 };
    static doublereal d4 = 1.791759469228055000094023;
    static doublereal p4[8] = { 14745.02166059939948905062,
	    2426813.369486704502836312,121475557.4045093227939592,
	    2663432449.630976949898078,29403789566.34553899906876,
	    170266573776.5398868392998,492612579337.743088758812,
	    560625185622.3951465078242 };
    static doublereal q4[8] = { 2690.530175870899333379843,
	    639388.5654300092398984238,41355999.30241388052042842,
	    1120872109.61614794137657,14886137286.78813811542398,
	    101680358627.2438228077304,341747634550.7377132798597,
	    446315818741.9713286462081 };
    static doublereal c__[7] = { -.001910444077728,8.4171387781295e-4,
	    -5.952379913043012e-4,7.93650793500350248e-4,
	    -.002777777777777681622553,.08333333333333333331554247,
	    .0057083835261 };

    /* System generated locals */
    doublereal ret_val;

    /* Builtin functions */
    //double log();

    /* Local variables */
    static doublereal xden, corr, xnum;
    static integer i__;
    static doublereal y, xm1, xm2, xm4, res, ysq;

/* ---------------------------------------------------------------------- 
*/

/* This routine calculates the LOG(GAMMA) function for a positive real */
/*   argument X.  Computation is based on an algorithm outlined in */
/*   references 1 and 2.  The program uses rational functions that */
/*   theoretically approximate LOG(GAMMA) to at least 18 significant */
/*   decimal digits.  The approximation for X > 12 is from reference */
/*   3, while approximations for X < 12.0 are similar to those in */
/*   reference 1, but are unpublished.  The accuracy achieved depends */
/*   on the arithmetic system, the compiler, the intrinsic functions, */
/*   and proper selection of the machine-dependent constants. */


/* ********************************************************************* 
*/
/* ********************************************************************* 
*/

/* Explanation of machine-dependent constants */

/* beta   - radix for the floating-point representation */
/* maxexp - the smallest positive power of beta that overflows */
/* XBIG   - largest argument for which LN(GAMMA(X)) is representable */
/*          in the machine, i.e., the solution to the equation */
/*                  LN(GAMMA(XBIG)) = beta**maxexp */
/* XINF   - largest machine representable floating-point number; */
/*          approximately beta**maxexp. */
/* EPS    - The smallest positive floating-point number such that */
/*          1.0+EPS .GT. 1.0 */
/* FRTBIG - Rough estimate of the fourth root of XBIG */


/*     Approximate values for some important machines are: */

/*                           beta      maxexp         XBIG */

/* CRAY-1        (S.P.)        2        8191       9.62E+2461 */
/* Cyber 180/855 */
/*   under NOS   (S.P.)        2        1070       1.72E+319 */
/* IEEE (IBM/XT, */
/*   SUN, etc.)  (S.P.)        2         128       4.08E+36 */
/* IEEE (IBM/XT, */
/*   SUN, etc.)  (D.P.)        2        1024       2.55D+305 */
/* IBM 3033      (D.P.)       16          63       4.29D+73 */
/* VAX D-Format  (D.P.)        2         127       2.05D+36 */
/* VAX G-Format  (D.P.)        2        1023       1.28D+305 */


/*                           XINF        EPS        FRTBIG */

/* CRAY-1        (S.P.)   5.45E+2465   7.11E-15    3.13E+615 */
/* Cyber 180/855 */
/*   under NOS   (S.P.)   1.26E+322    3.55E-15    6.44E+79 */
/* IEEE (IBM/XT, */
/*   SUN, etc.)  (S.P.)   3.40E+38     1.19E-7     1.42E+9 */
/* IEEE (IBM/XT, */
/*   SUN, etc.)  (D.P.)   1.79D+308    2.22D-16    2.25D+76 */
/* IBM 3033      (D.P.)   7.23D+75     2.22D-16    2.56D+18 */
/* VAX D-Format  (D.P.)   1.70D+38     1.39D-17    1.20D+9 */
/* VAX G-Format  (D.P.)   8.98D+307    1.11D-16    1.89D+76 */

/* ************************************************************** */
/* ************************************************************** */

/* Error returns */

/*  The program returns the value XINF for X .LE. 0.0 or when */
/*     overflow would occur.  The computation is believed to */
/*     be free of underflow and overflow. */


/* Intrinsic functions required are: */

/*      LOG */


/* References: */

/*  1) W. J. Cody and K. E. Hillstrom, 'Chebyshev Approximations for */
/*     the Natural Logarithm of the Gamma Function,' Math. Comp. 21, */
/*     1967, pp. 198-203. */

/*  2) K. E. Hillstrom, ANL/AMD Program ANLC366S, DGAMMA/DLGAMA, May, */
/*     1969. */

/*  3) Hart, Et. Al., Computer Approximations, Wiley and sons, New */
/*     York, 1968. */


/*  Authors: W. J. Cody and L. Stoltz */
/*           Argonne National Laboratory */

/*  Latest modification: June 16, 1988 */

/* ---------------------------------------------------------------------- 
*/
/* S    REAL */
/* ---------------------------------------------------------------------- 
*/
/*  Mathematical constants */
/* ---------------------------------------------------------------------- 
*/
/* S    DATA ONE,HALF,TWELVE,ZERO/1.0E0,0.5E0,12.0E0,0.0E0/, */
/* S   1     FOUR,THRHAL,TWO,PNT68/4.0E0,1.5E0,2.0E0,0.6796875E0/, */
/* S   2     SQRTPI/0.9189385332046727417803297E0/ */
/* ---------------------------------------------------------------------- 
*/
/*  Machine dependent parameters */
/* ---------------------------------------------------------------------- 
*/
/* S    DATA XBIG,XINF,EPS,FRTBIG/4.08E36,3.401E38,1.19E-7,1.42E9/ */
/* ---------------------------------------------------------------------- 
*/
/*  Numerator and denominator coefficients for rational minimax */
/*     approximation over (0.5,1.5). */
/* ---------------------------------------------------------------------- 
*/
/* S    DATA D1/-5.772156649015328605195174E-1/ */
/* S    DATA P1/4.945235359296727046734888E0,2.018112620856775083915565E2,
 */
/* S   1        2.290838373831346393026739E3,1.131967205903380828685045E4,
 */
/* S   2        2.855724635671635335736389E4,3.848496228443793359990269E4,
 */
/* S   3        2.637748787624195437963534E4,7.225813979700288197698961E3/
 */
/* S    DATA Q1/6.748212550303777196073036E1,1.113332393857199323513008E3,
 */
/* S   1        7.738757056935398733233834E3,2.763987074403340708898585E4,
 */
/* S   2        5.499310206226157329794414E4,6.161122180066002127833352E4,
 */
/* S   3        3.635127591501940507276287E4,8.785536302431013170870835E3/
 */
/* ---------------------------------------------------------------------- 
*/
/*  Numerator and denominator coefficients for rational minimax */
/*     Approximation over (1.5,4.0). */
/* ---------------------------------------------------------------------- 
*/
/* S    DATA D2/4.227843350984671393993777E-1/ */
/* S    DATA P2/4.974607845568932035012064E0,5.424138599891070494101986E2,
 */
/* S   1        1.550693864978364947665077E4,1.847932904445632425417223E5,
 */
/* S   2        1.088204769468828767498470E6,3.338152967987029735917223E6,
 */
/* S   3        5.106661678927352456275255E6,3.074109054850539556250927E6/
 */
/* S    DATA Q2/1.830328399370592604055942E2,7.765049321445005871323047E3,
 */
/* S   1        1.331903827966074194402448E5,1.136705821321969608938755E6,
 */
/* S   2        5.267964117437946917577538E6,1.346701454311101692290052E7,
 */
/* S   3        1.782736530353274213975932E7,9.533095591844353613395747E6/
 */
/* ---------------------------------------------------------------------- 
*/
/*  Numerator and denominator coefficients for rational minimax */
/*     Approximation over (4.0,12.0). */
/* ---------------------------------------------------------------------- 
*/
/* S    DATA D4/1.791759469228055000094023E0/ */
/* S    DATA P4/1.474502166059939948905062E4,2.426813369486704502836312E6,
 */
/* S   1        1.214755574045093227939592E8,2.663432449630976949898078E9,
 */
/* S   2      2.940378956634553899906876E10,1.702665737765398868392998E11,
 */
/* S   3      4.926125793377430887588120E11,5.606251856223951465078242E11/
 */
/* S    DATA Q4/2.690530175870899333379843E3,6.393885654300092398984238E5,
 */
/* S   2        4.135599930241388052042842E7,1.120872109616147941376570E9,
 */
/* S   3      1.488613728678813811542398E10,1.016803586272438228077304E11,
 */
/* S   4      3.417476345507377132798597E11,4.463158187419713286462081E11/
 */
/* ---------------------------------------------------------------------- 
*/
/*  Coefficients for minimax approximation over (12, INF). */
/* ---------------------------------------------------------------------- 
*/
/* S    DATA C/-1.910444077728E-03,8.4171387781295E-04, */
/* S   1     -5.952379913043012E-04,7.93650793500350248E-04, */
/* S   2     -2.777777777777681622553E-03,8.333333333333333331554247E-02, 
*/
/* S   3      5.7083835261E-03/ */
/* ---------------------------------------------------------------------- 
*/
    y = *x;
    if (y > zero && y <= xbig) {
	if (y <= eps) {
	    res = -log(y);
	} else if (y <= thrhal) {
/* --------------------------------------------------------------
-------- */
/*  EPS .LT. X .LE. 1.5 */
/* --------------------------------------------------------------
-------- */
	    if (y < pnt68) {
		corr = -log(y);
		xm1 = y;
	    } else {
		corr = zero;
		xm1 = y - half - half;
	    }
	    if (y <= half || y >= pnt68) {
		xden = one;
		xnum = zero;
		for (i__ = 1; i__ <= 8; ++i__) {
		    xnum = xnum * xm1 + p1[i__ - 1];
		    xden = xden * xm1 + q1[i__ - 1];
/* L140: */
		}
		res = corr + xm1 * (d1 + xm1 * (xnum / xden));
	    } else {
		xm2 = y - half - half;
		xden = one;
		xnum = zero;
		for (i__ = 1; i__ <= 8; ++i__) {
		    xnum = xnum * xm2 + p2[i__ - 1];
		    xden = xden * xm2 + q2[i__ - 1];
/* L220: */
		}
		res = corr + xm2 * (d2 + xm2 * (xnum / xden));
	    }
	} else if (y <= four) {
/* --------------------------------------------------------------
-------- */
/*  1.5 .LT. X .LE. 4.0 */
/* --------------------------------------------------------------
-------- */
	    xm2 = y - two;
	    xden = one;
	    xnum = zero;
	    for (i__ = 1; i__ <= 8; ++i__) {
		xnum = xnum * xm2 + p2[i__ - 1];
		xden = xden * xm2 + q2[i__ - 1];
/* L240: */
	    }
	    res = xm2 * (d2 + xm2 * (xnum / xden));
	} else if (y <= twelve) {
/* --------------------------------------------------------------
-------- */
/*  4.0 .LT. X .LE. 12.0 */
/* --------------------------------------------------------------
-------- */
	    xm4 = y - four;
	    xden = -one;
	    xnum = zero;
	    for (i__ = 1; i__ <= 8; ++i__) {
		xnum = xnum * xm4 + p4[i__ - 1];
		xden = xden * xm4 + q4[i__ - 1];
/* L340: */
	    }
	    res = d4 + xm4 * (xnum / xden);
	} else {
/* --------------------------------------------------------------
-------- */
/*  Evaluate for argument .GE. 12.0, */
/* --------------------------------------------------------------
-------- */
	    res = zero;
	    if (y <= frtbig) {
		res = c__[6];
		ysq = y * y;
		for (i__ = 1; i__ <= 6; ++i__) {
		    res = res / ysq + c__[i__ - 1];
/* L450: */
		}
	    }
	    res /= y;
	    corr = log(y);
	    res = res + sqrtpi - half * corr;
	    res += y * (corr - one);
	}
    } else {
/* ------------------------------------------------------------------
---- */
/*  Return for bad arguments */
/* ------------------------------------------------------------------
---- */
	res = xinf;
    }
/* ---------------------------------------------------------------------- 
*/
/*  Final adjustments and return */
/* ---------------------------------------------------------------------- 
*/
/* S    ALGAMA = RES */
    ret_val = res;
    return ret_val;
/* ---------- Last line of DLGAMA ---------- */
} /* dlgama_ */

doublereal s14aaf_(doublereal * x, integer * ifail)
{
    /* Initialized data */

    static doublereal xbig = 57.;
    static doublereal gbig = 7.1e74;
    static doublereal xminv = 1.4e-76;
    static struct {
	char e_1[8];
	doublereal e_2;
	} equiv_108 = { {' ', 'S', '1', '4', 'A', 'A', 'F', ' '}, 0. };

#define srname (*(doublereal *)&equiv_108)

    static doublereal xsmall = 1e-17;

    /* System generated locals */
    integer i__1;
    doublereal ret_val, d__1;

    /* Builtin functions */
    //double d_sign();

    /* Local variables */
    static doublereal g;
    static integer i__, m;
    static doublereal t, y;

/*     GAMMA FUNCTION */
    t = abs(*x);
    if (t > xbig) {
	goto L160;
    }
    if (t <= xsmall) {
	goto L140;
    }
    m = (integer) (*x);
    if (*x < 0.) {
	goto L80;
    }
    t = *x - (doublereal) m;
    --m;
    g = 1.;
    if (m < 0) {
	goto L20;
    } else if (m == 0) {
	goto L120;
    } else {
	goto L40;
    }
L20:
    g /= *x;
    goto L120;
L40:
    i__1 = m;
    for (i__ = 1; i__ <= i__1; ++i__) {
	g *= *x - (doublereal) i__;
/* L60: */
    }
    goto L120;
L80:
    t = *x - (doublereal) (m - 1);
    if (t == 1.) {
	goto L220;
    }
    m = 1 - m;
    g = *x;
    i__1 = m;
    for (i__ = 1; i__ <= i__1; ++i__) {
	g *= *x + (doublereal) i__;
/* L100: */
    }
    g = 1. / g;
L120:
    t = t * 2. - 1.;
    y = t * (t * (t * (t * (t * (t * (t * (t * (t * (t * (t * (t * (t * (t * (
	    t * (t * (t * (t * (t * (t * (t * (t * (t * -1.463812096e-11 + 
	    4.265607168e-11) - 4.014997504e-11) + 1.2767985664e-10) - 
	    6.1351395328e-10) + 1.8224316416e-9) - 5.1196133376e-9) + 
	    1.538352152576e-8) - 4.647749271552e-8) + 1.3938352259072e-7) - 
	    4.1780877635584e-7) + 1.25281466396672e-6) - 3.75499034136576e-6) 
	    + 1.125246429755904e-5) - 3.363758332402688e-5) + 
	    1.0092814882336512e-4) - 2.968901216332e-4) + 
	    9.1578599728893312e-4) - .00242259538436268176) + 
	    .00904033494028101968) - .0134118505705967765) + 
	    .103703363422075456) + .0161691987244425092) + 
	    .886226925452758013;
    ret_val = y * g;
    *ifail = 0;
    goto L240;
L140:
    if (t < xminv) {
	goto L200;
    }
    ret_val = 1. / *x;
    *ifail = 0;
    goto L240;
L160:
    if (*x < 0.) {
	goto L180;
    }
    *ifail = p01aaf_(ifail, &c__1, &srname);
    ret_val = gbig;
    goto L240;
L180:
    *ifail = p01aaf_(ifail, &c__2, &srname);
    ret_val = 0.;
    goto L240;
L200:
    *ifail = p01aaf_(ifail, &c__3, &srname);
    t = *x;
    if (*x == 0.) {
	t = 1.;
    }
    d__1 = 1. / xminv;
    ret_val = d_sign(&d__1, &t);
    goto L240;
L220:
    *ifail = p01aaf_(ifail, &c__4, &srname);
    ret_val = gbig;
L240:
    return ret_val;
}

#undef srname


integer p01aaf_(integer * ifail, integer * error, doublereal * srname)
{
    /* Format strings */
    static char fmt_99999[] = "(\0020\002,\002ERROR DETECTED BY NAG LIBRARY \
ROUTINE \002,a8,\002 - IFAIL = \002,i5//)";

    /* System generated locals */
    integer ret_val;

    /* Builtin functions */
    //integer s_wsfe(), do_fio(), e_wsfe();
    // int s_stop();

    /* Local variables */
    static integer nout;

    /* Fortran I/O blocks */
    static cilist io___110 = { 0, 0, 0, fmt_99999, 0 };
    static cilist io___111 = { 0, 0, 0, fmt_99999, 0 };


    if (*error == 0) {
	goto L20;
    }
    x04aaf_(&c__0, &nout);
    if (*ifail % 10 == 1) {
	goto L10;
    }
    io___110.ciunit = nout;
    s_wsfe(&io___110);
    do_fio(&c__1, (char *)&(*srname), (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&(*error), (ftnlen)sizeof(integer));
    e_wsfe();
    s_stop("", 0L);
L10:
    if (*ifail / 10 % 10 == 0) {
	goto L20;
    }
    io___111.ciunit = nout;
    s_wsfe(&io___111);
    do_fio(&c__1, (char *)&(*srname), (ftnlen)sizeof(doublereal));
    do_fio(&c__1, (char *)&(*error), (ftnlen)sizeof(integer));
    e_wsfe();
L20:
    ret_val = *error;
    return ret_val;
}

int x04aaf_(integer * i__, integer * nerr)
{
    /* Initialized data */

    static integer nerr1 = 6;

    if (*i__ == 0) {
	*nerr = nerr1;
    }
    if (*i__ == 1) {
	nerr1 = *nerr;
    }
    return 0;
}

