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

int koyama_worm_kernel_cub(unsigned ndim, const double *x, void *pam,
      unsigned fdim, double *fval) {
	koyama_param *cparam;
	cparam = (koyama_param *) pam;
	if ((ndim < 1) || (fdim < 1)) {
		sasfit_out("false dimensions fdim:%d ndim:%d\n",fdim,ndim);
		return 1;
	}
	fval[0]=koyama_worm_kernel(x[0],pam);
//	sasfit_out("::koyama_worm_kernel_cub: x[0]:%lf fval[0]:%lf\n",x[0],fval[0]);
	return 0;
}

scalar sasfit_sq_koyama_worms(scalar q, sasfit_param * param)
{	scalar *aw, res,err,sum, X;
    scalar cubxmin[1], cubxmax[1], fval[1], ferr[1];
	koyama_param cparam;
    int intstrategy, lenaw=4000;

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
//	sasfit_out("::sasfit_ff_koyama_worm: q: %lf X:%lf s:%lf\n",q, cparam.X, cparam.s);
	intstrategy = sasfit_get_int_strategy();
//	intstrategy=OOURA_CLENSHAW_CURTIS_QUADRATURE;
	switch(intstrategy) {
    case OOURA_DOUBLE_EXP_QUADRATURE: {
            aw = (scalar *)malloc((lenaw)*sizeof(scalar));
            sasfit_intdeini(lenaw, GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
            sasfit_intde(&koyama_worm_kernel,0,cparam.X, aw, &res, &err, &cparam);
			sum=res;
            free(aw);
            break;
            }
    case OOURA_CLENSHAW_CURTIS_QUADRATURE: {
            aw = (scalar *)malloc((lenaw+1)*sizeof(scalar));
            sasfit_intccini(lenaw, aw);
            sasfit_intcc(&koyama_worm_kernel, 0,cparam.X, sasfit_eps_get_nriq(), lenaw, aw, &res, &err,&cparam);
			sum=res;
            free(aw);
            break;
            }
    case H_CUBATURE: {
			hcubature(1, &koyama_worm_kernel_cub,&cparam,1, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED,
				fval, ferr);
			sum = fval[0];
            break;
            }
    case P_CUBATURE: {
			pcubature(1, &koyama_worm_kernel_cub,&cparam,1, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED,
				fval, ferr);
			sum = fval[0];
            break;
            }
    default: {
//		    sasfit_out("This is default sasfit_integrate routine\n");
//            sum=sasfit_integrate(0.0,cparam.X, &koyama_worm_kernel, &cparam);
sum=999;
            break;
            }
    }
	sasfit_out("::sasfit_ff_koyama_worm: q:%lf sum: %lf\n",q, sum);
	return sum*L*L;
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

