/*
 * src/sasfit_common/sasfit_integrate.c
 *
 * Copyright (c) 2008-2022, Paul Scherrer Institute (PSI)
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
 *   Ingo Bressler (ingo.bressler@bam.de)
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */


// #include <omp.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_monte.h>
#include <gsl/gsl_monte_plain.h>
#include <gsl/gsl_monte_miser.h>
#include <gsl/gsl_monte_vegas.h>
#include <sys/time.h>
#include <gsl/gsl_integration.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_sf.h>
#include "include/sasfit_common.h"
#include "include/sasfit_function.h"
#include "include/kk51Hankel.h"
#include "include/kk101Hankel.h"
#include "include/kk201Hankel.h"
#include "include/wa801Hankel.h"
#include "include/kk101CosSin.h"
#include "include/kk201CosSin.h"
#include "include/sasfit_hankel.h"
#include "include/lobatto.h"
#include "multidiminte/src/tanhsinh/tanhsinh.h"
#include "quasimontecarlo/quasimontecarlo.h"
#include "quasimontecarlo/Burley2020Scrambling/genpoints.h"
#include "../sasfit_jburkardt/include/filon_rule.h"

typedef struct {
    sasfit_param *param;
    sasfit_func_two_t *Kernel2D_fct;
    sasfit_func_one_t *Kernel1D_fct;
} int_cub;

//sasfit_int_workspace_t	sasfit_int_workspace;
// = {0, NULL};
sasfit_int_ws_all_t	sasfit_int_ws_all;
// = { -1 };

int omp_get_thread_num() {
	return 1;
}

// not used yet
void sasfit_int_ws_all_free(int thid)
{
	int i=0;
	for(i=SASFIT_MAX_WS_PER_THREAD-1; i >= 0 ;i--)
	{
		sasfit_int_free(i, thid);
		sasfit_int_release(thid);
	}
}
void sasfit_int_ws_init(void) {
    int i,j;
    for (i=0;i<SASFIT_MAX_THREAD;i++) {
        for (j=0;j<SASFIT_MAX_WS_PER_THREAD;j++) {
            sasfit_int_ws_all.last[omp_get_thread_num()] = -1;
        }
    }
}

void sasfit_int_free(int idx, int thid)
{
	if ( idx >= SASFIT_MAX_WS_PER_THREAD || idx < 0 )
	{
		sasfit_err("Invalid number of integration workspaces !\n");
		return;
	}
	if ( sasfit_int_ws_all.ws[omp_get_thread_num()][idx].ptr != NULL )
	{
		gsl_integration_workspace_free(sasfit_int_ws_all.ws[omp_get_thread_num()][idx].ptr);
	}
	sasfit_int_ws_all.ws[thid][idx].ptr = NULL;
	sasfit_int_ws_all.ws[thid][idx].size = 0;
}

void sasfit_int_occupy(int size, int thid)
{	int occ;
	occ = sasfit_int_ws_all.last[thid]+1;
	if ( size <= 0 ) return;
	if ( occ >= SASFIT_MAX_WS_PER_THREAD || occ < 0 )
	{
		sasfit_err("Can't occupy more integration workspace !\n");
		return;
	}
	// enlarge the gsl workspace if required
	if ( size > sasfit_int_ws_all.ws[thid][occ].size )
	{
		sasfit_int_free(occ,thid);
		sasfit_int_ws_all.ws[thid][occ].ptr = gsl_integration_workspace_alloc(size);
		sasfit_int_ws_all.ws[thid][occ].size = size;
	}
	sasfit_int_ws_all.last[thid] = occ; // finally occupy it
//sasfit_out("occupied: %d\n",sasfit_int_ws_all.last);
}

void sasfit_int_release(int thid)
{
	sasfit_int_ws_all.last[thid]--;
//sasfit_out("released: %d\n",sasfit_int_ws_all.last);
}

gsl_integration_workspace * sasfit_int_mem(int thid)
{
	if ( sasfit_int_ws_all.last[thid] >= SASFIT_MAX_WS_PER_THREAD ||
	     sasfit_int_ws_all.last[thid] < 0 ) return NULL;
	return sasfit_int_ws_all.ws[thid][sasfit_int_ws_all.last[thid]].ptr;
}

double sasfit_gauss_legendre_2D_cube(sasfit_func_two_void_t f,
                                    void *pam,
                                    double a,
                                    double b,
                                    double c,
                                    double d,
                                    const gsl_integration_glfixed_table * tbl)
{
    const double * const x = tbl->x;
    const double * const w = tbl->w;
    const int n = tbl->n;
	double A,B,C,D,Ax,Cy,s,t;
	int i,j, dtbl, m;

	m = (n+1)>>1;

	A = 0.5*(b-a);
	B = 0.5*(b+a);
	C = 0.5*(d-c);
	D = 0.5*(d+c);

	if(n&1) /* n - odd */
	{

		s = w[0]*w[0]*(*f)(B,D,pam);

		for (j=1,t=0.0;j<m;j++)
		{
			Cy = C*x[j];
			t += w[j]*((*f)(B,D+Cy,pam)+(*f)(B,D-Cy,pam));
		}
		s += w[0]*t;

		for (i=1,t=0.0;i<m;i++)
		{
			Ax = A*x[i];
			t += w[i]*((*f)(B+Ax,D,pam)+(*f)(B-Ax,D,pam));
		}
		s += w[0]*t;

		for (i=1;i<m;i++)
		{
			Ax = A*x[i];
			for (j=1;j<m;j++)
			{
				Cy = C*x[j];
				s += w[i]*w[j]*( (*f)(B+Ax,D+Cy,pam)+(*f)(Ax+B,D-Cy,pam)+(*f)(B-Ax,D+Cy,pam)+(*f)(B-Ax,D-Cy,pam));
			}
		}

	}else{ /* n - even */

		s = 0.0;
		for (i=0;i<m;i++)
		{
			Ax = A*x[i];
			for (j=0;j<m;j++)
			{
				Cy = C*x[j];
				s += w[i]*w[j]*( (*f)(B+Ax,D+Cy,pam)+(*f)(Ax+B,D-Cy,pam)+(*f)(B-Ax,D+Cy,pam)+(*f)(B-Ax,D-Cy,pam));
			}
		}
	}
	return C*A*s;
}


int Kernel_cub_1D(unsigned ndim, const double *x, void *pam,
      unsigned fdim, double *fval) {
	sasfit_param * param;
	int_cub *cub;
	cub = (int_cub *) pam;
	param = (sasfit_param *) cub->param;

	if ((ndim < 1) || (fdim < 1)) {
		sasfit_out("false dimensions fdim:%d ndim:%d\n",fdim,ndim);
		return 1;
	}
	fval[0]=(*cub->Kernel1D_fct)(x[0],param);
	return 0;
}

scalar Kernel_1D(scalar x, void *pam) {
	sasfit_param * param;
	int_cub *cub;
	cub = (int_cub *) pam;
	param = (sasfit_param *) cub->param;
	return (*cub->Kernel1D_fct)(x,param);
}

int Kernel_cub_2D(unsigned ndim, const double *x, void *pam,
      unsigned fdim, double *fval) {
	sasfit_param * param;
	int_cub *cub;
	cub = (int_cub *) pam;
	param = (sasfit_param *) cub->param;

	if ((ndim < 2) || (fdim < 1)) {
		sasfit_out("false dimensions fdim:%d ndim:%d\n",fdim,ndim);
		return 1;
	}
	fval[0]=(*cub->Kernel2D_fct)(acos(x[0]),2*M_PI*x[1],param);
//	fval[0]=(*cub->Kernel2D_fct)(x[0],x[1],param)*sin(x[0]);
	return 0;
}

scalar Kernel_GL(scalar theta, scalar phi, void *pam) {
	sasfit_param * param;
	int_cub *cub;
	cub = (int_cub *) pam;
	param = (sasfit_param *) cub->param;
	return (*cub->Kernel2D_fct)(acos(theta),2*M_PI*phi,param);
//	return (*cub->Kernel2D_fct)(theta,phi,param)*sin(theta);
}

double Kernel_MC1D(double *k, size_t dim, void *pam) {
	sasfit_param * param;
	int_cub *cub;
	cub = (int_cub *) pam;
	param = (sasfit_param *) cub->param;
	if (dim !=1) {
            sasfit_err("wrong dimension for using Kernel_MC1D (%d), dim needs to be dim>=2\n",dim);
            return 0;
	}
	return (*cub->Kernel1D_fct)(k[0],param);
}

double Kernel_cubature(const double *k, size_t dim, void *pam) {
	sasfit_param * param;
	int_cub *cub;
	cub = (int_cub *) pam;
	param = (sasfit_param *) cub->param;
	if (dim <2) sasfit_err("wrong dimension for using Kernel_MC2D (%d), dim needs to be dim>=2\n",dim);
	return (*cub->Kernel2D_fct)(acos(k[0]),2*M_PI*k[1],param);
}

double Kernel_MC2D(double *k, size_t dim, void *pam) {
	sasfit_param * param;
	int_cub *cub;
	cub = (int_cub *) pam;
	param = (sasfit_param *) cub->param;
	if (dim <2) sasfit_err("wrong dimension for using Kernel_MC2D (%d), dim needs to be dim>=2\n",dim);
	return (*cub->Kernel2D_fct)(acos(k[0]),2*M_PI*k[1],param);
//	return (*cub->Kernel2D_fct)(k[0],k[1],param)*sin(k[0]);
}

