/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define RG	param->p[0]
#define FP	param->p[1]
#define ALPHA	param->p[2]
#define BETA	param->p[3]
#define GAMMA	param->p[4]
#define DELTA	param->p[5]
#define I0  param->p[6]
#define THETA0_DEG	param->p[7]
#define THETA0	(param->p[7]*M_PI/180)
#define PSI_DEG	param->p[8]
#define DELTA_THETA0DEG  param->p[9]
#define DELTA_THETA0    (param->p[9]*M_PI/180.)
#define Q   param->p[MAXPAR-1]
#define PSI	param->p[MAXPAR-2]
#define BB  param->p[MAXPAR-3]
#define CC  param->p[MAXPAR-4]

scalar r2expQRn2(scalar x,void *pam){
	sasfit_param * param;
	param = (sasfit_param *) pam;
	scalar mu,cscale;
	mu = fabs(x);

    cscale = sqrt(BB/(CC + 2*BB*FP - 2*CC*FP));

	if (mu<=FP) {
        return BB*mu*cscale;
	} else if (mu>=1-FP) {
	    return (CC - 2*CC*FP + BB*(-1 + 2*FP + mu))*cscale;
    } else {
	    return (BB*FP + CC*(-FP + mu))*cscale;
	}

}

scalar gsl_r2expQRn2(scalar x,sasfit_param * param){
    scalar t,t1;

    t=r2expQRn2(x,param);
    t1=r2expQRn2(1,param);

	return 2*(1-x)*exp(-2*gsl_pow_2(Q*RG)*t*(t1-t));
}

int rshearflow2_kernel_cub(unsigned ndim, const double *x, void *pam,
      unsigned fdim, double *fval)
{
	sasfit_param * param;
	param = (sasfit_param *) pam;

	if ((ndim < 2) || (fdim < 1)) {
		sasfit_out("false dimensions fdim:%d ndim:%d\n",fdim,ndim);
		return 1;
	}

    BB= gsl_pow_2(ALPHA*cos(x[1]+THETA0-PSI))+gsl_pow_2(BETA *sin(x[1]+THETA0-PSI));
    CC= gsl_pow_2(GAMMA*cos(x[1]+THETA0-PSI))+gsl_pow_2(DELTA*sin(x[1]+THETA0-PSI));

	fval[0]=gsl_r2expQRn2(x[0],param);
	return 0;
}

scalar rshearflow2_kernel_OOURA(scalar x, void *pam) {
	sasfit_param * param;
	param = (sasfit_param *) pam;
	return gsl_r2expQRn2(x,param);
}

scalar sasfit_ff_ring_under_shearflow2(scalar q, sasfit_param * param)
{	scalar *aw, res,err,sum;
    scalar cubxmin[2], cubxmax[2], fval[1], ferr[1];
    int intstrategy, lenaw=4000;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((RG < 0.0), param, "Rg(%lg) < 0",RG); // modify condition to your needs
	// insert your code here

	PSI = sasfit_param_override_get_psi(PSI_DEG * M_PI/180.);
	Q = q;

	cubxmin[0]=0;
	cubxmax[0]=1;
	cubxmin[1]=-DELTA_THETA0/2.;
	cubxmax[1]= DELTA_THETA0/2.;
    BB= gsl_pow_2(ALPHA*cos(THETA0-PSI))+gsl_pow_2(BETA *sin(THETA0-PSI));
    CC= gsl_pow_2(GAMMA*cos(THETA0-PSI))+gsl_pow_2(DELTA*sin(THETA0-PSI));

	intstrategy = sasfit_get_int_strategy();
//    intstrategy=OOURA_CLENSHAW_CURTIS_QUADRATURE;
	switch(intstrategy) {
    case OOURA_DOUBLE_EXP_QUADRATURE: {
            aw = (scalar *)malloc((lenaw)*sizeof(scalar));
            sasfit_intdeini(lenaw, GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
            sasfit_intde(&rshearflow2_kernel_OOURA,0,1, aw, &res, &err, param);
			sum=res;
            free(aw);
            break;
            }
    case OOURA_CLENSHAW_CURTIS_QUADRATURE: {
            aw = (scalar *)malloc((lenaw+1)*sizeof(scalar));
            sasfit_intccini(lenaw, aw);
            sasfit_intcc(&rshearflow2_kernel_OOURA, 0,1, sasfit_eps_get_nriq(), lenaw, aw, &res, &err,param);
			sum=res;
            free(aw);
            break;
            }
    case H_CUBATURE: {
			hcubature(1, &rshearflow2_kernel_cub,param,2, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_aniso(), ERROR_PAIRED,
				fval, ferr);
			sum = fval[0];
			if (DELTA_THETA0 != 0) sum=sum/DELTA_THETA0;
            break;
            }
    case P_CUBATURE: {
			pcubature(1, &rshearflow2_kernel_cub,param,2, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_aniso(), ERROR_PAIRED,
				fval, ferr);
			sum = fval[0];
			if (DELTA_THETA0 != 0) sum=sum/DELTA_THETA0;
            break;
            }
    default: {
//		    sasfit_out("This is default sasfit_integrate routine\n");
			sum = sasfit_integrate(0,1,&gsl_r2expQRn2,param);
            break;
            }
    }
    return I0*sum;
}

scalar sasfit_ff_ring_under_shearflow2_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_ring_under_shearflow2_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}
