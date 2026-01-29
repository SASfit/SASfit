/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define RG	param->p[0]
#define WI	param->p[1]
#define NU	param->p[2]
#define I0	param->p[3]
#define THETA0_DEG	param->p[4]
#define PSI_DEG	param->p[5]
#define DELTA_THETA0DEG param->p[6]
#define Q   param->p[MAXPAR-1]
#define PSI	param->p[MAXPAR-2]
#define DTHETA0 param->p[MAXPAR-3]
#define XX      param->p[MAXPAR-4]
#define EPS (2*NU-1.)

scalar rexpQRn2(scalar x,void *pam){
	sasfit_param * param;
	param = (sasfit_param *) pam;
	scalar mu;
	mu = pow(fabs(x*(1-x)),2*NU);
	return (mu+gsl_pow_4(M_PI)/(1440.)*gsl_pow_2(WI*cos(PSI))*(mu*mu+2*gsl_pow_3(mu)));
//  mu = fabs(x);
//	return (mu*(1-mu)+gsl_pow_4(M_PI)/(1440.+0*5760.)*gsl_pow_2(mu*WI*cos(PSI))*(1-5*mu*mu+6*gsl_pow_3(mu)-2*gsl_pow_4(mu)));
}

scalar gsl_rexpQRn2(scalar x,sasfit_param * param){
    scalar KEPS;
    KEPS = gsl_sf_gamma(2+EPS)*gsl_sf_gamma(3+EPS)/gsl_sf_gamma(5+2*EPS);
	return 2*(1-x)*exp(-2*gsl_pow_2(Q*RG)/(12*KEPS)*rexpQRn2(x,param));
}

int rshearflow_kernel_cub(unsigned ndim, const double *x, void *pam,
      unsigned fdim, double *fval) {
	sasfit_param * param;
	param = (sasfit_param *) pam;
	if ((ndim < 1) || (fdim < 1)) {
		sasfit_out("false dimensions fdim:%d ndim:%d\n",fdim,ndim);
		return 1;
	}
	if (ndim==2) {
        PSI = sasfit_param_override_get_psi(PSI_DEG*M_PI/180.) - THETA0_DEG*M_PI/180. - x[1];
        fval[0]=gsl_rexpQRn2(x[0],param)/(DTHETA0);
	} else {
	    PSI = sasfit_param_override_get_psi(PSI_DEG*M_PI/180.) - THETA0_DEG*M_PI/180.;
	    fval[0]=gsl_rexpQRn2(x[0],param);
	}
	return 0;
}

scalar rshearflow_kernel_sasfit_cubature(const double *x, size_t ndim, void *pam) {
    scalar fval;
    int ierr;
    ierr = rshearflow_kernel_cub(ndim, x, pam,1,&fval);
    return fval;
}

scalar sasfit_ff_ring_under_shearflow(scalar q, sasfit_param * param)
{	scalar *aw, res,err,sum, ierr;
    scalar cubxmin[2], cubxmax[2], fval[1], ferr[1];
    int intstrategy, lenaw=4000;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((RG < 0.0), param, "Rg(%lg) < 0",RG); // modify condition to your needs
	SASFIT_CHECK_COND1((WI < 0.0), param, "Wi(%lg) < 0",WI); // modify condition to your needs
	SASFIT_CHECK_COND1((NU <= 0.0), param, "nu(%lg) <= 0",NU); // modify condition to your needs

	// insert your code here

	PSI = sasfit_param_override_get_psi(PSI_DEG*M_PI/180.) - THETA0_DEG*M_PI/180.;
    DTHETA0 = DELTA_THETA0DEG*M_PI/360.;
	cubxmin[0]=0;
	cubxmax[0]=1;
	cubxmin[1]= -DTHETA0/2;
	cubxmax[1]=  DTHETA0/2;
	Q = q;
    if (DTHETA0 == 0) {
        ierr  = sasfit_cubature(1,cubxmin,cubxmax,rshearflow_kernel_sasfit_cubature,param,sasfit_eps_get_nriq(), &sum,&err);
    } else {
        ierr  = sasfit_cubature(2,cubxmin,cubxmax,rshearflow_kernel_sasfit_cubature,param,sasfit_eps_get_nriq(), &sum,&err);
    }
    return I0*sum;
}

scalar sasfit_ff_ring_under_shearflow_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_ring_under_shearflow_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