scalar sasfit_orient_avg_ctm(
			sasfit_func_two_t *intKern_fct,
			sasfit_param * param,
			int limit,
			scalar epsabs,
			scalar epsrel)
{   double Iavg,sphi,cphi,PHI1,i_r8;
    int i;
    double *w, *x,*y,*z, xc, yc, zc;
    int order, rule, lambda_i, itmp;
    double ang_x;
    double fact,phi,theta;
    scalar cubxmin[2], cubxmax[2], fval[1], ferr[1];
    gsl_integration_glfixed_table * wglfixed;
    int_cub cubstruct;
    const gsl_rng_type *T;
    gsl_rng *r;
    gsl_qrng *qrng;
    unsigned int calls;
    struct timeval tv; // Seed generation based on time
    gettimeofday(&tv,0);
    unsigned long mySeed = tv.tv_sec + tv.tv_usec;
    gsl_monte_function GMC;
    cubstruct.Kernel2D_fct=intKern_fct;
    cubstruct.param=param;
    GMC.f = &Kernel_MC2D;
    GMC.params=&cubstruct;
    GMC.dim=2;
    Iavg = 0;

    switch (sasfit_get_sphavg_strategy()) {
        case SPHAVG_MC_PLAIN: {
                cubxmax[0] = 1;
                cubxmin[0] = cos(sasfit_param_get_polar_theta());
                cubxmin[1] = 0;
                cubxmax[1] = sasfit_param_get_polar_phi()/(2*M_PI);
                calls = gsl_max(sasfit_eps_get_iter_4_mc(),50);
                gsl_rng_env_setup ();
                T = gsl_rng_default;
                r = gsl_rng_alloc (T);
                gsl_rng_set(r, mySeed);
                gsl_monte_plain_state *s_plain = gsl_monte_plain_alloc(2);
                gsl_monte_plain_integrate (&GMC, cubxmin, cubxmax, 2, calls, r, s_plain,
                               &fval[0], &ferr[0]);
                gsl_monte_plain_free (s_plain);
                gsl_rng_free(r);
                Iavg = fval[0]/((1-cos(sasfit_param_get_polar_theta()))*sasfit_param_get_polar_phi()/(2*M_PI));
                break;
            }
        case SPHAVG_QMC_NIEDERREITER_2 :
                cubxmax[0] = 1;
                cubxmin[0] = cos(sasfit_param_get_polar_theta());
                cubxmin[1] = 0;
                cubxmax[1] = sasfit_param_get_polar_phi()/(2*M_PI);
                calls = gsl_max(sasfit_eps_get_iter_4_mc(),50);
                qrng = gsl_qrng_alloc(gsl_qrng_niederreiter_2, 2);
                quasi_monte_state* s_niederreiter_2 = quasi_monte_alloc(2);
                quasi_monte_integrate(&GMC, cubxmin, cubxmax, 2, calls, epsrel, epsabs, qrng, s_niederreiter_2,
                                &fval[0], &ferr[0]);
                quasi_monte_free(s_niederreiter_2);
                gsl_qrng_free(qrng);
                gsl_rng_free(r);
                Iavg = fval[0]/((1-cos(sasfit_param_get_polar_theta()))*sasfit_param_get_polar_phi()/(2*M_PI));
                break;
        case SPHAVG_QMC_SOBOL :
                cubxmax[0] = 1;
                cubxmin[0] = cos(sasfit_param_get_polar_theta());
                cubxmin[1] = 0;
                cubxmax[1] = sasfit_param_get_polar_phi()/(2*M_PI);
                calls = gsl_max(sasfit_eps_get_iter_4_mc(),50);
                qrng = gsl_qrng_alloc(gsl_qrng_sobol, 2);
                quasi_monte_state* s_sobol = quasi_monte_alloc(2);
                quasi_monte_integrate(&GMC, cubxmin, cubxmax, 2, calls, epsrel, epsabs, qrng, s_sobol,
                                &fval[0], &ferr[0]);
                quasi_monte_free(s_sobol);
                gsl_qrng_free(qrng);
                gsl_rng_free(r);
                Iavg = fval[0]/((1-cos(sasfit_param_get_polar_theta()))*sasfit_param_get_polar_phi()/(2*M_PI));
                break;
        case SPHAVG_QMC_HALTON :
                cubxmax[0] = 1;
                cubxmin[0] = cos(sasfit_param_get_polar_theta());
                cubxmin[1] = 0;
                cubxmax[1] = sasfit_param_get_polar_phi()/(2*M_PI);
                calls = gsl_max(sasfit_eps_get_iter_4_mc(),50);
                qrng = gsl_qrng_alloc(gsl_qrng_halton, 2);
                quasi_monte_state* s_halton = quasi_monte_alloc(2);
                quasi_monte_integrate(&GMC, cubxmin, cubxmax, 2, calls, epsrel, epsabs, qrng, s_halton,
                                &fval[0], &ferr[0]);
                quasi_monte_free(s_halton);
                gsl_qrng_free(qrng);
                gsl_rng_free(r);
                Iavg = fval[0]/((1-cos(sasfit_param_get_polar_theta()))*sasfit_param_get_polar_phi()/(2*M_PI));
                break;
        case SPHAVG_QMC_REVERSEHALTON :
                cubxmax[0] = 1;
                cubxmin[0] = cos(sasfit_param_get_polar_theta());
                cubxmin[1] = 0;
                cubxmax[1] = sasfit_param_get_polar_phi()/(2*M_PI);
                calls = gsl_max(sasfit_eps_get_iter_4_mc(),50);
                qrng = gsl_qrng_alloc(gsl_qrng_reversehalton, 2);
                quasi_monte_state* s_r_halton = quasi_monte_alloc(2);
                quasi_monte_integrate(&GMC, cubxmin, cubxmax, 2, calls, epsrel, epsabs, qrng, s_r_halton,
                                &fval[0], &ferr[0]);
                quasi_monte_free(s_r_halton);
                gsl_qrng_free(qrng);
                gsl_rng_free(r);
                Iavg = fval[0]/((1-cos(sasfit_param_get_polar_theta()))*sasfit_param_get_polar_phi()/(2*M_PI));
                break;
        case SPHAVG_RQMC_SOBOL_RDS :
                cubxmax[0] = 1;
                cubxmin[0] = cos(sasfit_param_get_polar_theta());
                cubxmin[1] = 0;
                cubxmax[1] = sasfit_param_get_polar_phi()/(2*M_PI);
                calls = gsl_max(sasfit_eps_get_iter_4_mc(),50);
                quasi_monte_state* s_sobol_rds = quasi_monte_alloc(2);
                randomized_quasi_monte_integrate(&GMC, cubxmin, cubxmax, 2, calls, epsrel, epsabs, &genpoint_sobol_rds, s_sobol_rds,
                                &fval[0], &ferr[0]);
                quasi_monte_free(s_sobol_rds);
                Iavg = fval[0]/((1-cos(sasfit_param_get_polar_theta()))*sasfit_param_get_polar_phi()/(2*M_PI));
                break;
        case SPHAVG_RQMC_SOBOL_OWEN :
                cubxmax[0] = 1;
                cubxmin[0] = cos(sasfit_param_get_polar_theta());
                cubxmin[1] = 0;
                cubxmax[1] = sasfit_param_get_polar_phi()/(2*M_PI);
                calls = gsl_max(sasfit_eps_get_iter_4_mc(),50);
                quasi_monte_state* s_sobol_owen = quasi_monte_alloc(2);
                randomized_quasi_monte_integrate(&GMC, cubxmin, cubxmax, 2, calls, epsrel, epsabs, &genpoint_sobol_owen, s_sobol_owen,
                                &fval[0], &ferr[0]);
                quasi_monte_free(s_sobol_owen);
                Iavg = fval[0]/((1-cos(sasfit_param_get_polar_theta()))*sasfit_param_get_polar_phi()/(2*M_PI));
                break;
        case SPHAVG_RQMC_FAURE05_OWEN :
                cubxmax[0] = 1;
                cubxmin[0] = cos(sasfit_param_get_polar_theta());
                cubxmin[1] = 0;
                cubxmax[1] = sasfit_param_get_polar_phi()/(2*M_PI);
                calls = gsl_max(sasfit_eps_get_iter_4_mc(),50);
                quasi_monte_state* s_faure05_owen = quasi_monte_alloc(2);
                randomized_quasi_monte_integrate(&GMC, cubxmin, cubxmax, 2, calls, epsrel, epsabs, &genpoint_faure05_owen, s_faure05_owen,
                                &fval[0], &ferr[0]);
                quasi_monte_free(s_faure05_owen);
                Iavg = fval[0]/((1-cos(sasfit_param_get_polar_theta()))*sasfit_param_get_polar_phi()/(2*M_PI));
                break;
        case SPHAVG_RQMC_LAINE_KARRAS :
                cubxmax[0] = 1;
                cubxmin[0] = cos(sasfit_param_get_polar_theta());
                cubxmin[1] = 0;
                cubxmax[1] = sasfit_param_get_polar_phi()/(2*M_PI);
                calls = gsl_max(sasfit_eps_get_iter_4_mc(),50);
                quasi_monte_state* s_laine_karras = quasi_monte_alloc(2);
                randomized_quasi_monte_integrate(&GMC, cubxmin, cubxmax, 2, calls, epsrel, epsabs, &genpoint_laine_karras, s_laine_karras,
                                &fval[0], &ferr[0]);
                quasi_monte_free(s_laine_karras);
                Iavg = fval[0]/((1-cos(sasfit_param_get_polar_theta()))*sasfit_param_get_polar_phi()/(2*M_PI));
                break;
        case SPHAVG_MC_VEGAS: {
                cubxmax[0] = 1;
                cubxmin[0] = cos(sasfit_param_get_polar_theta());
                cubxmin[1] = 0;
                cubxmax[1] = sasfit_param_get_polar_phi()/(2*M_PI);
                calls = gsl_max(sasfit_eps_get_iter_4_mc(),50);
                gsl_rng_env_setup ();
                T = gsl_rng_default;
                r = gsl_rng_alloc (T);
                gsl_rng_set(r, mySeed);
                gsl_monte_vegas_state *s_vegas = gsl_monte_vegas_alloc(2);
                gsl_monte_vegas_integrate (&GMC, cubxmin, cubxmax, 2, calls/10, r, s_vegas,
                               &fval[0], &ferr[0]);
                i=1;
                do
                {
                    gsl_monte_vegas_integrate (&GMC, cubxmin, cubxmax, 2, calls/10, r, s_vegas,
                                   &fval[0], &ferr[0]);
                    i++;
                } while (i<10 && ((fabs (gsl_monte_vegas_chisq (s_vegas) - 1.0) > 0.5) || fabs(ferr[0]/fval[0])>sasfit_eps_get_aniso()));
//                sasfit_out("VEGAS: number of calls:%d\t ferr/fval=%lf\n",i*calls/10,fabs(ferr[0]/fval[0]));
                gsl_monte_vegas_free (s_vegas);
                gsl_rng_free(r);
                Iavg = fval[0]/((1-cos(sasfit_param_get_polar_theta()))*sasfit_param_get_polar_phi()/(2*M_PI));;
                break;
            }
        case SPHAVG_MC_MISER: {
                cubxmax[0] = 1;
                cubxmin[0] = cos(sasfit_param_get_polar_theta());
                cubxmin[1] = 0;
                cubxmax[1] = sasfit_param_get_polar_phi()/(2*M_PI);
                calls = gsl_max(sasfit_eps_get_iter_4_mc(),50);
                gsl_rng_env_setup ();
                T = gsl_rng_default;
                r = gsl_rng_alloc (T);
                gsl_rng_set(r, mySeed);
                gsl_monte_miser_state *s_miser = gsl_monte_miser_alloc(2);
                gsl_monte_miser_integrate (&GMC, cubxmin, cubxmax, 2, calls, r, s_miser,
                               &fval[0], &ferr[0]);
                gsl_monte_miser_free (s_miser);
                gsl_rng_free(r);
                Iavg = fval[0]/((1-cos(sasfit_param_get_polar_theta()))*sasfit_param_get_polar_phi()/(2*M_PI));
                break;
            }
        case SPHAVG_GSL_2D_GAUSSLEGENDRE: {
                wglfixed = gsl_integration_glfixed_table_alloc(sasfit_eps_get_gausslegendre());
//sasfit_out("GSL_2D_GAUSSLEGENDRE order:%d\n",sasfit_eps_get_gausslegendre());
                Iavg = sasfit_gauss_legendre_2D_cube(&Kernel_GL, &cubstruct,
                                                     1, cos(sasfit_param_get_polar_theta()),
                                                     0, sasfit_param_get_polar_phi()/(2*M_PI),
                                                     wglfixed);
                Iavg=-Iavg/((1-cos(sasfit_param_get_polar_theta()))*sasfit_param_get_polar_phi()/(2*M_PI));
                gsl_integration_glfixed_table_free(wglfixed);
                break;
            }
        case SPHAVG_Lebedev: {
                rule = sasfit_eps_get_lebedev();
                while (!sasfit_available_table(rule) && rule < 65) rule++;
                if (rule < 1) rule = 1;
                if (rule > 65) rule = 65;
                order = sasfit_order_table ( rule );
                w = ( double * ) malloc ( order * sizeof ( double ) );
                x = ( double * ) malloc ( order * sizeof ( double ) );
                y = ( double * ) malloc ( order * sizeof ( double ) );
                z = ( double * ) malloc ( order * sizeof ( double ) );
                sasfit_ld_by_order ( order, x, y, z, w );
//sasfit_out("Lebedev order:%d\n",order);
                for (i=0;i<order;i++) {
                    if (x[i] == 0.0 && y[i] == 0.0) {
                        phi=0;
                    } else {
                        phi=atan2(y[i],x[i]);
                    }
                    theta = acos(z[i]);
                    Iavg = Iavg + w[i]*(*intKern_fct)(theta,phi,param);
//sasfit_out("%d: x:%lf y%lf z%lf phi%lf theta%lf Iavg:%lf\n",i,x[i],y[i],z[i],phi,theta,Iavg);
                }
                free ( x );
                free ( y );
                free ( z );
                free ( w );
                break;
            }
        case SPHAVG_SPHERICAL_T_DESIGN: {
                rule = sasfit_eps_get_spherical_t_design();
                while (!sasfit_available_sph_t_table(rule) && rule < 137) rule++;
                if (rule < 1) rule = 1;
                if (rule > 133) rule = 133;
                order = sasfit_sph_t_order_table ( rule );
                x = ( double * ) malloc ( order * sizeof ( double ) );
                y = ( double * ) malloc ( order * sizeof ( double ) );
                z = ( double * ) malloc ( order * sizeof ( double ) );
                sasfit_sph_t_by_order ( order, x, y, z );
//sasfit_out("spherical-t design order:%d\n",order);
                for (i=0;i<order;i++) {
                    if (x[i] == 0.0 && y[i] == 0.0) {
                        phi=0;
                    } else {
                        phi=atan2(y[i],x[i]);
                    }
                    theta = acos(z[i]);
                    Iavg = Iavg + (*intKern_fct)(theta,phi,param);
//sasfit_out("%d: x:%lf y%lf z%lf phi%lf theta%lf Iavg:%lf\n",i,x[i],y[i],z[i],phi,theta,Iavg);
                }
                Iavg = Iavg/order;
                free ( x );
                free ( y );
                free ( z );
                break;
            }
        case SPHAVG_FIBONACCI: {
                //rule = abs(sasfit_eps_get_fibonacci());
                PHI1 = 0.5*(1.0 + sqrt(5.0));
                //order = truncl( gsl_pow_int(PHI1, rule)/sqrt(5.0) + 0.5 ); // this is the n-th Fibonacci number F(n)
                order = abs(sasfit_eps_get_fibonacci());

// sasfit_out("Fibonacci order:%d\n",order);
                for (i=0;i<order;i++) {
                    i_r8 = ( double ) ( - order + 1 + 2 * i );
                    theta = 2.0 * M_PI * i_r8 / PHI1;
                    lambda_i = 2*M_PI*i/PHI1;
                    sphi = i_r8 / order;
                    cphi = sqrt ( ( order + i_r8 ) * ( order - i_r8 ) ) / order;
                    xc = cphi * sin ( theta );
                    yc = cphi * cos ( theta );
                    zc = sphi;
                    // phi : lattitude [-pi/2;pi/2]
                    // theta:lambda_i: longitude [0;2pi]

                    // now turning lattitude and longitude coordinates into standard spherical coordinates
                    // phi is now the rotation angle around z-axis, which was in the previous naming convention lambda_i=theta
                    if (xc == 0.0 && yc == 0.0) {
                        phi=0;
                    } else {
                        phi=atan2(yc,xc);
                    }
                    theta = acos(zc);
                    // phi: polar angle [0,2*pi]
                    // theta: azimuthal angle [0;pi]
                    Iavg = Iavg + (*intKern_fct)(theta,phi,param);
                }
                Iavg = Iavg/order;
                break;
            }
        case SPHAVG_HCUBATURE: {
                cubxmin[0] = 1;
                cubxmax[0] = cos(sasfit_param_get_polar_theta());
                cubxmin[1] = 0;
                cubxmax[1] = sasfit_param_get_polar_phi()/(2*M_PI);
                hcubature(1, &Kernel_cub_2D,&cubstruct,2, cubxmin, cubxmax,
                        limit, epsabs, epsrel, ERROR_PAIRED,
                        fval, ferr);
                Iavg = -fval[0]/((1-cos(sasfit_param_get_polar_theta()))*sasfit_param_get_polar_phi()/(2*M_PI));
                break;
            }
        case SPHAVG_PCUBATURE: {
                cubxmin[0] = 1;
                cubxmax[0] = cos(sasfit_param_get_polar_theta());
                cubxmin[1] = 0;
                cubxmax[1] = sasfit_param_get_polar_phi()/(2*M_PI);
                pcubature(1, &Kernel_cub_2D,&cubstruct,2, cubxmin, cubxmax,
                        limit, epsabs, epsrel, ERROR_PAIRED,
                        fval, ferr);
                Iavg = -fval[0]/((1-cos(sasfit_param_get_polar_theta()))*sasfit_param_get_polar_phi()/(2*M_PI));
                break;
            }
        case SPHAVG_OOURA_DOUBLE_EXP_QUADRATURE:
                itmp = sasfit_get_int_strategy();
                sasfit_set_int_strategy(OOURA_DOUBLE_EXP_QUADRATURE);
                cubxmin[0] = 1;
                cubxmax[0] = cos(sasfit_param_get_polar_theta());
                cubxmin[1] = 0;
                cubxmax[1] = sasfit_param_get_polar_phi()/(2*M_PI);
                sasfit_cubature(2,cubxmin,cubxmax,&Kernel_cubature,&cubstruct,epsrel,fval, ferr);
                Iavg=-fval[0]/((1-cos(sasfit_param_get_polar_theta()))*sasfit_param_get_polar_phi()/(2*M_PI));
                sasfit_set_int_strategy(itmp);
                break;
        case SPHAVG_OOURA_CLENSHAW_CURTIS_QUADRATURE:
                itmp = sasfit_get_int_strategy();
                sasfit_set_int_strategy(OOURA_CLENSHAW_CURTIS_QUADRATURE);
                cubxmin[0] = 1;
                cubxmax[0] = cos(sasfit_param_get_polar_theta());
                cubxmin[1] = 0;
                cubxmax[1] = sasfit_param_get_polar_phi()/(2*M_PI);
                sasfit_cubature(2,cubxmin,cubxmax,&Kernel_cubature,&cubstruct,epsrel,fval, ferr);
                Iavg=-fval[0]/((1-cos(sasfit_param_get_polar_theta()))*sasfit_param_get_polar_phi()/(2*M_PI));
                sasfit_set_int_strategy(itmp);
                break;
        case SPHAVG_GSL_CQUAD:
                itmp = sasfit_get_int_strategy();
                sasfit_set_int_strategy(GSL_CQUAD);
                cubxmin[0] = 1;
                cubxmax[0] = cos(sasfit_param_get_polar_theta());
                cubxmin[1] = 0;
                cubxmax[1] = sasfit_param_get_polar_phi()/(2*M_PI);
                sasfit_cubature(2,cubxmin,cubxmax,&Kernel_cubature,&cubstruct,epsrel,fval, ferr);
                Iavg=-fval[0]/((1-cos(sasfit_param_get_polar_theta()))*sasfit_param_get_polar_phi()/(2*M_PI));
                sasfit_set_int_strategy(itmp);
                break;
        case SPHAVG_TANHSINH_1:
                itmp = sasfit_get_int_strategy();
                sasfit_set_int_strategy(TANHSINH_1);
                cubxmin[0] = 1;
                cubxmax[0] = cos(sasfit_param_get_polar_theta());
                cubxmin[1] = 0;
                cubxmax[1] = sasfit_param_get_polar_phi()/(2*M_PI);
                sasfit_cubature(2,cubxmin,cubxmax,&Kernel_cubature,&cubstruct,epsrel,fval, ferr);
                Iavg=-fval[0]/((1-cos(sasfit_param_get_polar_theta()))*sasfit_param_get_polar_phi()/(2*M_PI));
                sasfit_set_int_strategy(itmp);
                break;
        case SPHAVG_TANHSINH_2:
                itmp = sasfit_get_int_strategy();
                sasfit_set_int_strategy(TANHSINH_2);
                cubxmin[0] = 1;
                cubxmax[0] = cos(sasfit_param_get_polar_theta());
                cubxmin[1] = 0;
                cubxmax[1] = sasfit_param_get_polar_phi()/(2*M_PI);
                sasfit_cubature(2,cubxmin,cubxmax,&Kernel_cubature,&cubstruct,epsrel,fval, ferr);
                Iavg=-fval[0]/((1-cos(sasfit_param_get_polar_theta()))*sasfit_param_get_polar_phi()/(2*M_PI));
                sasfit_set_int_strategy(itmp);
                break;
        default:
                cubxmin[0] = 1;
                cubxmax[0] = cos(sasfit_param_get_polar_theta());
                cubxmin[1] = 0;
                cubxmax[1] = sasfit_param_get_polar_phi()/(2*M_PI);
                pcubature(1, &Kernel_cub_2D,&cubstruct,2, cubxmin, cubxmax,
                        limit, epsabs, epsrel, ERROR_PAIRED,
                        fval, ferr);
                Iavg = -fval[0]/((1-cos(sasfit_param_get_polar_theta()))*sasfit_param_get_polar_phi()/(2*M_PI));
                break;
	    }
	    return Iavg;

}
scalar sasfit_integrate_ctm(scalar int_start,
			scalar int_end,
			sasfit_func_one_t intKern_fct,
			sasfit_param *param,
			int limit,
			scalar epsabs,
			scalar epsrel)
{
	scalar res, errabs;
	int err, thid;
    scalar *aw;
    int_cub cubstruct;
    scalar cubxmin[1], cubxmax[1], fval[1], ferr[1];
    gsl_integration_cquad_workspace * wcquad;
    gsl_integration_glfixed_table * wglfixed;
    gsl_integration_fixed_workspace * wfixed;
	gsl_function F;
    size_t neval;
    int lenaw=10000,ierr;
    const gsl_rng_type *T;
    gsl_rng *r;
    unsigned int calls;
    gsl_monte_function GMC;;
    int done=0;
    int i;
    cubstruct.Kernel1D_fct=intKern_fct;
    cubstruct.param=param;
    GMC.f = &Kernel_MC1D;
    GMC.params=&cubstruct;
    GMC.dim=1;
    struct timeval tv; // Seed generation based on time
    gettimeofday(&tv,0);
    unsigned long mySeed = tv.tv_sec + tv.tv_usec;

	SASFIT_ASSERT_PTR(param);
	SASFIT_ASSERT_PTR(intKern_fct);

//	sasfit_out("integration strategy:%d\n",sasfit_get_int_strategy());
	if ( gsl_finite(int_start) && gsl_finite(int_end) &&
	     (int_end - int_start) == 0.0 )
	// nothing to integrate, test for an eps instead of 0 ? (which?)
	{
		return 0.0;
	}
    thid = omp_get_thread_num();
	sasfit_int_occupy(limit,thid);
	F.params = param;
	F.function = (double (*) (double, void*)) intKern_fct;

    cubstruct.Kernel1D_fct=intKern_fct;
    cubstruct.param=param;

	gsl_set_error_handler_off();
    err = GSL_SUCCESS;
	if ( gsl_isinf(int_start) && gsl_finite(int_end) ) 	// adaptive integration on infinite intervals (-\infty,b)
	{
	    switch (sasfit_get_int_strategy()) {
	        case TANHSINH_1: {
	            res = TanhSinhQuad(&Kernel_1D, &cubstruct,
                            int_start,int_end,7,epsrel,&ferr[0]);
                break;
	        }
	        case TANHSINH_2: {
	            res = qthsh(&Kernel_1D, &cubstruct,
                            int_start,int_end,7,epsrel,&ferr[0]);
                break;
	        }
            default : {
                err = gsl_integration_qagil(&F, int_end, epsabs, epsrel, limit, sasfit_int_mem(thid), &res, &errabs);
            }
	    }
	}
	else if ( gsl_finite(int_start) && gsl_isinf(int_end) )	// adaptive integration on infinite intervals (a,+\infty)
	{
	    switch (sasfit_get_int_strategy()) {
	        case TANHSINH_1: {
	            res = TanhSinhQuad(&Kernel_1D, &cubstruct,
                            int_start,int_end,7,epsrel,&ferr[0]);
                break;
	        }
	        case TANHSINH_2: {
	            res = qthsh(&Kernel_1D, &cubstruct,
                            int_start,int_end,7,epsrel,&ferr[0]);
                break;
	        }
            case OOURA_DOUBLE_EXP_QUADRATURE: {
                aw = (scalar *)malloc((lenaw)*sizeof(scalar));
                sasfit_intdeiini(lenaw, GSL_DBL_MIN, epsrel, aw);
                sasfit_intdei(&Kernel_1D,int_start, aw, &res, &ferr[0],&cubstruct);
                free(aw);
                break;
            }
            default :{
                err = gsl_integration_qagiu(&F, int_start, epsabs, epsrel, limit, sasfit_int_mem(thid), &res, &errabs);
            }
        }
	}
	else if ( gsl_isinf(int_start) && gsl_isinf(int_end) ) 	// adaptive integration on infinite intervals (-\infty,+\infty)
	{
	    switch (sasfit_get_int_strategy()) {
	        case TANHSINH_1: {
	            res = TanhSinhQuad(&Kernel_1D, &cubstruct,
                            int_start,int_end,7,epsrel,&ferr[0]);
                break;
	        }
	        case TANHSINH_2: {
	            res = qthsh(&Kernel_1D, &cubstruct,
                            int_start,int_end,7,epsrel,&ferr[0]);
                break;
	        }
            default : {
                err = gsl_integration_qagi(&F, epsabs, epsrel, limit, sasfit_int_mem(thid), &res, &errabs);
            }
	    }
	}
	else if ( gsl_finite(int_start) && gsl_finite(int_end) ) // adaptive integration
                                                             // on well defined interval (a,b)
	{
//	    sasfit_out("int strategy:%d\n",sasfit_get_int_strategy());
	    switch (sasfit_get_int_strategy()) {
	        case LOBATTO: {
	            res = sasfit_GaussLobattoInt(&Kernel_1D,
                            int_start,int_end,epsabs,epsrel, limit,&ferr[0],&cubstruct);
	        }
	        case TANHSINH_1: {
	            res = TanhSinhQuad(&Kernel_1D, &cubstruct,
                            int_start,int_end,7,epsrel,&ferr[0]);
                break;
	        }
	        case TANHSINH_2: {
	            res = qthsh(&Kernel_1D, &cubstruct,
                            int_start,int_end,7,epsrel,&ferr[0]);
                break;
	        }
	        case GSL_GAUSSLEGENDRE: {
                wglfixed = gsl_integration_glfixed_table_alloc(sasfit_eps_get_gausslegendre());
                err = GSL_SUCCESS;
                res = gsl_integration_glfixed(&F, int_start, int_end, wglfixed);
                gsl_integration_glfixed_table_free(wglfixed);
                break;
            }
            case H_CUBATURE: {
                cubxmin[0] = int_start;
                cubxmax[0] = int_end;
                err=hcubature(1, &Kernel_cub_1D,&cubstruct,1, cubxmin, cubxmax,
                        limit, epsabs, epsrel, ERROR_L2,
                        fval, ferr);
                res = fval[0];
                break;
            }
            case P_CUBATURE: {
                cubxmin[0] = int_start;
                cubxmax[0] = int_end;
                err=pcubature(1, &Kernel_cub_1D,&cubstruct,1, cubxmin, cubxmax,
                        limit, epsabs, epsrel, ERROR_L2,
                        fval, ferr);
                res = fval[0];
                break;
                }
            case OOURA_CLENSHAW_CURTIS_QUADRATURE: {
                aw = (scalar *)malloc((lenaw+1)*sizeof(scalar));
                sasfit_intccini(lenaw, aw);
                sasfit_intcc(&Kernel_1D,int_start,int_end,epsrel, lenaw, aw, &res, &ferr[0],&cubstruct);
                free(aw);
                break;
            }
            case OOURA_DOUBLE_EXP_QUADRATURE: {
                aw = (scalar *)malloc((lenaw)*sizeof(scalar));
                sasfit_intdeini(lenaw, GSL_DBL_MIN, epsrel, aw);
                sasfit_intde(&Kernel_1D,int_start,int_end, aw, &res, &ferr[0],&cubstruct);
                free(aw);
                break;
            }
            case GSL_CHEBYSHEV1: {
                wfixed = gsl_integration_fixed_alloc(gsl_integration_fixed_chebyshev, sasfit_eps_get_chebyshev1(), int_start, int_end, sasfit_eps_get_alpha(), sasfit_eps_get_beta());
                err = gsl_integration_fixed(&F, &res, wfixed);
                gsl_integration_fixed_free(wfixed);
                break;
            }
            case GSL_CHEBYSHEV2: {
                wfixed = gsl_integration_fixed_alloc(gsl_integration_fixed_chebyshev2, sasfit_eps_get_chebyshev2(), int_start, int_end, sasfit_eps_get_alpha(), sasfit_eps_get_beta());
                err = gsl_integration_fixed(&F, &res, wfixed);
                gsl_integration_fixed_free(wfixed);
                break;
            }
            case GSL_GEGENBAUER: {
                wfixed = gsl_integration_fixed_alloc(gsl_integration_fixed_gegenbauer, sasfit_eps_get_gegenbauer(), int_start, int_end, sasfit_eps_get_alpha(), sasfit_eps_get_beta());
                err = gsl_integration_fixed(&F, &res, wfixed);
                gsl_integration_fixed_free(wfixed);
                break;
            }
            case GSL_EXPONENTIAL: {
                wfixed = gsl_integration_fixed_alloc(gsl_integration_fixed_exponential, sasfit_eps_get_exponential(), int_start, int_end, sasfit_eps_get_alpha(), sasfit_eps_get_beta());
                err = gsl_integration_fixed(&F, &res, wfixed);
                gsl_integration_fixed_free(wfixed);
                break;
            }
            case GSL_LAGUERRE: {
                wfixed = gsl_integration_fixed_alloc(gsl_integration_fixed_laguerre, sasfit_eps_get_laguerre(), int_start, int_end, sasfit_eps_get_alpha(), sasfit_eps_get_beta());
                err = gsl_integration_fixed(&F, &res, wfixed);
                gsl_integration_fixed_free(wfixed);
                break;
            }
            case GSL_JACOBI: {
                wfixed = gsl_integration_fixed_alloc(gsl_integration_fixed_jacobi, sasfit_eps_get_jacobi(),
                                                     int_start, int_end, sasfit_eps_get_alpha(), sasfit_eps_get_beta());
                err = gsl_integration_fixed(&F, &res, wfixed);
                gsl_integration_fixed_free(wfixed);
                break;
            }
            case GSL_MC_PLAIN: {
                cubxmin[0] = int_start;
                cubxmax[0] = int_end;
                calls = gsl_max(sasfit_eps_get_iter_4_mc(),50);
                gsl_rng_env_setup ();
                T = gsl_rng_default;
                r = gsl_rng_alloc (T);
                gsl_rng_set(r, mySeed);
                gsl_monte_plain_state *s_plain = gsl_monte_plain_alloc(GMC.dim);
                gsl_monte_plain_integrate (&GMC, cubxmin, cubxmax, GMC.dim, calls, r, s_plain,
                                &res, &ferr[0]);
                gsl_monte_plain_free (s_plain);
                break;
            }
            case GSL_MC_MISER: {
                cubxmin[0] = int_start;
                cubxmax[0] = int_end;
                calls = gsl_max(sasfit_eps_get_iter_4_mc(),50);
                gsl_rng_env_setup ();
                T = gsl_rng_default;
                r = gsl_rng_alloc (T);
                gsl_rng_set(r, mySeed);
                gsl_monte_miser_state *s_miser = gsl_monte_miser_alloc(GMC.dim);
                gsl_monte_miser_integrate (&GMC, cubxmin, cubxmax, GMC.dim, calls, r, s_miser,
                                &res, &ferr[0]);
                gsl_monte_miser_free (s_miser);
                break;
            }
            case GSL_MC_VEGAS: {
                cubxmin[0] = int_start;
                cubxmax[0] = int_end;
                calls = gsl_max(sasfit_eps_get_iter_4_mc(),50);
                gsl_rng_env_setup ();
                T = gsl_rng_default;
                r = gsl_rng_alloc (T);
                gsl_rng_set(r, mySeed);
                gsl_monte_vegas_state *s_vegas = gsl_monte_vegas_alloc(GMC.dim);
                gsl_monte_vegas_integrate (&GMC, cubxmin, cubxmax, GMC.dim, calls/10, r, s_vegas,
                                &res, &ferr[0]);
                i=1;
                do
                {
                    err=gsl_monte_vegas_integrate (&GMC, cubxmin, cubxmax, GMC.dim, calls/10, r, s_vegas,
                                    &res, &ferr[0]);
                    i++;
                } while (i<10 && ((fabs (gsl_monte_vegas_chisq (s_vegas) - 1.0) > 0.5) ||  ferr[0]/res>sasfit_eps_get_aniso()));
//                sasfit_out("VEGAS: number of calls:%d\t ferr/fval=%lf\n",i*calls/10,ferr[0]/fval[0]);
                gsl_monte_vegas_free (s_vegas);
                break;
            }
            default: err = gsl_integration_qags(&F, int_start, int_end, epsabs, epsrel, limit, sasfit_int_mem(thid), &res, &errabs);
	    }

	} else {
		sasfit_err("Erroneous specification of integration intervals!\n");
		err = 1; // error
	}

	sasfit_int_release(thid);

	if (err)
	{
                sasfit_param_set_err(param,
                    DBGINFO("Could not properly perform integration of %x "
                            "over [%e, %e]: %s\n"), intKern_fct,
                    int_start, int_end, gsl_strerror(err));
                return SASFIT_RETURNVAL_ON_ERROR;
	}

	return res;
}

