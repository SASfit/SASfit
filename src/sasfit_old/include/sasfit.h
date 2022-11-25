/*
 * src/sasfit_old/include/sasfit.h
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
 */

/*#######################################################################*/
/*#     initialization  of sas_fit                                      #*/
/*#######################################################################*/
/*# Ver. a.1  06.06.2005  JKcom       sas_fit                           #*/
/*#######################################################################*/

#ifndef _sas_fit_
#define _sas_fit_ 1

#include <tcl.h>
#include <string.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlin.h>
#include "SASFIT_nr.h"
#include "sasfit_oz.h"
#include "sasfit_function.h"

/* enum bool { FALSE = 0, TRUE = 1 }; */
#define float double

/* #define PI M_PI */
#ifndef PI
#define PI 3.1415926536
#endif
#define MAX_DATA_POINTS 5000
#define MAX_SIZE_DISTRIBUTIONS 16
#define INTEGRAL_PRECISION 1e-6
#define NCYLINT 500

/*
struct Gparam {
	sasfit_analytpar *GlobalAP;
    sasfit_commonpar GCP;
    Tcl_Interp *interp;
	int    ma;
	int    mfit,*ndata;
	int    max_SD;
	int    error_type;
	float  **h, **Ih, **DIh, **Ith, **res;
	float  *a;
    int    *lista;
	bool   error;
};
*/

struct extrapolPar {
   float a[MAXPAR];
   float err[MAXPAR];
   float chisq, I0chisq, Porodchisq;
   int   active[MAXPAR];
   int   Guiniernpoints;
   int   Guinierfirst;
   float RG, DRG, R2G, DR2G;
   float I0, DI0;
   float Invariant, DInvariant, Qexp, DQexp, Qc4, DQc4, Qq0, DQq0;
   float intI, DintI, intIexp, DintIexp, intIc4, DintIc4, intIq0, DintIq0;
   float iI, DiI, iIexp, DiIexp, iIc4, DiIc4, iIq0, DiIq0;
   float VP, DVP, RP, DRP;
   float S_V, DS_V;
   float lc, Dlc;
   float li, Dli;
   float Ac,DAc;
   float Qmin;
   float Qmax;
   int   Porodfirst;
   int   Porodnpoints;
   float PorodD, DPorodD;
   int   PorodDFit;
   float c0, Dc0;
   int   c0Fit;
   float c1, Dc1;
   float c4, Dc4;
   int   first;
   int   npoints;
   int   ndata;
   int   error;
   char  name[132];
   char  order[132];
   char  errortype[132];
   char  typestr[132],I0typestr[132];
   char  aname[MAXPAR][132];
};


typedef enum {
        Idendity,
        first_deriv_eps_b,
        first_deriv_eps_e,
        first_deriv,
        second_deriv_DD,
        second_deriv_NN,
        second_deriv_DN,
        second_deriv_ND,
        second_deriv
} sasfit_EM_Lmatrix_definition;

typedef enum {
        Lcorner_o,
        Lcorner2,
        Lcorner_l,
        Lcorner_w,
        Lcorner_wo,
        GCV,
        redchi2,
        manual
} sasfit_EM_opt_Lagrange_param_method;

typedef enum {
        LLS,
        NNLLS
} sasfit_LLS_method;


typedef enum {
        MuCh,
        SDM,
        lin_Reg,
        EM_smoothing,
        EM_constant_prior,
        EM_adaptive_prior
} sasfit_DR_algorithm;


typedef struct
{
	scalar  Rmax;
	scalar  Dmax;
	int     Nshannon;
	scalar  Delta_r;
	scalar  smooth;
	scalar  Entropy;
	scalar  redchi2,Gtest,JSDtest,chi2test;
	scalar  eps;
	scalar  chi2;
	int     maxit;
	scalar  lambda;
	scalar  overrelaxation;
	char    spacing[132];
	char    iteration_scheme[132];
	char    opt_Lagrange_scheme[132];
	char    Lmatrix[132];
	char    smooth_type[132];
	char    FIinv[132];
	int     smooth_bool;
	int     em_weight;
	char    seed[132];
	sasfit_DR_algorithm DR_algorithm;
	sasfit_EM_opt_Lagrange_param_method optLagrange_method;
	sasfit_EM_Lmatrix_definition defL;
	sasfit_LLS_method LLSmethod;
	char    LLS_type[132];
	int     nLagrange;
	double  maxLagrange;
	double  minLagrange;
	double  maxslope;
	int     maxKrylov;
	scalar  dim;
	int     nh, nR;
	scalar  C4, C0;
	double *in, *out, *xwork, **A, **Ared, **Awork, **S, **T3, *Ih4mC4, *prior;
	double *h, *Ih,*Ihred, *Ihwork, *Iexp, *DIh, *Ith, *Ithred, *Ithwork, *r, *dr, *dh;
    gsl_vector *gsl_x,*gsl_dx, *gsl_b, *gsl_S, *gsl_dS;
    gsl_matrix *gsl_A;
    sasfit_oz_root_algorithms root_algorithm;
//        char ct[STRLEN];
} EM_param_t;


