/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define NUNU	param->p[0]
#define R_EQUATORIAL	param->p[1]
#define T	param->p[3]

scalar sasfit_ff_spheroid_nu_core(sasfit_param * param) {
	B = R_EQUATORIAL;
	A = NUNU*B;
	TNU = 0;
	return LNDISTR*gsl_sf_pow_int(F_EllSh(param),lround(P));
}

scalar sasfit_ff_spheroid_nu_core_MU(scalar x, sasfit_param * param)
{
	scalar res;

	SASFIT_ASSERT_PTR(param);

	MU = x;
	LNDISTR = 1;
	NU = 1;
	res = sasfit_ff_spheroid_nu_core(param);
	return res;
}

scalar sasfit_ff_spheroid_nu(scalar q, sasfit_param * param)
{
    int ierr,ndim;
	scalar res,err;
    scalar cubxmin[2], cubxmax[2];
	cubature_param cparam;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	NU = 1;
	LNDISTR=1.0;

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((NUNU < 0.0), param, "nu(%lg) < 0",NUNU);
	SASFIT_CHECK_COND1((R_EQUATORIAL < 0.0), param, "R_EQUATORIAL(%lg) < 0",R_EQUATORIAL);

	Q = q;		// Q
	P = 2.0;	// pow
	SIGMA = 0;
	T = 0;
	if (SIGMA==0) {
		ndim=1;
		NUMIN = 1;
		NUMAX = 1;
	} else {
		ndim  =2;
		find_LogNorm_int_range(6,1,SIGMA,&NUMIN, &NUMAX, param);
	}
	cubxmin[0]=0;
	cubxmax[0]=1;
	cubxmin[1]=NUMIN;
	cubxmax[1]=NUMAX;
	cparam.param = param;
	cparam.cubxmin=cubxmin;
	cparam.cubxmax=cubxmax;
	cparam.ndim=ndim;
	cparam.func = &sasfit_ff_spheroid_nu_core;

    ierr = sasfit_cubature(ndim,cubxmin,cubxmax,&ellip_shell_cubature,&cparam,sasfit_eps_get_nriq(), &res, &err);
    return res;
}

scalar sasfit_ff_spheroid_nu_f(scalar q, sasfit_param * param)
{
    int ierr,ndim;
	scalar res,err;
    scalar cubxmin[2], cubxmax[2];
	cubature_param cparam;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	NU = 1;
	LNDISTR=1.0;

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((NUNU < 0.0), param, "nu(%lg) < 0",NUNU);
	SASFIT_CHECK_COND1((R_EQUATORIAL < 0.0), param, "R_EQUATORIAL(%lg) < 0",R_EQUATORIAL);

	Q = q;		// Q
	P = 1.0;	// pow
	SIGMA = 0;
	T = 0.0;
	if (SIGMA==0) {
		ndim=1;
		NUMIN = 1;
		NUMAX = 1;
	} else {
		ndim  =2;
		find_LogNorm_int_range(6,1,SIGMA,&NUMIN, &NUMAX, param);
	}
	cubxmin[0]=0;
	cubxmax[0]=1;
	cubxmin[1]=NUMIN;
	cubxmax[1]=NUMAX;
	cparam.param = param;
	cparam.cubxmin=cubxmin;
	cparam.cubxmax=cubxmax;
	cparam.ndim=ndim;
	cparam.func = &sasfit_ff_spheroid_nu_core;

    ierr = sasfit_cubature(ndim,cubxmin,cubxmax,&ellip_shell_cubature,&cparam,sasfit_eps_get_nriq(), &res, &err);
    return res;
}

scalar sasfit_ff_spheroid_nu_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 4./.3*M_PI*gsl_pow_3(R_EQUATORIAL)*NUNU;
}

