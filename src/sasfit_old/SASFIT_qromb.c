/*
 * src/sasfit_old/SASFIT_qromb.c
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

#include "../sasfit_common/cubature/cubature.h"
#include <math.h>
#include <sasfit.h>
#include "include/SASFIT_nr.h"
#include <gsl/gsl_integration.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_monte.h>
#include <gsl/gsl_monte_plain.h>
#include <gsl/gsl_monte_miser.h>
#include <gsl/gsl_monte_vegas.h>
#include <sys/time.h>

#ifndef M_PI
#define M_PI       3.14159265358979323846264338328      /* pi */
#endif

#ifndef M_PI_2
#define M_PI_2     1.57079632679489661923132169164      /* pi/2 */
#endif

#define EPS 1.0e-4
#define EPS_RES 1.0e-4
#define EPS_D_RES 1.0e-4
#define EPS_IQ 1.0e-5
#define EPS_ELL1 1.0e-5
#define EPS_ELL2 1.0e-5
#define EPS_STACKDISCS 1.0e-5
#define JMAX 22
#define JMAXP JMAX+1
#define JMAXELL2 22
#define JMAXPELL2 JMAXELL2+1
#define JMAXINF 14
#define JMAXPINF JMAX+1

#define K 5

float SASFITqrombNR_V_dR(Tcl_Interp *interp,
                            float a[],
                            sasfit_function*  SD,
                            sasfit_function*  FF,
                            float Rstart,
                            float Rend,
                            bool  *error)
{
	float ss,dss;
	float s[JMAXP+1],h[JMAXP+1];
	int j;

	h[1]=1.0;
	for (j=1;j<=JMAX;j++) {
		s[j]=SASFITtrapzdNR_V_dR(interp,a,SD,FF,Rstart,Rend,j,error);
		if (*error) return 0.0;
		if (j >= K) {
			SASFITpolint(interp,&h[j-K],&s[j-K],K,0.0,&ss,&dss,error);
			if (*error) return 0.0;
			if (fabs(dss) <= sasfit_eps_get_nriq()*fabs(ss)) return ss;
		}
		s[j+1]=s[j];
		h[j+1]=0.25*h[j];
	}
	sasfit_err("Too many steps in routine SASFITqrombNRdR\n");
	*error = TRUE;
	return 0.0;
}

float SASFITqrombIQdR_old(Tcl_Interp *interp,
		      int  *dF_dpar,
		      float l[],
		      float sq[],
		      float Q,
		      float a[],
		      sasfit_function*  SD,
		      sasfit_function*  FF,
		      sasfit_function*  SQ,
		      int   distr,
		      float Rstart,
		      float Rend,
		      bool  *error)
{
	float ss,dss;
	float s[JMAXP+1],h[JMAXP+1];
	int j;

	h[1]=1.0;
	for (j=1;j<=JMAX;j++) {
		s[j]=SASFITtrapzdIQdR(interp,dF_dpar,l,sq,Q,a,SD,FF,SQ,distr,Rstart,Rend,j,error);
		if (*error) return 0.0;
		if (j >= K) {
			SASFITpolint(interp,&h[j-K],&s[j-K],K,0.0,&ss,&dss,error);
			if (*error) return 0.0;
			if (fabs(dss) <= sasfit_eps_get_nriq()*fabs(ss)) return ss;
		}
		s[j+1]=s[j];
		h[j+1]=0.25*h[j];
	}
	return ss;
	sasfit_err("Too many steps in routine SASFITqrombIQdR\n");
	*error = TRUE;
	return 0.0;
}

int f1D_cubature(unsigned ndim,
                 const double *x,
                 void *param4int,
                 unsigned fdim,
                 double *fval) {
//    sasfit_out("executing now: f1D_cubature\n");
    fval[0] = (*((sasfit_param4int *)param4int)->function)(x[0],param4int);
//    sasfit_out("just called : (*((sasfit_param4int *)param4int)->function)(x[0],param4int)\n");
    if (((sasfit_param4int *)param4int)->error) {
        return TRUE;
    } else {
        return FALSE;
    }
}

typedef struct {
    void *param;
    double (*Kernel1D_fct)(double k, void * param) ;
} qrombint_cub;

double qrKernel_MC1D(double *k, size_t dim, void *pam) {
	qrombint_cub *cub;
	cub = (qrombint_cub *) pam;
	if (dim !=1) {
            sasfit_err("wrong dimension for using Kernel_MC1D (%d), dim needs to be dim>=2\n",dim);
            return 0;
	}
	return (*cub->Kernel1D_fct)(k[0],cub->param);
}

