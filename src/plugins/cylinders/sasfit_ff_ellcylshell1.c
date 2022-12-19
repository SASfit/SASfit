/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define T	param->p[1]
#define EPSILON	param->p[2]
#define L	param->p[3]
#define ETA_CORE	param->p[4]
#define ETA_SHELL	param->p[5]
#define ETA_SOLV	param->p[6]

#define Q   param->p[MAXPAR-1]
#define TYPE_SHELL  param->p[MAXPAR-2]
#define THETA param->p[MAXPAR-3]
#define ALPHA param->p[MAXPAR-4]
#define P param->p[MAXPAR-5]

scalar ell_cylshell1_avg(scalar theta, scalar phi, sasfit_param *param) {
    scalar x[2];
    x[0]=theta;
    x[1]=phi;
    return A_ellcyl(x,2,param);
}

scalar sasfit_ff_ellcylshell1(scalar q, sasfit_param * param)
{   int ierr;
    scalar res,err;
    scalar cubxmin[2], cubxmax[2];

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((T < 0.0), param, "t(%lg) < 0",T); // modify condition to your needs
	SASFIT_CHECK_COND1((EPSILON <= 0.0), param, "epsilon(%lg) <= 0",EPSILON); // modify condition to your needs
	SASFIT_CHECK_COND1((L < 0.0), param, "L(%lg) < 0",L); // modify condition to your needs

	// insert your code here
    Q=q;
    P=2;
    TYPE_SHELL=0;
    if (EPSILON==1) {
        cubxmin[0]=0;
        cubxmax[0]=M_PI_2;
        ierr = sasfit_cubature(1,cubxmin,cubxmax,&A_ellcyl,param,sasfit_eps_get_nriq(),&res, &err);
        return res;
    } else {
        sasfit_param_set_polar_phi(M_PI_2);
        sasfit_param_set_polar_theta(M_PI_2);
        return sasfit_orient_avg(&ell_cylshell1_avg,param);
    }
}

scalar sasfit_ff_ellcylshell1_f(scalar q, sasfit_param * param)
{   int ierr;
    scalar res,err;
    scalar cubxmin[2], cubxmax[2];
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
    Q=q;
    P=1;
    TYPE_SHELL=0;
    if (EPSILON==1) {
        cubxmin[0]=0;
        cubxmax[0]=M_PI_2;
        ierr = sasfit_cubature(1,cubxmin,cubxmax,&A_ellcyl,param,sasfit_eps_get_nriq(),&res, &err);
        return res;
    } else {
        sasfit_param_set_polar_phi(M_PI_2);
        sasfit_param_set_polar_theta(M_PI_2);
        return sasfit_orient_avg(&ell_cylshell1_avg,param);
    }
}

scalar sasfit_ff_ellcylshell1_v(scalar x, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	switch (dist) {
    case 0: // R	param->p[0]
        return M_PI*(x+T)*(x*EPSILON+T)*L;
    case 1: // T	param->p[1]
        return M_PI*(R+x)*(R*EPSILON+x)*L;
    case 2: // EPSILON	param->p[2]
        return M_PI*(R+T)*(R*x+T)*L;
    case 3: // L	param->p[3]
        return M_PI*(R+T)*(R*EPSILON+T)*x;
    default:
        return M_PI*(R+T)*(R*EPSILON+T)*L;
	}
}

