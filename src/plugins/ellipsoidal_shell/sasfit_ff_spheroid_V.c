/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define V	param->p[0]
#define R_EQUATORIAL		param->p[1]
#define T	param->p[3]

scalar sasfit_ff_spheroid_V_core(sasfit_param * param) {
	A = R_EQUATORIAL;
	B = V/(A*A)*3./(4.0*M_PI);
	TNU = 0;
	return LNDISTR*gsl_sf_pow_int(F_EllSh(param),lround(P));
}

scalar sasfit_ff_spheroid_V_core_NU(scalar x, sasfit_param * param)
{
	sasfit_param subParam;
	sasfit_init_param( &subParam );
	if (SIGMA==0) {
		LNDISTR = 1;
		NU = 1;
	} else {
		subParam.p[0] = 1.0;
		subParam.p[1] = SIGMA;
		subParam.p[2] = 1.0;
		subParam.p[3] = 1.0;
		LNDISTR = sasfit_sd_LogNorm(x, &subParam);
		NU=x;
		SASFIT_CHECK_SUB_ERR(param, subParam);
		if ( subParam.errStatus != FALSE ) {
			sasfit_out("LogNormError: SIGMA:%lf\n",SIGMA);
			return 1;
		}
	}
	return sasfit_ff_spheroid_V_core(param);
}


scalar sasfit_ff_spheroid_V_core_MU(scalar x, sasfit_param * param)
{
	scalar res;

	SASFIT_ASSERT_PTR(param);

	MU = x;

	res = sasfit_integrate(NUMIN, NUMAX, sasfit_ff_spheroid_V_core_NU, param);

	return res;
}

scalar sasfit_ff_spheroid_V(scalar q, sasfit_param * param)
{
	scalar res,err,sum;
    scalar cubxmin[2], cubxmax[2];
    int ndim,ierr;
	cubature_param cparam;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	NU = 1;
	LNDISTR=1.0;

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R_EQUATORIAL < 0.0), param, "R_EQUATORIAL(%lg) < 0",R_EQUATORIAL);
	SASFIT_CHECK_COND1((V < 0.0), param, "V(%lg) < 0",V);

	Q = q;		// Q
	P = 2.0;	// pow
	T = 0;
	X_CORE = 0;
	X_SHELL = 0;
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
	cparam.func = &sasfit_ff_spheroid_V_core;
    ierr = sasfit_cubature(ndim,cubxmin,cubxmax,&ellip_shell_mint,&cparam,sasfit_eps_get_aniso(), &sum,&err);
	return sum;
}

scalar sasfit_ff_spheroid_V_f(scalar q, sasfit_param * param)
{
	scalar res,err,sum;
    scalar cubxmin[2], cubxmax[2];
    int ndim, ierr;
	cubature_param cparam;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	NU = 1;
	LNDISTR=1.0;

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R_EQUATORIAL < 0.0), param, "R_EQUATORIAL(%lg) < 0",R_EQUATORIAL);
	SASFIT_CHECK_COND1((V < 0.0), param, "V(%lg) < 0",V);

	Q = q;		// Q
	P = 1.0;	// pow
	T = 0.0;
	X_CORE = 0;
	X_SHELL = 0;
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
	cparam.func = &sasfit_ff_spheroid_V_core;
    ierr = sasfit_cubature(ndim,cubxmin,cubxmax,&ellip_shell_mint,&cparam,sasfit_eps_get_aniso(), &sum,&err);
	return sum;
}

scalar sasfit_ff_spheroid_V_v(scalar x, sasfit_param * param, int dist)
{   scalar Rprinc;
    scalar Vol;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	switch (dist) {
    case 0:
        Vol = x;
        break;
    default:
        Vol=V;
        break;
	}
	return Vol;
}

