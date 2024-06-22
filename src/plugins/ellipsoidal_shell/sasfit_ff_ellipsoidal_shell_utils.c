/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

scalar f_sph(scalar x) {

    if (fabs(x) < 1e-6)	{
		return 1-gsl_pow_2(x)/10.+gsl_pow_4(x)/280.0-gsl_pow_6(x)/15120.+gsl_pow_8(x)/1330560.;
	} else {
		return 3*gsl_sf_bessel_j1(x)/x;
	}
}

scalar F_EllSh(sasfit_param * param) {
	scalar xc, xt, Vc, Vt;
	scalar eta_core, eta_shell;
	xc = Q*sqrt(gsl_pow_2(A*MU)+ gsl_pow_2(B)*(1-MU*MU));
	xt = Q*sqrt(gsl_pow_2((A+TNU)*MU)+ gsl_pow_2(B+TNU)*(1-MU*MU));
	Vc = 4./3.*M_PI*A*gsl_pow_2(B);
	Vt = 4./3.*M_PI*(A+TNU)*gsl_pow_2(B+TNU);
	eta_core  = ETA_CORE *(1-X_CORE ) + ETA_SOLV*X_CORE;
	eta_shell = ETA_SHELL*(1-X_SHELL) + ETA_SOLV*X_SHELL;
	return 	(eta_core-eta_shell)*Vc*f_sph(xc)+
			(eta_shell-ETA_SOLV)*Vt*f_sph(xt);
}

scalar ellip_shell_cubature(const double *x, size_t ndim,  void *pam) {
	sasfit_param subParam;
	sasfit_init_param( &subParam );
	sasfit_param *param;
	cubature_param *cparam;
	cparam = (cubature_param *) pam;
	param = cparam->param;

	if (ndim < 1) {
		sasfit_out("false dimensions ndim:%d\n",ndim);
		return 1;
	}
	if ((ndim < 2) || (SIGMA==0)) {
		LNDISTR = 1;
		NU = 1;
	} else {
		subParam.p[0] = 1.0;
		subParam.p[1] = SIGMA;
		subParam.p[2] = 1.0;
		subParam.p[3] = 1.0;
		NU=x[1];
		LNDISTR = sasfit_sd_LogNorm(NU, &subParam);
		SASFIT_CHECK_SUB_ERR(param, subParam);
		if ( subParam.errStatus != FALSE ) {
			sasfit_out("LogNormError: SIGMA:%lf\n",SIGMA);
			return -9999;
		}
	}
	MU = x[0];
    return (cparam->func)(param);
}

scalar ellip_shell_mint(const double *x, size_t ndim, void *pam) {
	sasfit_param subParam;
	sasfit_init_param( &subParam );
	sasfit_param *param;
	cubature_param *cparam;
	cparam = (cubature_param *) pam;
	param = cparam->param;

	if ((ndim < 1)) {
		sasfit_err("false dimensions ndim:%d\n",ndim);
		return 1;
	}
	if ((ndim < 2) || (SIGMA==0)) {
		LNDISTR = 1;
		NU = 1;
	} else {
		subParam.p[0] = 1.0;
		subParam.p[1] = SIGMA;
		subParam.p[2] = 1.0;
		subParam.p[3] = 1.0;
		NU=x[1];
		LNDISTR = sasfit_sd_LogNorm(NU, &subParam);
		SASFIT_CHECK_SUB_ERR(param, subParam);
		if ( subParam.errStatus != FALSE ) {
			sasfit_err("LogNormError: SIGMA:%lf\n",SIGMA);
			return 1;
		}
	}
	MU = x[0];
    return (cparam->func)(param);
}