typedef struct
{
	void   *fparams; //!< Flag to enable overriding.
	double (* function) (double x, void *);
	double nu;
	double Q;
} sasfit_param_FBT;

void *FBTparams_static;
double f_FBT(double Qr,void *FBT_param_void) {
    sasfit_param_FBT *FBT_param;
    FBT_param = (sasfit_param_FBT *) FBT_param_void;
    return Qr*FBT_param->function(Qr,FBT_param->fparams);
    return Qr/gsl_pow_2(FBT_param->Q) * FBT_param->function(Qr/FBT_param->Q,FBT_param->fparams);
}

double intdeo_FBT(double r, void *FBTparams) {
    sasfit_param_FBT *FBT_param;
    FBT_param = (sasfit_param_FBT *) FBTparams;
    if (r==0) return 0;
    return r*gsl_sf_bessel_Jnu(FBT_param->nu,FBT_param->Q*r) * FBT_param->function(r,FBT_param->fparams);
}

double intQAWF_FBT(double x, void *FBTparams) {
    scalar phi0;
    sasfit_param_FBT *FBT_param;
    FBT_param = (sasfit_param_FBT *) FBTparams;
    phi0 = M_PI_4*(1.+FBT_param->nu*2.);
    if (gsl_finite(1./cos(x*FBT_param->Q))) {
        return (x+phi0/FBT_param->Q)*FBT_param->function((x+phi0/FBT_param->Q),FBT_param->fparams)
            * gsl_sf_bessel_Jnu(FBT_param->nu,FBT_param->Q*x+phi0)
            / cos(x*FBT_param->Q);
    } else {
        return (x+phi0/FBT_param->Q)*FBT_param->function((x+phi0/FBT_param->Q),FBT_param->fparams)
            * ( gsl_sf_bessel_Jnu(FBT_param->nu,FBT_param->Q*x+phi0+1e-6)
                    / cos(x*FBT_param->Q+1e-6) +
                gsl_sf_bessel_Jnu(FBT_param->nu,FBT_param->Q*x+phi0-1e-6)
                    / cos(x*FBT_param->Q-1e-6) )/2.;
    }
    return (x+phi0/FBT_param->Q)*sqrt(M_2_PI/((x+phi0/FBT_param->Q)*FBT_param->Q))* FBT_param->function((x+phi0/FBT_param->Q),FBT_param->fparams);
}

