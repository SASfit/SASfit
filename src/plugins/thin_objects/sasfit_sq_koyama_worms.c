/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>
#include <float.h>

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
	return sqrt((2.*rn2/gsl_pow_2(LB))*sqrt(10.*fabs(1.0-3./5.*K)));
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
	scalar s,t,res;
	sasfit_param * param;
	koyama_param *cparam;
	cparam = (koyama_param *) pam;
	param = cparam->param;
	s=cparam->s;

	t=x*LB/2.;

	cparam->rn2=gsl_pow_2(LB)/2.*(x-(1-exp(-x)));
	cparam->rn4=gsl_pow_4(LB)/4.*(5/3.*x*x - 52.0/9.*x-2./27.*(1.0-exp(-3.0*x))
	           +8*(1.0-exp(-x))-2*x*exp(-x));

    if (fabs(x)< 1e-4) {
        cparam->rn2=gsl_pow_2(LB)/2.*(0.5*gsl_pow_2(x)-gsl_pow_3(x)/6.0+gsl_pow_4(x)/24.-gsl_pow_5(x)/120.+gsl_pow_6(x)/720.);
        cparam->rn4=gsl_pow_4(LB)/4.*(1./4.0*gsl_pow_4(x)-1./6.0*gsl_pow_5(x)+29./360.0*gsl_pow_6(x)-1./30.0*gsl_pow_7(x)+247./20160.0*gsl_pow_8(x));
    } else {
        cparam->rn2=gsl_pow_2(LB)/2.*(x-(1-exp(-x)));
        cparam->rn4=gsl_pow_4(LB)/4.*(5/3.*x*x - 52.0/9.*x-2./27.*(1.0-exp(-3.0*x))
	           +8*(1.0-exp(-x))-2*x*exp(-x));
    }
    if (fabs(x)<1e-12) {
        cparam->K=1;
    } else {
        cparam->K=cparam->rn4/gsl_pow_2(cparam->rn2);
    }
	res = exp(-s*s/3.0*xf(x,pam)) * gsl_sf_bessel_j0(fabs(s*xg(x,pam)));
//	sasfit_out("::phi: x: %lg, rn2=%lg, rn4=%lg, K=%lg phi:%lg xf:%lg, xg:%lg\n",x, cparam->rn2,cparam->rn4,cparam->K,res,xf(x,pam),xg(x,pam));
	return res;
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
		return 0;
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
	cubxmin[0]=DBL_EPSILON*cparam.X;
	cubxmax[0]=(1-DBL_EPSILON)*cparam.X;
	cubxmin[0]=0;
	cubxmax[0]=cparam.X;

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

