/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

typedef struct
{
	sasfit_param *param;
	scalar *cubxmin;
	scalar *cubxmax;
	scalar s;
	scalar rn2;
	scalar rn4;
	scalar K;
	scalar X;
} koyama_param;

// define shortcuts for local parameters/variables


scalar xg(scalar x, void *pam) {
	scalar rn2,rn4,K;
	sasfit_param * param;
	koyama_param *cparam;
	cparam = (koyama_param *) pam;
	param = cparam->param;
	rn2=cparam->rn2;
	rn4=cparam->rn4;
	K=cparam->K;
	return sqrt((2.*rn2/gsl_pow_2(LB))*sqrt(10.*fabs(1.-3./5.*K)));
}

scalar xf(scalar x, void *pam) {
	scalar rn2,rn4,K;
	sasfit_param * param;
	koyama_param *cparam;
	cparam = (koyama_param *) pam;
	param = cparam->param;
	rn2=cparam->rn2;
	rn4=cparam->rn4;
	K=cparam->K;
	return (2*rn2/gsl_pow_2(LB)) - gsl_pow_2(xg(x,pam))/2.0;
}

scalar phi(scalar x, void *pam) {
	scalar s,t;
	sasfit_param * param;
	koyama_param *cparam;
	cparam = (koyama_param *) pam;
	param = cparam->param;
	s=cparam->s;

	t=x*LB/2.;

	cparam->rn2=gsl_pow_2(LB)/2.*(x-(1-exp(-x)));
	cparam->rn4=gsl_pow_4(LB)/4.*(5/3.*x*x - 52.0/9.*x-2./27.*(1.0-exp(-3.0*x))
	           +8*(1.0-exp(-x))-2*x*exp(-x));


	if (fabs(cparam->rn2) <= sasfit_eps_get_nriq()) {
		cparam->K=1;
	} else {
		cparam->K=cparam->rn4/gsl_pow_2(cparam->rn2);
	}
//	sasfit_out("::phi: x: %lf, rn2=%lf, rn4=%lf, K=%lf\n",x, cparam->rn2,cparam->rn4,cparam->K);
	return exp(-s*s/3.*xf(x,pam)) * gsl_sf_bessel_j0(fabs(s*xg(x,pam)));
}

scalar koyama_worm_kernel(scalar x, void *pam) {
	koyama_param *cparam;
	cparam = (koyama_param *) pam;
	return 2*phi(x,pam)*(cparam->X - x)/gsl_pow_2(cparam->X);
}

scalar koyama_worm_kernel_cub(const double *x, size_t ndim, void *pam) {
	koyama_param *cparam;
	cparam = (koyama_param *) pam;
	if (ndim < 1) {
		sasfit_err("false dimensions ndim:%d\n",ndim);
		return -1;
	}
	return koyama_worm_kernel(x[0],pam);
}

scalar sasfit_sq_koyama_worms(scalar q, sasfit_param * param)
{
    int ierr;
    scalar res,err, X;
    scalar cubxmin[1], cubxmax[1];
	koyama_param cparam;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((L <= 0.0), param, "L(%lg) < 0",L); // modify condition to your needs
	SASFIT_CHECK_COND1((LB <= 0.0), param, "lb(%lg) < 0",LB); // modify condition to your needs

	// insert your code here

	Q=q;
	cparam.param = param;
	cparam.s=(q*LB/2.);
	cparam.X = 2*L/LB;
	cubxmin[0]=0;
	cubxmin[0]=cparam.X;

	ierr = sasfit_cubature(1,cubxmin, cubxmax,&koyama_worm_kernel_cub,&cparam , sasfit_eps_get_nriq(),&res,&err);

	return res*L*L;
}

scalar sasfit_sq_koyama_worms_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_koyama_worms_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

