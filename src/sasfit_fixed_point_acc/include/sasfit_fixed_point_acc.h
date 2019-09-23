/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifndef SASFIT_FP
#define SASFIT_FP

#include <tcl.h>
#include <sasfit_common.h>

#include <stdio.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_roots.h>
#include <gsl/gsl_sf.h>
#include <gsl/gsl_const_mksa.h>
#include <gsl/gsl_spline.h>

#include <nvector/nvector_serial.h>
#include "sasfit_oz.h"

typedef double FP_Operator (void *);

typedef struct {
        Tcl_Interp *interp;
		double *Ih, *DIh, *Ith, *h;
        int indx_min_appearent_sigma, indx_max_appearent_sigma;
        gsl_vector *DR;
        double dr, dq;
        int    it;
        int    Npoints;
        double mixcoeff;
        int    maxsteps;
        double relerror;
        double alpha;
        double gNorm;
		double KLD, JSD, Sum1stDeriv, Sum2ndDeriv, Entropy;
        double Chi2Norm,Gtest, JSDtest;
        sasfit_oz_root_algorithms root_algorithm;
        sasfit_oz_mixstrategy mixstrategy;
        double *in, *out;
        int interrupt;
        int failed;
        int PrintProgress;
        Tcl_Obj *fp_obj;
        int KINSetMAA;
        void  *kin_mem;
        double  KINSetFuncNormTol;
        double KINSetScaledSteptol;
        int KINSetNumMaxIters;
        int KINSetPrintLevel;
        int KINSetEtaForm;
        double KINSetEtaConstValue;
        int KINSpilsSetMaxRestarts;
        int KINSolStrategy;
        double KINSetMaxNewtonStep;
		void *FPstructure;
		N_Vector KINConstraints;
		FP_Operator * FP_Op;

} sasfit_fp_data;

int FP_init (sasfit_fp_data *);
int FP_solver (sasfit_fp_data *);
int FP_free (sasfit_fp_data *);
static int FP_step_kinsol(N_Vector, N_Vector, void *);
static int FP_step_kinsolFP(N_Vector, N_Vector, void *);
double FP_step(sasfit_fp_data *);
int FP_solver_by_iteration(sasfit_fp_data *, sasfit_oz_root_algorithms );
//Modified 29.4.2019

#endif // SASFIT_FP
