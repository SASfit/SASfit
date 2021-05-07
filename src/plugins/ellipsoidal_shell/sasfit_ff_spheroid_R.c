/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R_PRINCIPLE		param->p[0]
#define R_EQUATORIAL	param->p[1]
#define T	param->p[3]

scalar sasfit_ff_spheroid_R_core(sasfit_param * param) {
	A = R_PRINCIPLE;
	B = R_EQUATORIAL;
	TNU = 0;
	return LNDISTR*gsl_sf_pow_int(F_EllSh(param),lround(P));
}

scalar sasfit_ff_spheroid_R_core_MU(scalar x, sasfit_param * param)
{
	scalar res;

	SASFIT_ASSERT_PTR(param);

	MU = x;
	LNDISTR = 1;
	NU = 1;
	res = sasfit_ff_spheroid_R_core(param);
	return res;
}

scalar sasfit_ff_spheroid_R(scalar q, sasfit_param * param)
{
	scalar *aw, res,err,sum;
    scalar cubxmin[2], cubxmax[2], fval[1], ferr[1];
    size_t neval;
    int intstrategy, ndim, lenaw=4000, ierr;
	cubature_param cparam;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	NU = 1;
	LNDISTR=1.0;

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R_PRINCIPLE < 0.0), param, "R_PRINCIPLE(%lg) < 0",R_PRINCIPLE);
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
	cparam.func = &sasfit_ff_spheroid_R_core;
    ierr = sasfit_cubature(ndim,cubxmin,cubxmax,&ellip_shell_mint,&cparam,sasfit_eps_get_aniso(), &sum,&err);
	return sum;
}

scalar sasfit_ff_spheroid_R_f(scalar q, sasfit_param * param)
{
	scalar *aw, res,err,sum;
    scalar cubxmin[2], cubxmax[2], fval[1], ferr[1];
    size_t neval;
    int intstrategy, ndim, lenaw=4000, ierr;
	cubature_param cparam;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	NU = 1;
	LNDISTR=1.0;

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R_PRINCIPLE < 0.0), param, "R_PRINCIPLE(%lg) < 0",R_PRINCIPLE);
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
	cparam.func = &sasfit_ff_spheroid_R_core;

	ierr = sasfit_cubature(ndim,cubxmin,cubxmax,&ellip_shell_mint,&cparam,sasfit_eps_get_aniso(), &sum,&err);
	return sum;
}

scalar sasfit_ff_spheroid_R_v(scalar x, sasfit_param * param, int dist)
{
    scalar Vol;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	switch (dist) {
    case 0:
        Vol = 4./.3*M_PI*gsl_pow_2(x)*(R_PRINCIPLE);
        break;
    case 1:
        Vol = 4./.3*M_PI*gsl_pow_2(R_EQUATORIAL)*(x);
        break;
    default: Vol=4./.3*M_PI*gsl_pow_2(R_EQUATORIAL)*(R_PRINCIPLE);
	}
	return Vol;
}

