/*
 * src/sasfit_common/sasfit_integrate.c
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
 *   Ingo Bressler (ingo@cs.tu-berlin.de)
 */


// #include <omp.h>
#include <stddef.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_monte.h>
#include <gsl/gsl_monte_plain.h>
#include <gsl/gsl_monte_miser.h>
#include <gsl/gsl_monte_vegas.h>
#include <gsl/gsl_integration.h>
#include <gsl/gsl_errno.h>
#include "include/sasfit_common.h"
#include "include/sasfit_function.h"

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
	fval[0]=(*cub->Kernel2D_fct)(x[0],x[1],param)*sin(x[0]);
	return 0;
}

scalar Kernel_GL(scalar theta, scalar phi, void *pam) {
	sasfit_param * param;
	int_cub *cub;
	cub = (int_cub *) pam;
	param = (sasfit_param *) cub->param;
	return (*cub->Kernel2D_fct)(theta,phi,param)*sin(theta);
}

scalar Kernel_MC2D(scalar *k, unsigned int dim, void *pam) {
	sasfit_param * param;
	int_cub *cub;
	cub = (int_cub *) pam;
	param = (sasfit_param *) cub->param;
	if (dim <2) sasfit_err("wrong dimension for using Kernel_MC2D (%d), dim needs to be dim>=2\n",dim);
	return (*cub->Kernel2D_fct)(k[0],k[1],param)*sin(k[0]);
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
    int order, rule, lambda_i;
    double ang_x;
    double fact,phi,theta;
    scalar cubxmin[2], cubxmax[2], fval[1], ferr[1];
    gsl_integration_glfixed_table * wglfixed;
    int_cub cubstruct;
    const gsl_rng_type *T;
    gsl_rng *r;
    unsigned int calls;
    gsl_monte_function GMC;
    cubstruct.Kernel2D_fct=intKern_fct;
    cubstruct.param=param;
    GMC.f = &Kernel_MC2D;
    GMC.params=&cubstruct;
    GMC.dim=2;
    Iavg = 0;

    switch (sasfit_get_sphavg_strategy()) {
        case SPHAVG_MC_PLAIN: {
                cubxmin[0] = 0;
                cubxmax[0] = M_PI;
                cubxmin[1] = 0;
                cubxmax[1] = 2*M_PI;
                calls = sasfit_eps_get_iter_4_mc();
                gsl_rng_env_setup ();
                T = gsl_rng_default;
                r = gsl_rng_alloc (T);
                gsl_monte_plain_state *s_plain = gsl_monte_plain_alloc(2);
                gsl_monte_plain_integrate (&GMC, cubxmin, cubxmax, 2, calls/10, r, s_plain,
                               &fval[0], &ferr[0]);
                i=1;
                do
                {
                    gsl_monte_plain_integrate (&GMC, cubxmin, cubxmax, 2, calls/10, r, s_plain,
                                   &fval[0], &ferr[0]);
                    i++;
                } while (i<10 && (ferr[0]/fval[0]>sasfit_eps_get_aniso()));
 //               sasfit_out("PLAIN: number of calls:%d\t ferr/fval=%lf\n",i*calls/10,ferr[0]/fval[0]);
                gsl_monte_plain_free (s_plain);
                Iavg = fval[0]/(4*M_PI);
                break;
            }
        case SPHAVG_MC_VEGAS: {
                cubxmin[0] = 0;
                cubxmax[0] = M_PI;
                cubxmin[1] = 0;
                cubxmax[1] = 2*M_PI;
                calls = sasfit_eps_get_iter_4_mc();
                gsl_rng_env_setup ();
                T = gsl_rng_default;
                r = gsl_rng_alloc (T);
                gsl_monte_vegas_state *s_vegas = gsl_monte_vegas_alloc(2);
                gsl_monte_vegas_integrate (&GMC, cubxmin, cubxmax, 2, calls/10, r, s_vegas,
                               &fval[0], &ferr[0]);
                i=1;
                do
                {
                    gsl_monte_vegas_integrate (&GMC, cubxmin, cubxmax, 2, calls/10, r, s_vegas,
                                   &fval[0], &ferr[0]);
                    i++;
                } while (i<10 && ((fabs (gsl_monte_vegas_chisq (s_vegas) - 1.0) > 0.5) || ferr[0]/fval[0]>sasfit_eps_get_aniso()));
//                sasfit_out("VEGAS: number of calls:%d\t ferr/fval=%lf\n",i*calls/10,ferr[0]/fval[0]);
                gsl_monte_vegas_free (s_vegas);
                Iavg = fval[0]/(4*M_PI);
                break;
            }
        case SPHAVG_MC_MISER: {
                cubxmin[0] = 0;
                cubxmax[0] = M_PI;
                cubxmin[1] = 0;
                cubxmax[1] = 2*M_PI;
                calls = sasfit_eps_get_iter_4_mc();
                gsl_rng_env_setup ();
                T = gsl_rng_default;
                r = gsl_rng_alloc (T);
                gsl_monte_miser_state *s_miser = gsl_monte_miser_alloc(2);
                gsl_monte_miser_integrate (&GMC, cubxmin, cubxmax, 2, calls/10, r, s_miser,
                               &fval[0], &ferr[0]);
                i=1;
                do
                {
                    gsl_monte_miser_integrate (&GMC, cubxmin, cubxmax, 2, calls/10, r, s_miser,
                                   &fval[0], &ferr[0]);
                    i++;
                } while (i<10 && ferr[0]/fval[0]>sasfit_eps_get_aniso());
//                sasfit_out("MISER: number of calls:%d\t ferr/fval=%lf\n",i*calls/10,ferr[0]/fval[0]);
                gsl_monte_miser_free (s_miser);
                Iavg = fval[0]/(4*M_PI);
                break;
            }
        case SPHAVG_GSL_2D_GAUSSLEGENDRE: {
                wglfixed = gsl_integration_glfixed_table_alloc(sasfit_eps_get_gausslegendre());
//sasfit_out("GSL_2D_GAUSSLEGENDRE order:%d\n",sasfit_eps_get_gausslegendre());
                Iavg = sasfit_gauss_legendre_2D_cube(&Kernel_GL, &cubstruct, 0, M_PI, 0, 2*M_PI, wglfixed);
                Iavg=Iavg/(4*M_PI);
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
                cubxmin[0] = 0;
                cubxmax[0] = M_PI;
                cubxmin[1] = 0;
                cubxmax[1] = 2*M_PI;
                hcubature(1, &Kernel_cub_2D,&cubstruct,2, cubxmin, cubxmax,
                        limit, epsabs, epsrel, ERROR_PAIRED,
                        fval, ferr);
                Iavg = fval[0]/(4*M_PI);
                break;
            }
        case SPHAVG_PCUBATURE: {
                cubxmin[0] = 0;
                cubxmax[0] = M_PI;
                cubxmin[1] = 0;
                cubxmax[1] = 2*M_PI;
                pcubature(1, &Kernel_cub_2D,&cubstruct,2, cubxmin, cubxmax,
                        limit, epsabs, epsrel, ERROR_PAIRED,
                        fval, ferr);
                Iavg = fval[0]/(4*M_PI);
                break;
            }
        default:
                cubxmin[0] = 0;
                cubxmax[0] = M_PI;
                cubxmin[1] = 0;
                cubxmax[1] = 2*M_PI;
                pcubature(1, &Kernel_cub_2D,&cubstruct,2, cubxmin, cubxmax,
                        limit, epsabs, epsrel, ERROR_PAIRED,
                        fval, ferr);
                Iavg = fval[0]/(4*M_PI);
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
    int lenaw=4000,ierr;

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

	if ( gsl_isinf(int_start) && gsl_finite(int_end) ) 	// adaptive integration on infinite intervals (-\infty,b)
	{
        	err = gsl_integration_qagil(&F, int_end, epsabs, epsrel, limit, sasfit_int_mem(thid), &res, &errabs);
	}
	else if ( gsl_finite(int_start) && gsl_isinf(int_end) )	// adaptive integration on infinite intervals (a,+\infty)
	{
        	err = gsl_integration_qagiu(&F, int_start, epsabs, epsrel, limit, sasfit_int_mem(thid), &res, &errabs);
	}
	else if ( gsl_isinf(int_start) && gsl_isinf(int_end) ) 	// adaptive integration on infinite intervals (-\infty,+\infty)
	{
	    err = gsl_integration_qagi(&F, epsabs, epsrel, limit, sasfit_int_mem(thid), &res, &errabs);
	}
	else if ( gsl_finite(int_start) && gsl_finite(int_end) ) // adaptive integration with singularities
	                                                         // on well defined interval (a,b)
	{
	    switch (sasfit_get_int_strategy()) {
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
                        limit, epsabs, epsrel, ERROR_PAIRED,
                        fval, ferr);
                res = fval[0];
                break;
            }
            case P_CUBATURE: {
                cubxmin[0] = int_start;
                cubxmax[0] = int_end;
                err=pcubature(1, &Kernel_cub_1D,&cubstruct,1, cubxmin, cubxmax,
                        limit, epsabs, epsrel, ERROR_PAIRED,
                        fval, ferr);
                res = fval[0];
                break;
                }
            case OOURA_CLENSHAW_CURTIS_QUADRATURE: {
                aw = (scalar *)malloc((lenaw+1)*sizeof(scalar));
                sasfit_intccini(lenaw, aw);
                sasfit_intcc(&Kernel_1D,int_start,int_end,epsrel, lenaw, aw, &res, &ferr[0],&cubstruct);
                err=0;
                free(aw);
                break;
            }
            case OOURA_DOUBLE_EXP_QUADRATURE: {
                aw = (scalar *)malloc((lenaw)*sizeof(scalar));
                sasfit_intdeini(lenaw, GSL_DBL_MIN, epsrel, aw);
                sasfit_intde(&Kernel_1D,int_start,int_end, aw, &res, &ferr[0],&cubstruct);
                err=0;
                free(aw);
                break;
            }
            case GSL_CHEBYSHEV1: {
                wfixed = gsl_integration_fixed_alloc(gsl_integration_fixed_chebyshev, sasfit_eps_get_chebyshev1(), int_start, int_end, 0, 1);
                err = gsl_integration_fixed(&F, &res, wfixed);
                gsl_integration_fixed_free(wfixed);
                break;
            }
            case GSL_CHEBYSHEV2: {
                wfixed = gsl_integration_fixed_alloc(gsl_integration_fixed_chebyshev2, sasfit_eps_get_chebyshev2(), int_start, int_end, 0, 1);
                err = gsl_integration_fixed(&F, &res, wfixed);
                gsl_integration_fixed_free(wfixed);
                break;
            }
            case GSL_GEGENBAUER: {
                wfixed = gsl_integration_fixed_alloc(gsl_integration_fixed_gegenbauer, sasfit_eps_get_gegenbauer(), int_start, int_end, sasfit_eps_get_aniso(), 1);
                err = gsl_integration_fixed(&F, &res, wfixed);
                gsl_integration_fixed_free(wfixed);
                break;
            }
            case GSL_EXPONENTIAL: {
                wfixed = gsl_integration_fixed_alloc(gsl_integration_fixed_exponential, sasfit_eps_get_exponential(), int_start, int_end, sasfit_eps_get_aniso(), 1);
                err = gsl_integration_fixed(&F, &res, wfixed);
                gsl_integration_fixed_free(wfixed);
                break;
            }
            case GSL_LAGUERRE: {
                wfixed = gsl_integration_fixed_alloc(gsl_integration_fixed_laguerre, sasfit_eps_get_laguerre(), int_start, int_end, sasfit_eps_get_aniso(), 1);
                err = gsl_integration_fixed(&F, &res, wfixed);
                gsl_integration_fixed_free(wfixed);
                break;
            }
            case GSL_JACOBI: {
                wfixed = gsl_integration_fixed_alloc(gsl_integration_fixed_jacobi, sasfit_eps_get_jacobi(),
                                                     int_start, int_end, epsabs, sasfit_eps_get_aniso());
                err = gsl_integration_fixed(&F, &res, wfixed);
                gsl_integration_fixed_free(wfixed);
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
                            "over [%f, %f]: %s\n"), intKern_fct,
                    int_start, int_end, gsl_strerror(err));
                return SASFIT_RETURNVAL_ON_ERROR;
	}

	return res;
}