scalar DEtransform(scalar t) {
    return t * tanh(M_PI_2 * sinh(t));
}

scalar deriv_DEtransform(scalar t){
    scalar res;
    res = 1. / cosh(M_PI_2 * sinh(t));
    res = M_PI_2 * t * cosh(t) * res * res + tanh(M_PI_2 * sinh(t));
    return res;
}

scalar sasfit_hankel(double nu, double (*f)(double, void *), double x, void *fparams) {
    scalar *aw, res,err,eps_nriq;
    scalar zeros_PI, phi_dot,y_k,w_nv_k,J_nv, sum,nv,h;
    unsigned int i;
    int lenaw=4000;
    int status, na;
    scalar a, abserr, phi0, res0, err0;
    size_t limit = 4000;
    scalar lambda;
    static const scalar WJ0Fast[61] = {
            3.30220475766E-04, -1.18223623458E-03,  2.01879495264E-03, -2.13218719891E-03,
            1.60839063172E-03, -9.09156346708E-04,  4.37889252738E-04, -1.55298878782E-04,
            7.98411962729E-05,  4.37268394072E-06,  3.94253441247E-05,  4.02675924344E-05,
            5.66053344653E-05,  7.25774926389E-05,  9.55412535465E-05,  1.24699163157E-04,
            1.63262166579E-04,  2.13477133718E-04,  2.79304232173E-04,  3.65312787897E-04,
            4.77899413107E-04,  6.25100170825E-04,  8.17726956451E-04,  1.06961339341E-03,
            1.39920928148E-03,  1.83020380399E-03,  2.39417015791E-03,  3.13158560774E-03,
            4.09654426763E-03,  5.35807925630E-03,  7.00889482693E-03,  9.16637526490E-03,
            1.19891721272E-02,  1.56755740646E-02,  2.04953856060E-02,  2.67778388247E-02,
            3.49719672729E-02,  4.55975312615E-02,  5.93498881451E-02,  7.69179091244E-02,
            9.91094769804E-02,  1.26166963993E-01,  1.57616825575E-01,  1.89707800260E-01,
            2.13804195282E-01,  2.08669340316E-01,  1.40250562745E-01, -3.65385242807E-02,
            -2.98004010732E-01, -4.21898149249E-01,  5.94373771266E-02,  5.29621428353E-01,
            -4.41362405166E-01,  1.90355040550E-01, -6.19966386785E-02,  1.87255115744E-02,
            -5.68736766738E-03,  1.68263510609E-03, -4.38587145792E-04,  8.59117336292E-05,
            -9.15853765160E-06 };
    static const scalar aJ0Fast = -5.08250000000E+00, sJ0Fast = 1.16638303862E-01;

    static const scalar WJ0[120] = {
            9.62801364263E-07, -5.02069203805E-06,  1.25268783953E-05, -1.99324417376E-05,
            2.29149033546E-05, -2.04737583809E-05,  1.49952002937E-05, -9.37502840980E-06,
            5.20156955323E-06, -2.62939890538E-06,  1.26550848081E-06, -5.73156151923E-07,
            2.76281274155E-07, -1.09963734387E-07,  7.38038330280E-08, -9.31614600001E-09,
            3.87247135578E-08,  2.10303178461E-08,  4.10556513877E-08,  4.13077946246E-08,
            5.68828741789E-08,  6.59543638130E-08,  8.40811858728E-08,  1.01532550003E-07,
            1.26437360082E-07,  1.54733678097E-07,  1.91218582499E-07,  2.35008851918E-07,
            2.89750329490E-07,  3.56550504341E-07,  4.39299297826E-07,  5.40794544880E-07,
            6.66136379541E-07,  8.20175040653E-07,  1.01015545059E-06,  1.24384500153E-06,
            1.53187399787E-06,  1.88633707689E-06,  2.32307100992E-06,  2.86067883258E-06,
            3.52293208580E-06,  4.33827546442E-06,  5.34253613351E-06,  6.57906223200E-06,
            8.10198829111E-06,  9.97723263578E-06,  1.22867312381E-05,  1.51305855976E-05,
            1.86329431672E-05,  2.29456891669E-05,  2.82570465155E-05,  3.47973610445E-05,
            4.28521099371E-05,  5.27705217882E-05,  6.49856943660E-05,  8.00269662180E-05,
            9.85515408752E-05,  1.21361571831E-04,  1.49454562334E-04,  1.84045784500E-04,
            2.26649641428E-04,  2.79106748890E-04,  3.43716968725E-04,  4.23267056591E-04,
            5.21251001943E-04,  6.41886194381E-04,  7.90483105615E-04,  9.73420647376E-04,
            1.19877439042E-03,  1.47618560844E-03,  1.81794224454E-03,  2.23860214971E-03,
            2.75687537633E-03,  3.39471308297E-03,  4.18062141752E-03,  5.14762977308E-03,
            6.33918155348E-03,  7.80480111772E-03,  9.61064602702E-03,  1.18304971234E-02,
            1.45647517743E-02,  1.79219149417E-02,  2.20527911163E-02,  2.71124775541E-02,
            3.33214363101E-02,  4.08864842127E-02,  5.01074356716E-02,  6.12084049407E-02,
            7.45146949048E-02,  9.00780900611E-02,  1.07940155413E-01,  1.27267746478E-01,
            1.46676027814E-01,  1.62254276550E-01,  1.68045766353E-01,  1.52383204788E-01,
            1.01214136498E-01, -2.44389126667E-03, -1.54078468398E-01, -3.03214415655E-01,
            -2.97674373379E-01,  7.93541259524E-03,  4.26273267393E-01,  1.00032384844E-01,
            -4.94117404043E-01,  3.92604878741E-01, -1.90111691178E-01,  7.43654896362E-02,
            -2.78508428343E-02,  1.09992061155E-02, -4.69798719697E-03,  2.12587632706E-03,
            -9.81986734159E-04,  4.44992546836E-04, -1.89983519162E-04,  7.31024164292E-05,
            -2.40057837293E-05,  6.23096824846E-06, -1.12363896552E-06,  1.04470606055E-07 };
    static const scalar aJ0=-8.38850000000E+00, sJ0 = 9.0422646867E-02;

    static const scalar WJ1Fast[47] = {
            3.17926147465E-06, -9.73811660718E-06,  1.64866227408E-05, -1.81501261160E-05,
            1.87556556369E-05, -1.46550406038E-05,  1.53799733803E-05, -6.95628273934E-06,
            1.41881555665E-05,  3.41445665537E-06,  2.13941715512E-05,  2.34962369042E-05,
            4.84340283290E-05,  7.33732978590E-05,  1.27703784430E-04,  2.08120025730E-04,
            3.49803898913E-04,  5.79107814687E-04,  9.65887918451E-04,  1.60401273703E-03,
            2.66903777685E-03,  4.43111590040E-03,  7.35631696247E-03,  1.21782796293E-02,
            2.01097829218E-02,  3.30096953061E-02,  5.37143591532E-02,  8.60516613299E-02,
            1.34267607144E-01,  2.00125033067E-01,  2.74027505792E-01,  3.18168749246E-01,
            2.41655667461E-01, -5.40549161658E-02, -4.46912952135E-01, -1.92231885629E-01,
            5.52376753950E-01, -3.57429049025E-01,  1.41510519002E-01, -4.61421935309E-02,
            1.48273761923E-02, -5.07479209193E-03,  1.83829713749E-03, -6.67742804324E-04,
            2.21277518118E-04, -5.66248732755E-05,  7.88229202853E-06 };
    static const scalar aJ1Fast = -3.05078187595E+00, sJ1Fast = 1.10599010095E-01;

    static const scalar WJ1[140] = {
            -6.76671159511E-14,  3.39808396836E-13, -7.43411889153E-13,  8.93613024469E-13,
            -5.47341591896E-13, -5.84920181906E-14,  5.20780672883E-13, -6.92656254606E-13,
             6.88908045074E-13, -6.39910528298E-13,  5.82098912530E-13, -4.84912700478E-13,
             3.54684337858E-13, -2.10855291368E-13,  1.00452749275E-13,  5.58449957721E-15,
            -5.67206735175E-14,  1.09107856853E-13, -6.04067500756E-14,  8.84512134731E-14,
             2.22321981827E-14,  8.38072239207E-14,  1.23647835900E-13,  1.44351787234E-13,
             2.94276480713E-13,  3.39965995918E-13,  6.17024672340E-13,  8.25310217692E-13,
             1.32560792613E-12,  1.90949961267E-12,  2.93458179767E-12,  4.33454210095E-12,
             6.55863288798E-12,  9.78324910827E-12,  1.47126365223E-11,  2.20240108708E-11,
             3.30577485691E-11,  4.95377381480E-11,  7.43047574433E-11,  1.11400535181E-10,
             1.67052734516E-10,  2.50470107577E-10,  3.75597211630E-10,  5.63165204681E-10,
             8.44458166896E-10,  1.26621795331E-09,  1.89866561359E-09,  2.84693620927E-09,
             4.26886170263E-09,  6.40104325574E-09,  9.59798498616E-09,  1.43918931885E-08,
             2.15798696769E-08,  3.23584600810E-08,  4.85195105813E-08,  7.27538583183E-08,
             1.09090191748E-07,  1.63577866557E-07,  2.45275193920E-07,  3.67784458730E-07,
             5.51470341585E-07,  8.26916206192E-07,  1.23991037294E-06,  1.85921554669E-06,
             2.78777669034E-06,  4.18019870272E-06,  6.26794044911E-06,  9.39858833064E-06,
             1.40925408889E-05,  2.11312291505E-05,  3.16846342900E-05,  4.75093313246E-05,
             7.12354794719E-05,  1.06810848460E-04,  1.60146590551E-04,  2.40110903628E-04,
             3.59981158972E-04,  5.39658308918E-04,  8.08925141201E-04,  1.21234066243E-03,
             1.81650387595E-03,  2.72068483151E-03,  4.07274689463E-03,  6.09135552241E-03,
             9.09940027636E-03,  1.35660714813E-02,  2.01692550906E-02,  2.98534800308E-02,
             4.39060697220E-02,  6.39211368217E-02,  9.16763946228E-02,  1.28368795114E-01,
             1.73241920046E-01,  2.19830379079E-01,  2.51193131178E-01,  2.32380049895E-01,
             1.17121080205E-01, -1.17252913088E-01, -3.52148528535E-01, -2.71162871370E-01,
             2.91134747110E-01,  3.17192840623E-01, -4.93075681595E-01,  3.11223091821E-01,
            -1.36044122543E-01,  5.12141261934E-02, -1.90806300761E-02,  7.57044398633E-03,
            -3.25432753751E-03,  1.49774676371E-03, -7.24569558272E-04,  3.62792644965E-04,
            -1.85907973641E-04,  9.67201396593E-05, -5.07744171678E-05,  2.67510121456E-05,
            -1.40667136728E-05,  7.33363699547E-06, -3.75638767050E-06,  1.86344211280E-06,
            -8.71623576811E-07,  3.61028200288E-07, -1.05847108097E-07, -1.51569361490E-08,
             6.67633241420E-08, -8.33741579804E-08,  8.31065906136E-08, -7.53457009758E-08,
             6.48057680299E-08, -5.37558016587E-08,  4.32436265303E-08, -3.37262648712E-08,
             2.53558687098E-08, -1.81287021528E-08,  1.20228328586E-08, -7.10898040664E-09,
             3.53667004588E-09, -1.36030600198E-09,  3.52544249042E-10, -4.53719284366E-11 };
    static const scalar aJ1=-7.91001919000E+00, sJ1 =  8.7967143957E-02;

    sasfit_param_FBT FBTparam;
    FBTparam.fparams=fparams;
    FBTparam.function=f;
    FBTparam.nu=nu;
    FBTparam.Q=x;
    FBTparams_static = &FBTparam;

    switch (sasfit_get_hankel_strategy()) {
        case HANKEL_OOURA_DEO: {

            na = lround(sasfit_get_N_Ogata());
            a=gsl_sf_bessel_zero_Jnu(nv,(na<10?na:10))/FBTparam.Q;

            aw = (scalar *)malloc((lenaw)*sizeof(scalar));
            eps_nriq=sasfit_eps_get_nriq();
            sasfit_intdeini(lenaw, GSL_DBL_MIN, eps_nriq, aw);
            sasfit_intde(&intdeo_FBT,0, a, aw, &res0, &err0, &FBTparam);

            sasfit_intdeoini(lenaw, GSL_DBL_MIN, eps_nriq, aw);
            sasfit_intdeo(&intdeo_FBT,a, FBTparam.Q, aw, &res, &err, &FBTparam);
            free(aw);
            res += res0;
            err += err0;
            break;
        }
        case HANKEL_OGATA_2005: {
            sum = 0.0;
            nv = fabs(nu);
            h = sasfit_get_h_Ogata();
            for (i=1;i<=sasfit_get_N_Ogata();i++) {
                zeros_PI = gsl_sf_bessel_zero_Jnu(nv,i) / M_PI;
                phi_dot = deriv_DEtransform(h * zeros_PI);
                y_k = DEtransform(h*zeros_PI) * M_PI / h;
                // w_nv_k = gsl_sf_bessel_Ynu(nv, zeros_PI * M_PI) / gsl_sf_bessel_Jnu(nv + 1, zeros_PI * M_PI);
                w_nv_k = 2./ (gsl_pow_2(M_PI*gsl_sf_bessel_Jnu(nv+1,zeros_PI*M_PI)) * zeros_PI);
                J_nv = gsl_sf_bessel_Jnu(nv, y_k);
                res =  w_nv_k * y_k * (*f)(y_k / x,fparams) * J_nv * phi_dot;
                sum = sum+res;
            }
            // sum = 0.5 / gsl_pow_2(x) * sum;
            sum = M_PI / gsl_pow_2(x) * sum;
            if (nv==0) {
                res = sum;
            } else {
                res = sum * pow(GSL_SIGN(nu),nu);
            }
            break;
        }
        case HANKEL_FBT0: {
            sasfit_set_FBT(nu, 0, sasfit_get_N_Ogata(), 1.0e-2);
            res = sasfit_FBT(x, f_FBT, &FBTparam);
            break;
        }
        case HANKEL_FBT1: {
            sasfit_set_FBT(nu, 1, sasfit_get_N_Ogata(), 1.0e-2);
            res = sasfit_FBT(x, f_FBT, &FBTparam);
            break;
        }
        case HANKEL_FBT2: {
            sasfit_set_FBT(nu, 2, sasfit_get_N_Ogata(), 1.0e-2);
            res = sasfit_FBT(x, f_FBT, &FBTparam);
            break;
        }
        case HANKEL_GSL_QAWF: {
            phi0 = M_PI/4.*(1.+nu*2.);
            na = lround(sasfit_get_N_Ogata());
            a=gsl_sf_bessel_zero_Jnu(nv,(na<10?na:10))/FBTparam.Q;

            aw = (scalar *)malloc((lenaw)*sizeof(scalar));
            eps_nriq=sasfit_eps_get_nriq();
            sasfit_intdeini(lenaw, GSL_DBL_MIN, eps_nriq, aw);
            sasfit_intde(&intdeo_FBT,0, a, aw, &res0, &err, &FBTparam);
            free(aw);

            gsl_function F;
            F.params = &FBTparam;
            F.function = (double (*) (double, void*)) &intQAWF_FBT;
            gsl_integration_workspace * w = gsl_integration_workspace_alloc (limit);
            gsl_integration_workspace * w_cycle = gsl_integration_workspace_alloc (limit);
            gsl_integration_qawo_table * wf=gsl_integration_qawo_table_alloc (FBTparam.Q, 1.0, GSL_INTEG_COSINE, 200);
            status = gsl_integration_qawf (&F,a-phi0/FBTparam.Q,eps_nriq*3,limit,w, w_cycle, wf, &res, &abserr);
            if (status != GSL_SUCCESS) sasfit_out("Q:%lf\n",FBTparam.Q);
            gsl_integration_qawo_table_free (wf);
            gsl_integration_workspace_free(w);
            gsl_integration_workspace_free(w_cycle);
            res = res+res0;
            break;
        }
        case HANKEL_SINC_SEO: {
            scalar seta, sK, sh, sc, sni, sw, sH, st, sF0, sF1, stau, sq, sder, rN, rN0, rN1;
            int v, sM, sN, sN0, sN1, nval, k, kmax, l, lmax;
            seta = sasfit_eps_get_nriq();
            sM   = lround(ceil(-5*log10(seta)));
            sni  = FBTparam.nu;
            sw = FBTparam.Q;
            sK = pow(M_PI,(sni+2)) /( sw*sw*pow(2,sni) * gsl_sf_gamma(sni+1) *(sni+2) );
            sh = 1.0/((sni +2) * sM) * log(sK * pow(sM,(sni +2))/seta );
            sc = sqrt(2)/16.0/(sw*sw)*fabs(4*sni*sni-1);
            #define FUN(X) (sc * f(fabs(X*M_PI/sw) , fparams) / sqrt(X))
            #define PHI(X) (X/(1 - exp(-X)))
            #define PHIP(X) ((1 - exp(-X)-X*exp(-X))/(1-exp(-X)))
            #define F(t) f(fabs(stau/sw *PHI(t-sq)),fparams) *gsl_pow_2(stau/sw)*PHI(st-sq) * gsl_sf_bessel_Jnu(sni, stau*PHI(t-sq)) * PHIP(t-sq)
            stau = M_PI / sh;
            sq = M_PI /(4*stau) *(1-2*sni);
            sN0=5;
            rN0=5;
            sder = 0;
            lmax = 8;
            l=0;
            while (sder >= 0 && l<lmax) {
                rN0 *= 2;
                rN = rN0;
                rN1= rN0+1;
                st = 1;
                kmax = 100;
                sF0 = FUN(rN);
                sF1 = FUN(rN1);
                k = 1;
                while (st > seta && k < kmax && fabs(sF1) >= seta) {
                    sder = (sF1-sF0)/(rN1-rN0);
                    if (sder >= 0) {
                        sasfit_out("solution not found , increase N0=%lg\n",rN0);
                        break;
                    }
                    rN  = rN - (sF1 - seta)/sder ;
                    rN0 = rN1;
                    sF0 = sF1;
                    rN1 = rN;
                    sF1 = FUN(rN) ;
                    st  = fabs (sF1 - seta);
                    k++;
                }
            }
            sH = 0;
            sN = lround(rN);
            for (v=-sM;v<=sN;v++) {
                sH+= F(v*sh);
            }
            sH *= sh;
            res = sH;
            err = seta;
            nval = sM+sN+k+1;
            sasfit_out("needed %d function evaluations.\n",nval);
            break;
        }
        case HANKEL_GUPTASARMA_97_FAST: {
            if (!(nu==0 || nu==1)) {
                sasfit_err("GUPTASARMA_97_FAST strategy is only available for Hankel transforms J0 and J1\n");
                return 0;
            }
            if (nu==0) {
                res = 0;
                for (i=0;i<61;i++) {
                    lambda =pow(10.0E0,(aJ0Fast + i*sJ0Fast))/x;
                    res = res+(*f)(lambda,fparams)*lambda*WJ0Fast[i]/x;
                }
            } else {
                res = 0;
                for (i=0;i<47;i++) {
                    lambda =pow(10.0E0,(aJ1Fast + i*sJ1Fast))/x;
                    res = res+(*f)(lambda,fparams)*lambda*WJ0Fast[i]/x;
                }
            }
            break;
        }
        case HANKEL_GUPTASARMA_97: {
            if (!(nu==0 || nu==1)) {
                sasfit_err("GUPTASARMA_97_FAST strategy is only available for Hankel transforms J0 and J1\n");
                return 0;
            }
            if (nu==0) {
                res = 0;
                for (i=0;i<120;i++) {
                    lambda =pow(10.0E0,(aJ0 + i*sJ0))/x;
                    res = res+(*f)(lambda,fparams)*lambda*WJ0[i]/x;
                }
            } else {
                res = 0;
                for (i=0;i<140;i++) {
                    lambda =pow(10.0E0,(aJ1 + i*sJ1))/x;
                    res = res+(*f)(lambda,fparams)*lambda*WJ1[i]/x;
                }
            }
            break;
        }
        case HANKEL_KEY_51: {
            if (!(nu==0 || nu==1)) {
                sasfit_err("KEY_51 strategy is only available for Hankel transforms J0 and J1\n");
                return 0;
            }
            if (nu==0) {
                res = 0;
                for (i=0;i<51;i++) {
                    lambda =KK51Hankel[i][0]/x;
                    res = res+(*f)(lambda,fparams)*lambda*KK51Hankel[i][1]/x;
                }
            } else {
                res = 0;
                for (i=0;i<51;i++) {
                    lambda =KK51Hankel[i][0]/x;
                    res = res+(*f)(lambda,fparams)*lambda*KK51Hankel[i][2]/x;
                }
            }
            break;
        }
        case HANKEL_KEY_101: {
            if (!(nu==0 || nu==1)) {
                sasfit_err("KEY_101 strategy is only available for Hankel transforms J0 and J1\n");
                return 0;
            }
            if (nu==0) {
                res = 0;
                for (i=0;i<101;i++) {
                    lambda =KK101Hankel[i][0]/x;
                    res = res+(*f)(lambda,fparams)*lambda*KK101Hankel[i][1]/x;
                }
            } else {
                res = 0;
                for (i=0;i<101;i++) {
                    lambda =KK101Hankel[i][0]/x;
                    res = res+(*f)(lambda,fparams)*lambda*KK101Hankel[i][2]/x;
                }
            }
            break;
        }
        case HANKEL_KEY_201: {
            if (!(nu==0 || nu==1)) {
                sasfit_err("KEY_201 strategy is only available for Hankel transforms J0 and J1\n");
                return 0;
            }
            if (nu==0) {
                res = 0;
                for (i=0;i<201;i++) {
                    lambda =KK201Hankel[i][0]/x;
                    res = res+(*f)(lambda,fparams)*lambda*KK201Hankel[i][1]/x;
                }
            } else {
                res = 0;
                for (i=0;i<201;i++) {
                    lambda =KK201Hankel[i][0]/x;
                    res = res+(*f)(lambda,fparams)*lambda*KK201Hankel[i][2]/x;
                }
            }
            break;
        }
        case HANKEL_ANDERSON_801: {
            if (!(nu==0 || nu==1)) {
                sasfit_err("WA_801 strategy is only available for Hankel transforms J0 and J1\n");
                return 0;
            }
            if (nu==0) {
                res = 0;
                for (i=0;i<801;i++) {
                    lambda =WA801Hankel[i][0]/x;
                    res = res+(*f)(lambda,fparams)*lambda*WA801Hankel[i][1]/x;
                }
            } else {
                res = 0;
                for (i=0;i<801;i++) {
                    lambda =WA801Hankel[i][0]/x;
                    res = res+(*f)(lambda,fparams)*lambda*WA801Hankel[i][2]/x;
                }
            }
            break;
        }
        case HANKEL_QWE: {
            res = sasfit_qwe(0, f, x, fparams, lenaw, sasfit_eps_get_nriq()*10, DBL_MIN);
            break;
        }
        case HANKEL_CHAVE: {
            res = sasfit_HankelChave(0, f,x, fparams, lenaw, sasfit_eps_get_nriq()*10, DBL_MIN);
            break;
        }
        default:{
                aw = (scalar *)malloc((lenaw)*sizeof(scalar));
                eps_nriq=sasfit_eps_get_nriq();
                sasfit_intdeoini(lenaw, GSL_DBL_MIN, eps_nriq, aw);
                sasfit_intdeo(&intdeo_FBT,0, FBTparam.Q, aw, &res, &err, &FBTparam);
                free(aw);
                break;
        }
    }
    return res;
}