//float Par[(MAX_SIZE_DISTRIBUTIONS+1)][3*MAXPAR];
/*
 * if particle_type[i][0] == 0  &&  particle_type[i][1] == 0
 * then the next 7 parameters are defined as
 *                par[i][0] = c4
 *                par[i][1] = D
 *                par[i][2] = c0
 *                par[i][3] = c1
 *                par[i][4] = not used
 *                par[i][5] = not used
 *                par[i][6] = not used
 *                par[i][7] = not used
 *
 * for the i-th size distribution is a LogNorm distribution, i.e.
 * particle_type[i][1] == 1, then the next 7 parameter are defined as
 *                par[i][0] = N
 *                par[i][1] = s
 *                par[i][2] = p
 *                par[i][3] = m
 *                par[i][4] =     \    characteristic lengths of a particle,
 *                par[i][5] =      \   e.g. R1, R2, eta*mu for a spherical shell
 *                par[i][6] =      /   or a,b,c for a parallel epiped, etc.
 *                par[i][7] = eta /    par[i][7] : scattering contrast
 *
 * the parameter error_type defines the way how chi^2 will be calculated
 *      error_type == 0
 *         chi^2 = sum(( y_i^2 - f(x_i)^2 ) / Delta_y_i^2,i=1,n_points)
 *      error_type == 1
 *         chi^2 = sum(ln^2(y_i) - ln^2(f(x_i)),i=1,n_points)
 *      error_type == 3
 *         chi^2 = sum(abs(y_i) - abs(f(x_i)),i=1,n_points)
 */

/* float ParActive[(MAX_SIZE_DISTRIBUTIONS+1)][7];
float Q[MAX_DATA_POINTS], IQ[MAX_DATA_POINTS], IQerror[MAX_DATA_POINTS];
*/
typedef struct {
    Tcl_Interp *interp;
    int   * dF_dpar;
    scalar * l;
    scalar * s;
    scalar Vav;
    scalar Q;
    scalar * a;
    sasfit_function*  SD;
    sasfit_function*  FF;
    sasfit_function*  SQ;
    int   distr;
    bool  *error;
} SQij_param;

typedef struct {
    void *param;
    sasfit_func_ndim_t *KernelnD_fct;
    sasfit_func_two_t *Kernel2D_fct;
    sasfit_func_one_t *Kernel1D_fct;
} multint_cub;

void which_len(Tcl_Interp *, float, float *, int , bool *);
float I_Cyl_core(Tcl_Interp *, float, float, float, float, bool *);
float dI_dL_Cyl_core(Tcl_Interp *, float, float, float, float, bool *);
float dI_dR_Cyl_core(Tcl_Interp *, float, float, float, float, bool *);
float I_Ellipsoid_1_core(Tcl_Interp *, float, float, float, float, bool *);
float I_StackDiscs_core(Tcl_Interp *,float,float,float,float,float,float,float,float,float,float,bool *);
float dI_dV_Ellipsoid_1_core(Tcl_Interp *, float, float, float, float, bool *);
float dI_dR_Ellipsoid_1_core(Tcl_Interp *, float, float, float, float, bool *);
float I_Ellipsoid_2_core(Tcl_Interp *, float, float, float, float, bool *);
float dI_dnu_Ellipsoid_2_core(Tcl_Interp *, float, float, float, float, bool *);
float dI_dR_Ellipsoid_2_core(Tcl_Interp *, float, float, float, float, bool *);
float Int_r_K_Torus_core(Tcl_Interp *,float,float,float,float,float,float,float,bool *);
float Int_theta_K_Torus_core(Tcl_Interp *,float,float,float,float,float,float,float,float,float,bool *);

float IQ_IntdLen(scalar, void *);
float IQ_core(Tcl_Interp *,int *,float *, float *,float,float,float *,sasfit_function* ,sasfit_function* ,sasfit_function* ,int,bool *);
float IQSQij_core(Tcl_Interp *,int *,float *, float *,float,float,float,float *,sasfit_function* ,sasfit_function* ,sasfit_function* ,int,bool *);
float IQSQij_SA_core(Tcl_Interp *,int *,float *, float *,float,float,float,float,float *,sasfit_function* ,sasfit_function* ,sasfit_function* ,int,bool *);

float I_EllipsoidalCoreShell_core(Tcl_Interp *,float,float,float,float,float,float,float,float,bool *);
float F_EllipsoidalCoreShell_core(Tcl_Interp *,float,float,float,float,float,float,float,float,bool *);

void  IQ_t(Tcl_Interp *, float, float, float*, float *, float *, float *, int, sasfit_analytpar *, int, bool *);
void  IQ_t_global(Tcl_Interp *, float, float, float *, float *, float *, float *, int, sasfit_analytpar *, sasfit_commonpar *,int, bool *);
void  IQ_t_gsl_global(Tcl_Interp *, float,float, float *, float *, int, sasfit_analytpar *, sasfit_commonpar *,int, bool *);
#endif
