/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define RG	param->p[0]
#define WI	param->p[1]
#define NU  param->p[2]
#define I0  param->p[3]
#define THETA0_DEG	param->p[4]
#define PSI_DEG	param->p[5]
#define DELTA_THETA0DEG param->p[6]

#define Q   param->p[MAXPAR-1]
#define PSI	param->p[MAXPAR-2]
#define DTHETA0 param->p[MAXPAR-3]
#define XX      param->p[MAXPAR-4]

scalar fexpQRn2(scalar x,void *pam){
	sasfit_param * param;
	param = (sasfit_param *) pam;
	scalar mu;
	mu = pow(fabs(x),2*NU);
	return gsl_pow_2(Q*RG)*(2*NU+1)*(2*NU+2)/6.*(mu+gsl_pow_4(M_PI)/180.*gsl_pow_2(mu*WI*cos(PSI))*(1+mu-mu*mu/4.-2*gsl_pow_3(mu)+gsl_pow_4(mu)));
}

scalar gsl_fexpQRn2(scalar x,sasfit_param * param){
	return 2*(1-x)*exp(-fexpQRn2(x,param));
}

int shearflow_kernel_cub(unsigned ndim, const double *x, void *pam,
      unsigned fdim, double *fval) {
	sasfit_param * param;
	param = (sasfit_param *) pam;
	if ((ndim < 1) || (fdim < 1)) {
		sasfit_out("false dimensions fdim:%d ndim:%d\n",fdim,ndim);
		return 1;
	}
	if (ndim==2) {
        PSI = sasfit_param_override_get_psi(PSI_DEG*M_PI/180.) - THETA0_DEG*M_PI/180. - x[1];
        fval[0]=gsl_fexpQRn2(x[0],param)/(2*DTHETA0);
	} else {
	    PSI = sasfit_param_override_get_psi(PSI_DEG*M_PI/180.) - THETA0_DEG*M_PI/180.;
	    fval[0]=gsl_fexpQRn2(x[0],param);
	}

	return 0;
}

scalar shearflow_kernel_sasfit_cubature(const double *x, size_t ndim, void *pam) {
    scalar fval;
    int ierr;
    ierr = shearflow_kernel_cub(ndim, x, pam,1,&fval);
    return fval;
}

scalar sasfit_ff_polymer_under_shearflow(scalar q, sasfit_param * param)
{	scalar *aw, res,err,sum;
    scalar cubxmin[2], cubxmax[2], fval[1], ferr[1];
    int intstrategy, lenaw=4000, ierr;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((RG < 0.0), param, "Rg(%lg) < 0",RG); // modify condition to your needs
	SASFIT_CHECK_COND1((WI < 0.0), param, "Wi(%lg) < 0",WI); // modify condition to your needs

	// insert your code here

	PSI = sasfit_param_override_get_psi(PSI_DEG*M_PI/180.) - THETA0_DEG*M_PI/180.;
	Q = q;
    DTHETA0 = DELTA_THETA0DEG*M_PI/360.;
	cubxmin[0]=0;
	cubxmax[0]=1;
	cubxmin[1]= -DTHETA0/2;
	cubxmax[1]=  DTHETA0/2;
    if (DTHETA0 == 0) {
        ierr  = sasfit_cubature(1,cubxmin,cubxmax,shearflow_kernel_sasfit_cubature,param,sasfit_eps_get_nriq(), &sum,&err);
        sum = sum/DTHETA0;
    } else {
        ierr  = sasfit_cubature(2,cubxmin,cubxmax,shearflow_kernel_sasfit_cubature,param,sasfit_eps_get_nriq(), &sum,&err);
    }
	return I0*sum;
}

scalar sasfit_ff_polymer_under_shearflow_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_polymer_under_shearflow_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