scalar sasfit_clipped_hankel(double nu, double a, double b, double (*f)(double, void *), double x, void *fparams) {
    return 0;
}

scalar FrSincTinyQ(scalar r, sasfit_param * param) {
    scalar Q;
    Q  = param->more_p[1];
    return 4*M_PI*r*r*gsl_sf_bessel_j0(Q*r)*param->function(r,param->moreparam);
}

scalar FrSinc(scalar r, sasfit_param * param) {
    scalar Q;
    Q  = param->more_p[1];
    return 4*M_PI*r/Q*param->function(r,param->moreparam);
}

scalar sasfit_clipped_sinc_quad(double a, double b, double (*f)(double, void *), double x, void *fparams) {
    sasfit_param param;
    double res,abserr,eps_nriq;
    int limit=4000, status;

	param.function = f;
	param.more_p[1] = x;
    param.moreparam=fparams;
    eps_nriq=sasfit_eps_get_nriq();
    if (fabs(x*b) < 2*M_PI) return sasfit_integrate(a,b,&FrSincTinyQ,&param);

    gsl_set_error_handler_off();
    gsl_function F;
    F.params = &param;
    F.function = (double (*) (double, void*)) &FrSinc;
    gsl_integration_workspace * w = gsl_integration_workspace_alloc (limit);
    gsl_integration_qawo_table * wf=gsl_integration_qawo_table_alloc (x, b-a, GSL_INTEG_SINE, limit);
    status = gsl_integration_qawo (&F,b-a,0,eps_nriq*3,limit,w, wf, &res, &abserr);
    if (status != GSL_SUCCESS) {
        sasfit_err("integration failed for Q=%lf\n gsl_strerror(%d)\n",x,status);
    }
    gsl_integration_qawo_table_free (wf);
    gsl_integration_workspace_free(w);
    return res;
}

scalar sasfit_clipped_cos_quad(double a, double b, double (*f)(double, void *), double x, void *fparams) {
    sasfit_param param;
    double res,abserr,eps_nriq;
    int limit=4000, status;

	param.function = f;
	param.more_p[1] = x;
    param.moreparam=fparams;
    eps_nriq=sasfit_eps_get_nriq();
    gsl_set_error_handler_off();
    gsl_function F;
    F.params = fparams;
    F.function = f;
    gsl_integration_workspace * w = gsl_integration_workspace_alloc (limit);
    gsl_integration_qawo_table * wf=gsl_integration_qawo_table_alloc (x, b-a, GSL_INTEG_COSINE, limit);
    status = gsl_integration_qawo (&F,b-a,0,eps_nriq*3,limit,w, wf, &res, &abserr);
    if (status != GSL_SUCCESS) {
        sasfit_err("integration failed for Q=%lf\n gsl_strerror(%d)\n",x,status);
    }
    gsl_integration_qawo_table_free (wf);
    gsl_integration_workspace_free(w);
    return res;
}
