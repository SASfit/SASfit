/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define I0	param->p[0]
#define L	param->p[1]
#define LB	param->p[2]

#define F	param->p[4]
#define Q   param->p[MAXPAR-1]

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


scalar xg(scalar x, sasfit_param * param) {
	scalar rn2,rn4,K;
	koyama_param *cparam;
	cparam = param->moreparam;
	rn2=cparam->rn2;
	rn4=cparam->rn4;
	K=cparam->K;
	return sqrt((2.*rn2/gsl_pow_2(LB))*sqrt(10.*fabs(1.-3./5.*K)));
}

scalar xf(scalar x, sasfit_param * param) {
	scalar rn2,rn4,K;
	koyama_param *cparam;
	cparam = param->moreparam;
	rn2=cparam->rn2;
	rn4=cparam->rn4;
	K=cparam->K;
	return (2*rn2/gsl_pow_2(LB)) - gsl_pow_2(xg(x,param))/2.0;
}

scalar phi(scalar x, sasfit_param *param) {
	scalar s,t;
	koyama_param *cparam;
	cparam = param->moreparam;
	s=cparam->s;

	t=x*LB/2.;

	cparam->rn2=gsl_pow_2(LB)/2.*(x-(1-exp(-x)));
	cparam->rn4=gsl_pow_4(LB)/4.*(5/3.*x*x - 52.0/9.*x-2./27.*(1.0-exp(-3.0*x))
	           +8*(1.0-exp(-x))-2*x*exp(-x));


	if (fabs(cparam->rn2) <= sasfit_eps_get_nriq()) {
		cparam->K=5./3.;
	} else {
		cparam->K=cparam->rn4/gsl_pow_2(cparam->rn2);
	}
//	sasfit_out("::phi: x: %lf, rn2=%lf, rn4=%lf, K=%lf\n",x, cparam->rn2,cparam->rn4,cparam->K);
	return exp(-s*s/3.*xf(x,param)) * gsl_sf_bessel_j0(fabs(s*xg(x,param)));
}

scalar koyama_worm_Pkernel(scalar x, sasfit_param *param) {
	koyama_param *cparam;
	cparam = param->moreparam;
	return 2*phi(x,param)*(cparam->X - x)/gsl_pow_2(cparam->X);
}

scalar koyama_worm_Fkernel(scalar x, sasfit_param *param) {
	koyama_param *cparam;
	cparam = param->moreparam;
	return 1*phi(x,param)/cparam->X;
}

scalar sasfit_ff_broken_worms_star(scalar q, sasfit_param * param)
{	scalar PNW, FNW,X;
	koyama_param cparam;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((L <= 0.0), param, "L(%lg) < 0",L); // modify condition to your needs
	SASFIT_CHECK_COND1((LB <= 0.0), param, "lb(%lg) < 0",LB); // modify condition to your needs
	SASFIT_CHECK_COND1((F < 1.0), param, "f(%lg) < 1",F); // modify condition to your needs

	// insert your code here

	Q=q;
	cparam.param = param;
	cparam.s=(q*LB/2.);
	cparam.X = 2*L/LB;
    param->moreparam=&cparam;
    PNW=sasfit_integrate(0.0,cparam.X, &koyama_worm_Pkernel, param);
    FNW=sasfit_integrate(0.0,cparam.X, &koyama_worm_Fkernel, param);
	return (PNW+(F-1)*FNW*FNW)*I0/F;
}

scalar sasfit_ff_broken_worms_star_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_broken_worms_star_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

