/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */


#include "include/private.h"
#include <sasfit_error_ff.h>

scalar sinc(scalar x) {
	if (fabs(x)<1e-6) {
		return 1-gsl_pow_2(x)/6.0+gsl_pow_4(x)/120.-gsl_pow_6(x)/5040.+gsl_pow_8(x)/362880.;
	} else {
		return sin(x)/x;
	}
}

scalar KernelSphAvg_P(scalar theta, scalar phi , sasfit_param * param) {

	return 2./M_PI * gsl_pow_2(sinc(Q*NUA/2.0*sin(theta)*cos(phi)) *
				               sinc(Q*NUB/2.0*sin(theta)*sin(phi)) *
							   sinc(Q*NUC/2.0*cos(theta))*NUA*NUB*NUC*ETA) ;
}

scalar Kernel_P(const double *gam , void * pam) {
	return KernelSphAvg_P(gam[1],gam[0],(sasfit_param *) pam) * sin(gam[1]);
}

int K_cubature(unsigned ndim, const double *x, void *pam,
      unsigned fdim, double *fval) {
	scalar LNdistr;
	sasfit_param subParam;
	sasfit_param *param;
	param = (sasfit_param *) pam;
	sasfit_init_param( &subParam );

	fval[0] = 0;
	if ((ndim < 3) || (fdim < 1)) {
		sasfit_out("false dimensions fdim:%d ndim:%d\n",fdim,ndim);
		return 1;
	}
	subParam.p[0] = 1.0;
	subParam.p[1] = SIGMA;
	subParam.p[2] = 1.0;
	subParam.p[3] = 1.0;
	LNdistr = sasfit_sd_LogNorm(x[2], &subParam);

	if (NUDIM ==3) {
		NUA = x[2]*A;
		NUB = x[2]*B;
		NUC = x[2]*C;
	} else if (NUDIM ==2) {
		NUA = x[2]*A;
		NUB = x[2]*B;
		NUC = C;
	} else if (NUDIM ==1) {
		NUA = x[2]*A;
		NUB = B;
		NUC = C;
	}
	SASFIT_CHECK_SUB_ERR(param, subParam);
	if ( subParam.errStatus != FALSE ) return 1;

    fval[0] = LNdistr*Kernel_P(x,pam);
    return 0;
}

scalar K_SphAvg_P(const double *x, size_t ndim, void *pam) {
    scalar fval[1];
    int ierr;
    ierr = K_cubature(ndim, x, pam, 1 , &fval[0]);
    return fval[0];
}
