/*
 * src/sasfit_common/include/sasfit_utils_f2c.h
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

#ifndef SASFIT_COMMON_UTILS_F2C_H
#define SASFIT_COMMON_UTILS_F2C_H

#include <sasfit_common.h>
#include <f2c.h>

struct {
	integer p, idistr;
	doublereal atau, btau, tau[200], pi;
} _BLNK__;

#define _BLNK__1 _BLNK__

struct {
	integer li, lj;
} wl_;

#define wl_1 wl_

union {
	struct {
		integer v[20];
		doublereal x[200], x2[200];
	} _1;
	struct {
		integer v[20];
		doublereal la[200], l2[200];
	} _2;
} ve_;

#define ve_1 (ve_._1)
#define ve_2 (ve_._2)

struct {
	doublereal f_value__;
} ltrap_;

#define ltrap_1 ltrap_

union {
	struct {
		doublereal x[20], r__[20];
	} _1;
	struct {
		doublereal x[20], r1[20];
	} _2;
} pa_;

#define pa_1 (pa_._1)
#define pa_2 (pa_._2)

struct {
	doublereal k2, k3, h__, h2, pi12, pi6;
} fu_;

#define fu_1 fu_

struct {
	doublereal r1[400]  /* was [20][20] */,
		   r2[400]     /* was [20][20] */,
		   rr[400]     /* was [20][20] */;
} ri_;

#define ri_1 ri_

struct {
	doublereal b[20];
} be_;

#define be_1 be_

struct {
	doublereal rho[12], dlayer[10], dzeep;
	integer iform, nrho, nlayer;
} deeltje_;

#define deeltje_1 deeltje_

/*
 * functions
 */

// hide direct functions from plugins
#ifndef MAKE_SASFIT_PLUGIN

int sasfit_miev0(real *xx, complex *crefin, logical *perfct, real *mimcut, logical *anyang,
		integer *numang, real *xmu, integer *nmom, integer *ipolzn, integer *momdim,
		logical *prnt, real *qext, real *qsca, real *gqsc, real *pmom, complex *sforw,
		complex *sback, complex *s1, complex *s2, complex *tforw, complex *tback, real *spike);
int sasfit_dmilay(real *rcore, real *rshell, real *wvno, complex *rindsh, complex *rindco, real *mu,
		integer *numang, real *qext, real *qsca, real *qbs, real *gqsc, real *m1, real *m2,
		real *s21, real *d21, integer *maxang);
doublereal sasfit_th36(real * x, sasfit_param * param, char * thnam, char * parnam, integer * npar, integer * ini, ftnlen thnam_len, ftnlen parnam_len);

int sasfit_f2c_errmsg_(char *messag, logical *fatal, ftnlen messag_len);
logical sasfit_f2c_tstbad_(char *varnam, real *relerr, ftnlen varnam_len);
logical sasfit_f2c_wrtbad_(char *varnam, ftnlen varnam_len);
logical sasfit_f2c_wrtdim_(char *dimnam, integer *minval, ftnlen dimnam_len);

doublereal sasfit_struve_h0(doublereal *xvalue);
doublereal sasfit_struve_h1(doublereal *xvalue);

int zspow_(sasfit_func_zspow_t * fcn, integer * nsig, integer * n, integer * itmax, real * par, real * x, real * fnorm, real * wk, integer * ier);

int sasfit_robertus_deal(doublereal * rm, doublereal * sig, doublereal * phi, logical * labdas);
//int sasfit_robertus_show_tau();
int sasfit_robertus_calc(sasfit_param * param, doublereal * rm, doublereal * sig, doublereal * phi);
int sasfit_robertus_subint(sasfit_param * param, doublereal * q, doublereal * ai, doublereal * aint0);
double sasfit_3f2(double a1, double a2, double a3, double b1, double b2, double x, sasfit_param * param);
double sasfit_2f1(double a1, double a2, double b1, double x, sasfit_param * param);
double sasfit_pfq(double *p_r, double *p_i,
                  double *q_r, double *q_i,
                  int ip, int iq,
                  double z_r, double z_i, int ln_pFq, int ix,
                  double *pFq_r, double *pFq_i,
                  int nsigfig, sasfit_param * param);

#endif // MAKE_SASFIT_PLUGIN

#endif