void SASfitNRIQSQintcore(sasfit_param4int *param4int, scalar *res, scalar *err) {

    scalar *aw;
    scalar cubxmin[1], cubxmax[1], fval[1], ferr[1];
    gsl_integration_workspace * w;
    gsl_integration_cquad_workspace * wcquad;
    gsl_integration_glfixed_table * wglfixed;
    gsl_integration_fixed_workspace * wfixed;
    gsl_function F;
    size_t neval;
    int lenaw=4000;
    const gsl_rng_type *T;
    gsl_rng *r;
    unsigned int calls;
    gsl_monte_function GMC;
    int done=0;
    int i;
    qrombint_cub cubstruct;
    cubstruct.Kernel1D_fct=&IQ_IntdLen;
    cubstruct.param=param4int;
    GMC.f = &qrKernel_MC1D;
    GMC.params=&cubstruct;
    GMC.dim=1;
    struct timeval tv; // Seed generation based on time
    gettimeofday(&tv,0);
    unsigned long mySeed = tv.tv_sec + tv.tv_usec;
    int ierr;

    *res = 0;
    *err=0;
    if ( gsl_finite(((sasfit_param4int *)param4int)->Rstart) &&
          gsl_isinf(((sasfit_param4int *)param4int)->Rend) )	// adaptive integration on infinite intervals (a,+\infty)
	{
	    switch(sasfit_get_int_strategy()) {
            case OOURA_DOUBLE_EXP_QUADRATURE: {
                aw = (scalar *)malloc((lenaw)*sizeof(scalar));
                sasfit_intdeiini(lenaw, GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
                sasfit_intdei(&IQ_IntdLen,
                         ((sasfit_param4int *)param4int)->Rstart,
                         aw, res, err, param4int);
                ((sasfit_param4int *)param4int)->error = 0;
                free(aw);
                break;
            }
            default:
                w = gsl_integration_workspace_alloc(lenaw);
                F.function=&IQ_IntdLen;
                F.params = param4int;
                ((sasfit_param4int *)param4int)->error = gsl_integration_qagiu(&F,
                                                                           ((sasfit_param4int *)param4int)->Rstart,
                                                                            0, sasfit_eps_get_nriq(), lenaw,
                                                                            w, res, err);
                gsl_integration_workspace_free (w);
	    }
	}
    switch(sasfit_get_int_strategy()) {
    case OOURA_DOUBLE_EXP_QUADRATURE: {
            aw = (scalar *)malloc((lenaw)*sizeof(scalar));
            sasfit_intdeini(lenaw, GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
            sasfit_intde(&IQ_IntdLen,
                         ((sasfit_param4int *)param4int)->Rstart,
                         ((sasfit_param4int *)param4int)->Rend,
                         aw, res, err, param4int);
            ((sasfit_param4int *)param4int)->error = 0;
            free(aw);
            break;
            }
    case OOURA_CLENSHAW_CURTIS_QUADRATURE: {
            aw = (scalar *)malloc((lenaw+1)*sizeof(scalar));
            sasfit_intccini(lenaw, aw);
            sasfit_intcc(&IQ_IntdLen,
                         ((sasfit_param4int *)param4int)->Rstart,
                         ((sasfit_param4int *)param4int)->Rend,
                         sasfit_eps_get_nriq(),
                         lenaw, aw, res, err,param4int);
            ((sasfit_param4int *)param4int)->error = 0;
            free(aw);
            break;
            }
    case GSL_CQUAD: {
            wcquad = gsl_integration_cquad_workspace_alloc(lenaw);
            F.function=&IQ_IntdLen;
            F.params = param4int;
            ((sasfit_param4int *)param4int)->error = gsl_integration_cquad (&F,
                                                                            ((sasfit_param4int *)param4int)->Rstart,
                                                                            ((sasfit_param4int *)param4int)->Rend,
                                                                            0, sasfit_eps_get_nriq(),
                                                                            wcquad, res, err,&neval);
            gsl_integration_cquad_workspace_free(wcquad);
            break;
            }
    case GSL_QAG: {
            w = gsl_integration_workspace_alloc(lenaw);
            F.function=&IQ_IntdLen;
            F.params = param4int;
            ((sasfit_param4int *)param4int)->error = gsl_integration_qag (&F,
                                                                          ((sasfit_param4int *)param4int)->Rstart,
                                                                          ((sasfit_param4int *)param4int)->Rend,
                                                                          0, sasfit_eps_get_nriq(), lenaw, 3,
                                                                          w, res, err);
            gsl_integration_workspace_free (w);
            break;
            }
    case GSL_QNG: {
            F.function=&IQ_IntdLen;
            F.params = param4int;
            ((sasfit_param4int *)param4int)->error = gsl_integration_qng (&F,
                                                                          ((sasfit_param4int *)param4int)->Rstart,
                                                                          ((sasfit_param4int *)param4int)->Rend,
                                                                          0, sasfit_eps_get_nriq(),
                                                                          res, err, &neval);
            break;
            }
    case H_CUBATURE: {
            ((sasfit_param4int *)param4int)->function=&IQ_IntdLen;
            cubxmin[0]=((sasfit_param4int *)param4int)->Rstart;
            cubxmax[0]=((sasfit_param4int *)param4int)->Rend;
            ((sasfit_param4int *)param4int)->error = hcubature(1, &f1D_cubature,param4int,
                                                               1, cubxmin, cubxmax,
                                                               10000, 0.0, sasfit_eps_get_nriq(),
              ERROR_INDIVIDUAL, fval, ferr);
            *res = fval[0];
            *err = ferr[0];
            break;
            }
    case P_CUBATURE: {
            ((sasfit_param4int *)param4int)->function=&IQ_IntdLen;
            cubxmin[0]=((sasfit_param4int *)param4int)->Rstart;
            cubxmax[0]=((sasfit_param4int *)param4int)->Rend;
            ((sasfit_param4int *)param4int)->error = pcubature(1, &f1D_cubature, param4int,
                                                               1, cubxmin, cubxmax,
                                                               10000,0.0, sasfit_eps_get_nriq(),
                                                               ERROR_INDIVIDUAL, fval, ferr);
            *res = fval[0];
            *err=ferr[0];
            break;
            }
    case NR_QROMB: {
            *res = SASFITqrombIQdR_old(((sasfit_param4int *)param4int)->interp,
                                      ((sasfit_param4int *)param4int)->dF_dpar,
                                      ((sasfit_param4int *)param4int)->l,
                                      ((sasfit_param4int *)param4int)->s,
                                      ((sasfit_param4int *)param4int)->Q,
                                      ((sasfit_param4int *)param4int)->a,
                                      ((sasfit_param4int *)param4int)->SD,
                                      ((sasfit_param4int *)param4int)->FF,
                                      ((sasfit_param4int *)param4int)->SQ,
                                      ((sasfit_param4int *)param4int)->distr,
                                      ((sasfit_param4int *)param4int)->Rstart,
                                      ((sasfit_param4int *)param4int)->Rend,
                                      &ierr);
            ((sasfit_param4int *)param4int)->error = ierr;
            break;
            }
    case GSL_GAUSSLEGENDRE: {
            wglfixed = gsl_integration_glfixed_table_alloc(sasfit_eps_get_gausslegendre());
            F.function=&IQ_IntdLen;
            F.params = param4int;
            *res = gsl_integration_glfixed(&F, ((sasfit_param4int *)param4int)->Rstart, ((sasfit_param4int *)param4int)->Rend,
                                          wglfixed);
            ierr = 0;
            ((sasfit_param4int *)param4int)->error = ierr;
            gsl_integration_glfixed_table_free(wglfixed);
            break;
            }
    case GSL_CHEBYSHEV1: {
            wfixed = gsl_integration_fixed_alloc(gsl_integration_fixed_chebyshev, sasfit_eps_get_chebyshev1(),
                                                 ((sasfit_param4int *)param4int)->Rstart, ((sasfit_param4int *)param4int)->Rend,
                                                 sasfit_eps_get_alpha(), sasfit_eps_get_beta());
            F.function=&IQ_IntdLen;
            F.params = param4int;
            ierr = gsl_integration_fixed(&F, res, wfixed);
            ((sasfit_param4int *)param4int)->error = ierr;
            gsl_integration_fixed_free(wfixed);
            break;
            }
    case GSL_CHEBYSHEV2: {
            wfixed = gsl_integration_fixed_alloc(gsl_integration_fixed_chebyshev2, sasfit_eps_get_chebyshev2(),
                                                 ((sasfit_param4int *)param4int)->Rstart, ((sasfit_param4int *)param4int)->Rend,
                                                  sasfit_eps_get_alpha(), sasfit_eps_get_beta());
            F.function=&IQ_IntdLen;
            F.params = param4int;
            ierr = gsl_integration_fixed(&F, res, wfixed);
            ((sasfit_param4int *)param4int)->error = ierr;
            gsl_integration_fixed_free(wfixed);
            break;
            }
    case GSL_GEGENBAUER: {
            wfixed = gsl_integration_fixed_alloc(gsl_integration_fixed_gegenbauer, sasfit_eps_get_gegenbauer(),
                                                 ((sasfit_param4int *)param4int)->Rstart, ((sasfit_param4int *)param4int)->Rend,
                                                  sasfit_eps_get_alpha(), sasfit_eps_get_beta());
            F.function=&IQ_IntdLen;
            F.params = param4int;
            ierr = gsl_integration_fixed(&F, res, wfixed);
            ((sasfit_param4int *)param4int)->error = ierr;
            gsl_integration_fixed_free(wfixed);
            break;
            }
    case GSL_EXPONENTIAL: {
            wfixed = gsl_integration_fixed_alloc(gsl_integration_fixed_exponential, sasfit_eps_get_exponential(),
                                                 ((sasfit_param4int *)param4int)->Rstart, ((sasfit_param4int *)param4int)->Rend,
                                                 sasfit_eps_get_alpha(), sasfit_eps_get_beta());
            F.function=&IQ_IntdLen;
            F.params = param4int;
            ierr = gsl_integration_fixed(&F, res, wfixed);
            ((sasfit_param4int *)param4int)->error = ierr;
            gsl_integration_fixed_free(wfixed);
            break;
            }
    case GSL_LAGUERRE: {
            wfixed = gsl_integration_fixed_alloc(gsl_integration_fixed_laguerre, sasfit_eps_get_laguerre(),
                                                 ((sasfit_param4int *)param4int)->Rstart, ((sasfit_param4int *)param4int)->Rend,
                                                  sasfit_eps_get_alpha(), sasfit_eps_get_beta());
            F.function=&IQ_IntdLen;
            F.params = param4int;
            ierr = gsl_integration_fixed(&F, res, wfixed);
            ((sasfit_param4int *)param4int)->error = ierr;
            gsl_integration_fixed_free(wfixed);
            break;
            }
    case GSL_JACOBI: {
            wfixed = gsl_integration_fixed_alloc(gsl_integration_fixed_jacobi, sasfit_eps_get_jacobi(),
                                                 ((sasfit_param4int *)param4int)->Rstart, ((sasfit_param4int *)param4int)->Rend,
                                                  sasfit_eps_get_alpha(), sasfit_eps_get_beta());
            F.function=&IQ_IntdLen;
            F.params = param4int;
            ierr = gsl_integration_fixed(&F, res, wfixed);
            ((sasfit_param4int *)param4int)->error = ierr;
            gsl_integration_fixed_free(wfixed);
            break;
            }
    case GSL_MC_PLAIN: {
        cubxmin[0] = ((sasfit_param4int *)param4int)->Rstart;
        cubxmax[0] = ((sasfit_param4int *)param4int)->Rend;
        calls = gsl_max(sasfit_eps_get_iter_4_mc(),50);
        gsl_rng_env_setup ();
        T = gsl_rng_default;
        r = gsl_rng_alloc (T);
        gsl_rng_set(r, mySeed);
        gsl_monte_plain_state *s_plain = gsl_monte_plain_alloc(GMC.dim);
        gsl_monte_plain_integrate (&GMC, cubxmin, cubxmax, GMC.dim, calls, r, s_plain,
                        res, &ferr[0]);
        gsl_monte_plain_free (s_plain);
        if (ferr[0]/(*res)>sasfit_eps_get_nriq()) {
            ierr=1;
            ((sasfit_param4int *)param4int)->error = ierr;
            sasfit_err("\nGSL_MC_PLAIN integration failed with the required precision err/res=%le>eps_nriq=%lf\n",ferr[0]/(*res),sasfit_eps_get_nriq());
        }
        break;
        }
    case GSL_MC_MISER: {
        cubxmin[0] = ((sasfit_param4int *)param4int)->Rstart;
        cubxmax[0] = ((sasfit_param4int *)param4int)->Rend;
        calls = gsl_max(sasfit_eps_get_iter_4_mc(),50);
        gsl_rng_env_setup ();
        T = gsl_rng_default;
        r = gsl_rng_alloc (T);
        gsl_rng_set(r, mySeed);
        gsl_monte_miser_state *s_miser = gsl_monte_miser_alloc(GMC.dim);
        gsl_monte_miser_integrate (&GMC, cubxmin, cubxmax, GMC.dim, calls, r, s_miser,
                            res, &ferr[0]);
        gsl_monte_miser_free (s_miser);
        if (ferr[0]/(*res)>sasfit_eps_get_nriq()) {
            ierr=1;
            ((sasfit_param4int *)param4int)->error = ierr;
            sasfit_err("\nGSL_MC_MISER integration failed with the required precision err/res=%lf>eps_nriq=%lf\n",ferr[0]/(*res),sasfit_eps_get_nriq());
        }
        break;
        }
    case GSL_MC_VEGAS: {
        cubxmin[0] = ((sasfit_param4int *)param4int)->Rstart;
        cubxmax[0] = ((sasfit_param4int *)param4int)->Rend;
        calls = gsl_max(sasfit_eps_get_iter_4_mc(),50);
        gsl_rng_env_setup ();
        T = gsl_rng_default;
        r = gsl_rng_alloc (T);
        gsl_rng_set(r, mySeed);
        gsl_monte_vegas_state *s_vegas = gsl_monte_vegas_alloc(GMC.dim);
        gsl_monte_vegas_integrate (&GMC, cubxmin, cubxmax, GMC.dim, calls/10, r, s_vegas,
                        res, &ferr[0]);
        i=1;
        do
        {
            gsl_monte_vegas_integrate (&GMC, cubxmin, cubxmax, GMC.dim, calls/10, r, s_vegas,
                            res, &ferr[0]);
            i++;
        } while (i<10 && ((fabs (gsl_monte_vegas_chisq (s_vegas) - 1.0) > 0.5) ||  ferr[0]/(*res)>sasfit_eps_get_nriq()));
//              sasfit_out("VEGAS: number of calls:%d\t ferr/fval=%lf\n",i*calls/10,ferr[0]/fval[0]);
        gsl_monte_vegas_free (s_vegas);
        if (ferr[0]/(*res)>sasfit_eps_get_nriq()) {
            ierr=1;
            ((sasfit_param4int *)param4int)->error = ierr;
            sasfit_err("\nGSL_MC_VEGAS integration failed with the required precision err/res=%lg>eps_nriq=%lg\n",ferr[0]/(*res),sasfit_eps_get_nriq());
        }
        break;
        }
    default: {
            aw = (scalar *)malloc((lenaw)*sizeof(scalar));
            sasfit_intdeini(lenaw, GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
            sasfit_intde(&IQ_IntdLen,
                         ((sasfit_param4int *)param4int)->Rstart,
                         ((sasfit_param4int *)param4int)->Rend,
                         aw, res, err,param4int);
            ierr = 0;
            ((sasfit_param4int *)param4int)->error = ierr;
            free(aw);
            break;
            }
    }
}

scalar SASFITqrombIQdR(Tcl_Interp *interp,
			int   *dF_dpar,
			scalar l[],
			scalar s[],
			scalar Q,
			scalar a[],
			sasfit_function*  SD,
			sasfit_function*  FF,
			sasfit_function*  SQ,
			int   distr,
			scalar Len_start,
			scalar Len_end,
			bool  *error)
{
    sasfit_param4int param4int;
    scalar res,err;
    param4int.dF_dpar=dF_dpar;
    param4int.l=l;
    param4int.s=s;
    param4int.Q=Q;
    param4int.a=a;
    param4int.SD=SD;
    param4int.FF=FF;
    param4int.SQ=SQ;
    param4int.distr=distr;
    param4int.error=*error;
    param4int.withSQ=FALSE;
    param4int.Rstart=Len_start;
    param4int.Rend=Len_end;
    param4int.interp=interp;
    SASfitNRIQSQintcore(&param4int,&res,&err);
    if (param4int.error > 0) {
        sasfit_err("Integration Int[N(R)I(Q,R),R=0,Infty] did not converged for Q=%lf",Q);
    }
    return res;
}


float SASFITqrombIQSQdR_old(Tcl_Interp *interp,
			int   *dF_dpar,
			float l[],
			float sq[],
			float Q,
			float a[],
			sasfit_function*  SD,
			sasfit_function*  FF,
			sasfit_function*  SQ,
			int   distr,
			float Rstart,
			float Rend,
			bool  *error)
{
	float ss,dss;
	float s[JMAXP+1],h[JMAXP+1];
	int j;

	h[1]=1.0;
	for (j=1;j<=JMAX;j++) {
		s[j]=SASFITtrapzdIQSQdR(interp,dF_dpar,l,sq,Q,a,SD,FF,SQ,distr,Rstart,Rend,j,error);
		if (*error) return 0.0;
		if (j >= K) {
			SASFITpolint(interp,&h[j-K],&s[j-K],K,0.0,&ss,&dss,error);
			if (*error) return 0.0;
			if (fabs(dss) <= sasfit_eps_get_nriq()*fabs(ss)) return ss;
		}
		s[j+1]=s[j];
		h[j+1]=0.25*h[j];
	}
	return ss;
	sasfit_err("Too many steps in routine SASFITqrombIQSQdR\n");
	*error = TRUE;
	return 0.0;
}

scalar SASFITqrombIQSQdR(Tcl_Interp *interp,
			int   *dF_dpar,
			scalar l[],
			scalar s[],
			scalar Q,
			scalar a[],
			sasfit_function*  SD,
			sasfit_function*  FF,
			sasfit_function*  SQ,
			int   distr,
			scalar Len_start,
			scalar Len_end,
			bool  *error)
{
    sasfit_param4int param4int;
    scalar res,err;
    param4int.dF_dpar=dF_dpar;
    param4int.l=l;
    param4int.s=s;
    param4int.Q=Q;
    param4int.a=a;
    param4int.SD=SD;
    param4int.FF=FF;
    param4int.SQ=SQ;
    param4int.distr=distr;
    param4int.error=*error;
    param4int.withSQ=TRUE;
    param4int.Rstart=Len_start;
    param4int.Rend=Len_end;
    param4int.interp=interp;
    SASfitNRIQSQintcore(&param4int,&res,&err);
    if (param4int.error > 0) {
        sasfit_err("Integration Int[N(R)I(Q,R),R=0,Infty] did not converged for Q=%lf",Q);
    }
    return res;
}



float SASFITqrombIQSQijdRi(Tcl_Interp *interp,
						    int *dF_dpar,
						    float l[],
							float len1,
							float sq[],
                            float Q,
                            float a[],
                            sasfit_function*  SD,
                            sasfit_function*  FF,
							sasfit_function*  SQ,
                            int   distr,
                            float Rstart,
                            float Rend,
                            bool  *error)
{
	float ss,dss;
	float s[JMAXP+1],h[JMAXP+1];
	int j;

	h[1]=1.0;
	for (j=1;j<=JMAX;j++) {
		s[j]=SASFITtrapzdIQSQijdRi(interp,dF_dpar,l,len1,sq,Q,a,SD,FF,SQ,distr,Rstart,Rend,j,error);
		if (*error) return 0.0;
		if (j >= K) {
			SASFITpolint(interp,&h[j-K],&s[j-K],K,0.0,&ss,&dss,error);
			if (*error) return 0.0;
			if (fabs(dss) <= sasfit_eps_get_nriq()*fabs(ss)) return ss;
		}
		s[j+1]=s[j];
		h[j+1]=0.25*h[j];
	}
	return ss;
	sasfit_err("Too many steps in routine SASFITqrombIQSQdR\n");
	*error = TRUE;
	return 0.0;
}

float SASFITqrombIQSQijdRj(Tcl_Interp *interp,
						    int   *dF_dpar,
						    float l[],
							float sq[],
                            float Q,
                            float a[],
                            sasfit_function*  SD,
                            sasfit_function*  FF,
							sasfit_function*  SQ,
                            int   distr,
                            float Rstart,
                            float Rend,
                            bool  *error)
{
	float ss,dss;
	float s[JMAXP+1],h[JMAXP+1];
	int j;

	h[1]=1.0;
	for (j=1;j<=JMAX;j++) {
		s[j]=SASFITtrapzdIQSQijdRj(interp,dF_dpar,l,sq,Q,a,SD,FF,SQ,distr,Rstart,Rend,j,error);
		if (*error) return 0.0;
		if (j >= K) {
			SASFITpolint(interp,&h[j-K],&s[j-K],K,0.0,&ss,&dss,error);
			if (*error) return 0.0;
			if (fabs(dss) <= sasfit_eps_get_nriq()*fabs(ss)) return ss;
		}
		s[j+1]=s[j];
		h[j+1]=0.25*h[j];
	}
	return ss;
	sasfit_err("Too many steps in routine SASFITqrombIQSQdR\n");
	*error = TRUE;
	return 0.0;
}

float SASFITqrombSA_IQSQijdRi(Tcl_Interp *interp,
                            int   *dF_dpar,
						    float l[],
							float len1,
							float xav,
							float sq[],
                            float Q,
                            float a[],
                            sasfit_function*  SD,
                            sasfit_function*  FF,
							sasfit_function*  SQ,
                            int   distr,
                            float Rstart,
                            float Rend,
                            bool  *error)
{
	float ss,dss;
	float s[JMAXP+1],h[JMAXP+1];
	int j;

	h[1]=1.0;
	for (j=1;j<=JMAX;j++) {
		s[j]=SASFITtrapzdSA_IQSQijdRi(interp,dF_dpar,l,len1,xav,sq,Q,a,SD,FF,SQ,distr,Rstart,Rend,j,error);
		if (*error) return 0.0;
		if (j >= K) {
			SASFITpolint(interp,&h[j-K],&s[j-K],K,0.0,&ss,&dss,error);
			if (*error) return 0.0;
			if (fabs(dss) <= sasfit_eps_get_nriq()*fabs(ss)) return ss;
		}
		s[j+1]=s[j];
		h[j+1]=0.25*h[j];
	}
	return ss;
	sasfit_err("Too many steps in routine SASFITqrombSA_IQSQdR\n");
	*error = TRUE;
	return 0.0;
}

float SASFITqrombSA_IQSQijdRj(Tcl_Interp *interp,
                            int   *dF_dpar,
			    float l[],
			    float xav,
			    float sq[],
                            float Q,
                            float a[],
                            sasfit_function*  SD,
                            sasfit_function*  FF,
			    sasfit_function*  SQ,
                            int   distr,
                            float Rstart,
                            float Rend,
                            bool  *error)
{
	float ss,dss;
	float s[JMAXP+1],h[JMAXP+1];
	int j;

	h[1]=1.0;
	for (j=1;j<=JMAX;j++) {
		s[j]=SASFITtrapzdSA_IQSQijdRj(interp,dF_dpar,l,xav,sq,Q,a,SD,FF,SQ,distr,Rstart,Rend,j,error);
		if (*error) return 0.0;
		if (j >= K) {
			SASFITpolint(interp,&h[j-K],&s[j-K],K,0.0,&ss,&dss,error);
			if (*error) return 0.0;
			if (fabs(dss) <= sasfit_eps_get_nriq()*fabs(ss)) return ss;
		}
		s[j+1]=s[j];
		h[j+1]=0.25*h[j];
	}
	return ss;
	sasfit_err("Too many steps in routine SASFITqrombSA_IQSQdR\n");
	*error = TRUE;
	return 0.0;
}

float SASFITqromoGz(interp,aa,bb,Q,Qres,par,Ifit,Isubstract,dydpar,max_SD,AP,error_type,error)
Tcl_Interp	*interp;
float		aa,bb,Q,Qres;
float		*par;
float		*Ifit;
float		*Isubstract;
float		*dydpar;
int			max_SD;
sasfit_analytpar *AP;
int			error_type;
bool		*error;
{
	float ss, dss, *DDss, *DDdss;
	float s[JMAXPINF+1],h[JMAXPINF+1], **DDs, **DDh;
	int j,i,kk;
	bool integrate_ready;

	DDss  = dvector(0,(3*MAXPAR)*max_SD-1);
	DDdss = dvector(0,(3*MAXPAR)*max_SD-1);
    DDs   = matrix(0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
	DDh   = matrix(0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
	h[1]=1.0;
	for (i=0;i<max_SD;i++) {
		for (kk=0;kk<(3*MAXPAR);kk++) {
	       DDh[i*(3*MAXPAR)+kk][1] = 1.0;
		}
	}
	for (j=1;j<=JMAXINF;j++) {
		integrate_ready = TRUE;
		s[j]=SASFITmidexpGzHankel(interp,aa,bb,Q,Qres,par,Ifit,Isubstract,dydpar,max_SD,AP,error_type,j,error);
		for (i=0;i<max_SD;i++) {
		    for (kk=0;kk<(3*MAXPAR);kk++) {
				DDs[i*(3*MAXPAR)+kk][j] = dydpar[i*(3*MAXPAR)+kk];
			}
		}
		if (*error) {
			free_dvector(DDss,0,(3*MAXPAR)*max_SD-1);
			free_dvector(DDdss,0,(3*MAXPAR)*max_SD-1);
			free_matrix(DDs,0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
			free_matrix(DDh,0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
			return 0.0;
		}
		if (j >= K) {
			SASFITpolint(interp,&h[j-K],&s[j-K],K,0.0,&ss,&dss,error);
			if (*error) {
				free_dvector(DDss,0,(3*MAXPAR)*max_SD-1);
				free_dvector(DDdss,0,(3*MAXPAR)*max_SD-1);
				free_matrix(DDs,0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
				free_matrix(DDh,0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
				return 0.0;
			}
			for (i=0;i<max_SD;i++) {
				for (kk=0;kk<(3*MAXPAR);kk++) {
					SASFITpolint(interp,&DDh[i*(3*MAXPAR)+kk][j-K],&DDs[i*(3*MAXPAR)+kk][j-K],K,0.0,&DDss[i*(3*MAXPAR)+kk],&DDdss[i*(3*MAXPAR)+kk],error);
					if (*error) {
						free_dvector(DDss,0,(3*MAXPAR)*max_SD-1);
						free_dvector(DDdss,0,(3*MAXPAR)*max_SD-1);
						free_matrix(DDs,0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
						free_matrix(DDh,0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
						return 0.0;
					}
				}
			}
			for (i=0;i<max_SD;i++) {
				for (kk=0;kk<(3*MAXPAR);kk++) {
					if (fabs(DDdss[i*(3*MAXPAR)+kk]) > sasfit_eps_get_res()*fabs(DDss[i*(3*MAXPAR)+kk])) integrate_ready = FALSE;
				}
			}
			if (fabs(dss) > sasfit_eps_get_res()*fabs(ss)) integrate_ready = FALSE;
			if (integrate_ready == TRUE) return ss;
		}
		s[j+1]=s[j];
		h[j+1]=h[j]/9.0;
		for (i=0;i<max_SD;i++) {
		    for (kk=0;kk<(3*MAXPAR);kk++) {
				DDs[i*(3*MAXPAR)+kk][j+1] = DDs[i*(3*MAXPAR)+kk][j];
				DDh[i*(3*MAXPAR)+kk][j+1] = DDh[i*(3*MAXPAR)+kk][j]/9.0;
			}
		}
	}
	sasfit_err("Too many steps in routine SASFITqromoGz\n");
	*error = TRUE;
	free_dvector(DDss,0,(3*MAXPAR)*max_SD-1);
	free_dvector(DDdss,0,(3*MAXPAR)*max_SD-1);
	free_matrix(DDs,0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
	free_matrix(DDh,0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
	return 0.0;
}

float SASFITqromoGlobalGz(interp,aa,bb,Q,Qres,par,Ifit,Isub,dydpar,max_SD,GAP,GCP,error_type,error)
Tcl_Interp	*interp;
float		aa,bb,Q,Qres;
float		*par;
float		*Ifit;
float		*Isub;
float		*dydpar;
int			max_SD;
sasfit_analytpar *GAP;
sasfit_commonpar *GCP;
int			error_type;
bool		*error;
{
	float ss, dss, *DDss, *DDdss;
	float s[JMAXPINF+1],h[JMAXPINF+1], **DDs, **DDh;
	int j,i,kk;
	bool integrate_ready;

	DDss  = dvector(0,(3*MAXPAR)*max_SD-1);
	DDdss = dvector(0,(3*MAXPAR)*max_SD-1);
    DDs   = matrix(0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
	DDh   = matrix(0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
	h[1]=1.0;
	for (i=0;i<max_SD;i++) {
		for (kk=0;kk<(3*MAXPAR);kk++) {
	       DDh[i*(3*MAXPAR)+kk][1] = 1.0;
		}
	}
	for (j=1;j<=JMAXINF;j++) {
		integrate_ready = TRUE;
		s[j]=SASFITmidexpGlobalGzHankel(interp,aa,bb,Q,Qres,par,Ifit,Isub,dydpar,max_SD,GAP,GCP,error_type,j,error);
		for (i=0;i<max_SD;i++) {
		    for (kk=0;kk<(3*MAXPAR);kk++) {
				DDs[i*(3*MAXPAR)+kk][j] = dydpar[i*(3*MAXPAR)+kk];
			}
		}
		if (*error) {
			free_dvector(DDss,0,(3*MAXPAR)*max_SD-1);
			free_dvector(DDdss,0,(3*MAXPAR)*max_SD-1);
			free_matrix(DDs,0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
			free_matrix(DDh,0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
			return 0.0;
		}
		if (j >= K) {
			SASFITpolint(interp,&h[j-K],&s[j-K],K,0.0,&ss,&dss,error);
			if (*error) {
				free_dvector(DDss,0,(3*MAXPAR)*max_SD-1);
				free_dvector(DDdss,0,(3*MAXPAR)*max_SD-1);
				free_matrix(DDs,0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
				free_matrix(DDh,0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
				return 0.0;
			}
			for (i=0;i<max_SD;i++) {
				for (kk=0;kk<(3*MAXPAR);kk++) {
					SASFITpolint(interp,&DDh[i*(3*MAXPAR)+kk][j-K],&DDs[i*(3*MAXPAR)+kk][j-K],K,0.0,&DDss[i*(3*MAXPAR)+kk],&DDdss[i*(3*MAXPAR)+kk],error);
					if (*error) {
						free_dvector(DDss,0,(3*MAXPAR)*max_SD-1);
						free_dvector(DDdss,0,(3*MAXPAR)*max_SD-1);
						free_matrix(DDs,0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
						free_matrix(DDh,0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
						return 0.0;
					}
				}
			}
			for (i=0;i<max_SD;i++) {
				for (kk=0;kk<(3*MAXPAR);kk++) {
					if (fabs(DDdss[i*(3*MAXPAR)+kk]) > sasfit_eps_get_res()*fabs(DDss[i*(3*MAXPAR)+kk])) integrate_ready = FALSE;
				}
			}
			if (fabs(dss) > sasfit_eps_get_res()*fabs(ss)) integrate_ready = FALSE;
			if (integrate_ready == TRUE) {
				free_dvector(DDss,0,(3*MAXPAR)*max_SD-1);
			    free_dvector(DDdss,0,(3*MAXPAR)*max_SD-1);
			    free_matrix(DDs,0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
  			    free_matrix(DDh,0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
				return ss;
			}
		}
		s[j+1]=s[j];
		h[j+1]=h[j]/9.0;
		for (i=0;i<max_SD;i++) {
		    for (kk=0;kk<(3*MAXPAR);kk++) {
				DDs[i*(3*MAXPAR)+kk][j+1] = DDs[i*(3*MAXPAR)+kk][j];
				DDh[i*(3*MAXPAR)+kk][j+1] = DDh[i*(3*MAXPAR)+kk][j]/9.0;
			}
		}
	}
	sasfit_err("Too many steps in routine SASFITqromoGlobalGz\n");
	*error = TRUE;
	free_dvector(DDss,0,(3*MAXPAR)*max_SD-1);
	free_dvector(DDdss,0,(3*MAXPAR)*max_SD-1);
	free_matrix(DDs,0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
	free_matrix(DDh,0,(3*MAXPAR)*max_SD-1,0,JMAXPINF+1);
	return 0.0;
}

float SASFITqrombIQ(interp,Qmin,Qmax,Q,Qres,par,Ifit,Isubstract,dydpar,max_SD,AP,error_type,error)
Tcl_Interp	*interp;
float		Qmin,Qmax,Q,Qres;
float		*par;
float		*Ifit;
float		*Isubstract;
float		*dydpar;
int			max_SD;
sasfit_analytpar *AP;
int			error_type;
bool		*error;
{
	float ss, dss, *DDss, *DDdss;
	float s[JMAXP+1],h[JMAXP+1], **DDs, **DDh;
	int j,i,kk;
	bool integrate_ready;

	DDss  = dvector(0,(3*MAXPAR)*max_SD-1);
	DDdss = dvector(0,(3*MAXPAR)*max_SD-1);
    DDs   = matrix(0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
	DDh   = matrix(0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
	h[1]=1.0;
	for (i=0;i<max_SD;i++) {
		for (kk=0;kk<(3*MAXPAR);kk++) {
	       DDh[i*(3*MAXPAR)+kk][1] = 1.0;
		}
	}
	for (j=1;j<=JMAX;j++) {
		integrate_ready = TRUE;
		s[j]=SASFITtrapzdIQ(interp,Qmin,Qmax,Q,Qres,par,Ifit,Isubstract,dydpar,max_SD,AP,error_type,j,error);
		for (i=0;i<max_SD;i++) {
		    for (kk=0;kk<(3*MAXPAR);kk++) {
				DDs[i*(3*MAXPAR)+kk][j] = dydpar[i*(3*MAXPAR)+kk];
			}
		}
		if (*error) {
			free_dvector(DDss,0,(3*MAXPAR)*max_SD-1);
			free_dvector(DDdss,0,(3*MAXPAR)*max_SD-1);
			free_matrix(DDs,0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
			free_matrix(DDh,0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
			return 0.0;
		}
		if (j >= K) {
			SASFITpolint(interp,&h[j-K],&s[j-K],K,0.0,&ss,&dss,error);
			if (*error) {
				free_dvector(DDss,0,(3*MAXPAR)*max_SD-1);
				free_dvector(DDdss,0,(3*MAXPAR)*max_SD-1);
				free_matrix(DDs,0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
				free_matrix(DDh,0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
				return 0.0;
			}
			for (i=0;i<max_SD;i++) {
				for (kk=0;kk<(3*MAXPAR);kk++) {
					SASFITpolint(interp,&DDh[i*(3*MAXPAR)+kk][j-K],&DDs[i*(3*MAXPAR)+kk][j-K],K,0.0,&DDss[i*(3*MAXPAR)+kk],&DDdss[i*(3*MAXPAR)+kk],error);
					if (*error) {
						free_dvector(DDss,0,(3*MAXPAR)*max_SD-1);
						free_dvector(DDdss,0,(3*MAXPAR)*max_SD-1);
						free_matrix(DDs,0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
						free_matrix(DDh,0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
						return 0.0;
					}
				}
			}
			for (i=0;i<max_SD;i++) {
				for (kk=0;kk<(3*MAXPAR);kk++) {
					if (fabs(DDdss[i*(3*MAXPAR)+kk]) > sasfit_eps_get_res()*fabs(DDss[i*(3*MAXPAR)+kk])) integrate_ready = FALSE;
				}
			}
			if (fabs(dss) > sasfit_eps_get_res()*fabs(ss)) integrate_ready = FALSE;
			if (integrate_ready == TRUE) {
                    free_dvector(DDss,0,(3*MAXPAR)*max_SD-1);
                    free_dvector(DDdss,0,(3*MAXPAR)*max_SD-1);
                    free_matrix(DDs,0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
                    free_matrix(DDh,0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
                    return ss;
			}
		}
		s[j+1]=s[j];
		h[j+1]=0.25*h[j];
		for (i=0;i<max_SD;i++) {
		    for (kk=0;kk<(3*MAXPAR);kk++) {
				DDs[i*(3*MAXPAR)+kk][j+1] =      DDs[i*(3*MAXPAR)+kk][j];
				DDh[i*(3*MAXPAR)+kk][j+1] = 0.25*DDh[i*(3*MAXPAR)+kk][j];
			}
		}
	}
	sasfit_err("Too many steps in routine SASFITqrombIQ\n");
	*error = TRUE;
	free_dvector(DDss,0,(3*MAXPAR)*max_SD-1);
	free_dvector(DDdss,0,(3*MAXPAR)*max_SD-1);
	free_matrix(DDs,0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
	free_matrix(DDh,0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
	return 0.0;
}

float SASFITqrombIQglobal(interp,Qmin,Qmax,Q,Qres,par,Ifit,Isub,dydpar,max_SD,GAP,GCP,error_type,error)
Tcl_Interp	*interp;
float		Qmin,Qmax,Q,Qres;
float		*par;
float		*Ifit;
float		*Isub;
float		*dydpar;
int			max_SD;
sasfit_analytpar *GAP;
sasfit_commonpar *GCP;
int			error_type;
bool		*error;
{
	float ss, dss, *DDss, *DDdss;
	float s[JMAXP+1],h[JMAXP+1], **DDs, **DDh;
	int j,i,kk;
	bool integrate_ready;

	DDss  = dvector(0,(3*MAXPAR)*max_SD-1);
	DDdss = dvector(0,(3*MAXPAR)*max_SD-1);
    DDs   = matrix(0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
	DDh   = matrix(0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
	h[1]=1.0;
	for (i=0;i<max_SD;i++) {
		for (kk=0;kk<(3*MAXPAR);kk++) {
	       DDh[i*(3*MAXPAR)+kk][1] = 1.0;
		}
	}
	for (j=1;j<=JMAX;j++) {
		integrate_ready = TRUE;
		s[j]=SASFITtrapzdIQglobal(interp,Qmin,Qmax,Q,Qres,par,Ifit,Isub,dydpar,max_SD,GAP,GCP,error_type,j,error);
		for (i=0;i<max_SD;i++) {
		    for (kk=0;kk<(3*MAXPAR);kk++) {
				DDs[i*(3*MAXPAR)+kk][j] = dydpar[i*(3*MAXPAR)+kk];
			}
		}
		if (*error) {
			free_dvector(DDss,0,(3*MAXPAR)*max_SD-1);
			free_dvector(DDdss,0,(3*MAXPAR)*max_SD-1);
			free_matrix(DDs,0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
			free_matrix(DDh,0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
			return 0.0;
		}
		if (j >= K) {
			SASFITpolint(interp,&h[j-K],&s[j-K],K,0.0,&ss,&dss,error);
			if (*error) {
				free_dvector(DDss,0,(3*MAXPAR)*max_SD-1);
				free_dvector(DDdss,0,(3*MAXPAR)*max_SD-1);
				free_matrix(DDs,0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
				free_matrix(DDh,0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
				return 0.0;
			}
			for (i=0;i<max_SD;i++) {
				for (kk=0;kk<(3*MAXPAR);kk++) {
					SASFITpolint(interp,&DDh[i*(3*MAXPAR)+kk][j-K],&DDs[i*(3*MAXPAR)+kk][j-K],K,0.0,&DDss[i*(3*MAXPAR)+kk],&DDdss[i*(3*MAXPAR)+kk],error);
					if (*error) {
						free_dvector(DDss,0,(3*MAXPAR)*max_SD-1);
						free_dvector(DDdss,0,(3*MAXPAR)*max_SD-1);
						free_matrix(DDs,0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
						free_matrix(DDh,0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
						return 0.0;
					}
				}
			}
			for (i=0;i<max_SD;i++) {
				for (kk=0;kk<(3*MAXPAR);kk++) {
					if (fabs(DDdss[i*(3*MAXPAR)+kk]) > sasfit_eps_get_res()*fabs(DDss[i*(3*MAXPAR)+kk])) integrate_ready = FALSE;
				}
			}
			if (fabs(dss) > sasfit_eps_get_res()*fabs(ss)) integrate_ready = FALSE;
			if (integrate_ready == TRUE) {
				free_dvector(DDss,0,(3*MAXPAR)*max_SD-1);
			    free_dvector(DDdss,0,(3*MAXPAR)*max_SD-1);
			    free_matrix(DDs,0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
  			    free_matrix(DDh,0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
				return ss;
			}
		}
		s[j+1]=s[j];
		h[j+1]=0.25*h[j];
		for (i=0;i<max_SD;i++) {
		    for (kk=0;kk<(3*MAXPAR);kk++) {
				DDs[i*(3*MAXPAR)+kk][j+1] =      DDs[i*(3*MAXPAR)+kk][j];
				DDh[i*(3*MAXPAR)+kk][j+1] = 0.25*DDh[i*(3*MAXPAR)+kk][j];
			}
		}
	}
	sasfit_err("Too many steps in routine SASFITqrombIQ\n");
	*error = TRUE;
	free_dvector(DDss,0,(3*MAXPAR)*max_SD-1);
	free_dvector(DDdss,0,(3*MAXPAR)*max_SD-1);
	free_matrix(DDs,0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
	free_matrix(DDh,0,(3*MAXPAR)*max_SD-1,0,JMAXP+1);
	return 0.0;
}


float SASFITqrombIQ_gsl_global(interp,Qmin,Qmax,Q,Qres,par,Ifit,max_SD,GAP,GCP,error_type,error)
Tcl_Interp	*interp;
float		Qmin,Qmax,Q,Qres;
float		*par;
float		*Ifit;
int			max_SD;
sasfit_analytpar *GAP;
sasfit_commonpar *GCP;
int			error_type;
bool		*error;
{
	float ss, dss;
	float s[JMAXP+1],h[JMAXP+1];
	int j;
	bool integrate_ready;

	h[1]=1.0;

	for (j=1;j<=JMAX;j++) {
		integrate_ready = TRUE;
		s[j]=SASFITtrapzdIQ_gsl_global(interp,Qmin,Qmax,Q,Qres,par,Ifit,max_SD,GAP,GCP,error_type,j,error);
		if (j >= K) {
			SASFITpolint(interp,&h[j-K],&s[j-K],K,0.0,&ss,&dss,error);
			if (fabs(dss) > sasfit_eps_get_res()*fabs(ss)) integrate_ready = FALSE;
			if (integrate_ready == TRUE) return ss;
		}
		s[j+1]=s[j];
		h[j+1]=0.25*h[j];
	}
	sasfit_err("Too many steps in routine SASFITqrombIQ\n");
	*error = TRUE;
	return 0.0;
}

#undef EPS
#undef EPS_D_RES
#undef EPS_RES
#undef EPS_IQ
#undef EPS_ELL1
#undef EPS_ELL2
#undef EPS_STACKDISCS
#undef JMAX
#undef JMAXP
#undef JMAXELL2
#undef JMAXPELL2
#undef JMAXINF
#undef JMAXPINF
#undef K

