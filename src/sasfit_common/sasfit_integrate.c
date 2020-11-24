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
#include <gsl/gsl_integration.h>
#include <gsl/gsl_errno.h>
#include "include/sasfit_common.h"

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

double sasfit_gauss_legendre_2D_cube(sasfit_func_two_t f,
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

scalar sasfit_orient_avg_ctm(
			sasfit_func_two_t intKern_fct,
			sasfit_param * param,
			int limit,
			scalar epsabs,
			scalar epsrel)
{   double Iavg;
    int i;
    gsl_integration_glfixed_table * wglfixed;
    Iavg = 0;
    switch (sasfit_get_sphavg_strategy()) {
        case SPHAVG_GSL_2D_GAUSSLEGENDRE: {
                wglfixed = gsl_integration_glfixed_table_alloc(sasfit_eps_get_gausslegendre());
                sasfit_gauss_legendre_2D_cube(intKern_fct, param, 0, M_PI, 0, 2*M_PI, wglfixed);
                gsl_integration_glfixed_table_free(wglfixed);
                break;
            }
        case SPHAVG_Lebedev: {
                double *w, *x,*y,*z;
                int order, rule;
                double ang_x;
                double fact,phi,theta;
                rule = sasfit_eps_get_lebedev();
                if (rule < 1) rule = 1;
                if (rule > 65) rule = 65;
                order = sasfit_order_table ( rule );
                w = ( double * ) malloc ( order * sizeof ( double ) );
                x = ( double * ) malloc ( order * sizeof ( double ) );
                y = ( double * ) malloc ( order * sizeof ( double ) );
                z = ( double * ) malloc ( order * sizeof ( double ) );
                sasfit_ld_by_order ( order, x, y, z, w );

                for (i=0;i<order;i++) {
                    phi = acos ( z[i] );
                    fact = sqrt ( x[i] * x[i] + y[i] * y[i] );
                    if ( 0 < fact ) {
                        ang_x = acos ( x[i] / fact );
                    } else {
                        ang_x = acos ( x[i] );
                    }
                    if ( y[i] < 0 ) {
                        ang_x = - ang_x;
                    }
                    theta = ang_x;
                    Iavg = Iavg+w[i]* intKern_fct(theta,phi,param);
                }
                free ( x );
                free ( y );
                free ( z );
                free ( w );
                break;
            }
        case SPHAVG_FIBONACCI: {

                break;
            }
        case SPHAVG_HCUBATURE: {

                break;
            }
        case SPHAVG_PCUBATURE: {

                break;
            }
        default: break;
	    }
	    return Iavg;

}
scalar sasfit_integrate_ctm(scalar int_start,
			scalar int_end,
			sasfit_func_one_t intKern_fct,
			sasfit_param * param,
			int limit,
			scalar epsabs,
			scalar epsrel)
{
	scalar res, errabs;
	int err, thid;
    scalar *aw;
    scalar cubxmin[1], cubxmax[1], fval[1], ferr[1];
    gsl_integration_cquad_workspace * wcquad;
    gsl_integration_glfixed_table * wglfixed;
    gsl_integration_fixed_workspace * wfixed;
	gsl_function F;
    size_t neval;
    int lenaw=4000,ierr;

	SASFIT_ASSERT_PTR(param);
	SASFIT_ASSERT_PTR(intKern_fct);

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

