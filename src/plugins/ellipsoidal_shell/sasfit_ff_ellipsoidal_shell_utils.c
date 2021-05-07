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
	xc = Q*sqrt(gsl_pow_2(A*MU)+ gsl_pow_2(B)*(1-MU*MU));
	xt = Q*sqrt(gsl_pow_2((A+TNU)*MU)+ gsl_pow_2(B+TNU)*(1-MU*MU));
	Vc = 4./3.*M_PI*A*gsl_pow_2(B);
	Vt = 4./3.*M_PI*(A+TNU)*gsl_pow_2(B+TNU);
	return 	(ETA_CORE-ETA_SHELL)*Vc*f_sph(xc)+
			(ETA_SHELL-ETA_SOLV)*Vt*f_sph(xt);
}

int ellip_shell_cubature(unsigned ndim, const double *x, void *pam,
      unsigned fdim, double *fval) {
	sasfit_param subParam;
	sasfit_init_param( &subParam );
	sasfit_param *param;
	cubature_param *cparam;
	cparam = (cubature_param *) pam;
	param = cparam->param;

	fval[0] = 0;
	if ((ndim < 1) || (fdim < 1)) {
		sasfit_out("false dimensions fdim:%d ndim:%d\n",fdim,ndim);
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
			return 1;
		}
	}
	MU = x[0];
    fval[0] = (cparam->func)(param);
    return 0;
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

scalar Kernel_P_OOURA2(scalar x, void * pam) {
	sasfit_param subParam;
	sasfit_init_param( &subParam );
	sasfit_param *param;
	cubature_param *cparam;
	cparam = (cubature_param *) pam;
	param = cparam->param;

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
	return  (cparam->func)(param);
}

scalar Kernel_P_OOURA1(scalar x, void * pam) {
	sasfit_param *param;
	cubature_param *cparam;
	cparam = (cubature_param *) pam;
	param = cparam->param;
	scalar sum, err, *aw;
	int lenaw;

	lenaw=10000;
	MU = x;
	if (cparam->ndim == 1) {
		NU=1;
		LNDISTR = 1;
		return (cparam->func)(param);
	}
	switch(sasfit_get_int_strategy()) {
    case OOURA_DOUBLE_EXP_QUADRATURE: {
			aw = (scalar *)malloc((lenaw)*sizeof(scalar));
			sasfit_intdeini(lenaw,GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
			sasfit_intde(&Kernel_P_OOURA2, cparam->cubxmin[1], cparam->cubxmax[1], aw, &sum, &err,pam);
			free(aw);
            break;
            }
    case OOURA_CLENSHAW_CURTIS_QUADRATURE: {
			aw = (scalar *)malloc((lenaw+1)*sizeof(scalar));
			sasfit_intccini(lenaw, aw);
			sasfit_intcc(&Kernel_P_OOURA2, cparam->cubxmin[1], cparam->cubxmax[1], sasfit_eps_get_nriq(), lenaw, aw, &sum, &err,pam);
			free(aw);
            break;
            }
	}
	return sum;
}
