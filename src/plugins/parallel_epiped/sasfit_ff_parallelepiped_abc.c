/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */
 
#include "../../sasfit_common/cubature/cubature.h"
#include <sasfit.h>

#include <sasfit_common.h>
#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define A	param->p[0]
#define B	param->p[1]
#define C	param->p[2]
#define ETA	param->p[3]
#define Q	param->p[MAXPAR-1]
#define ALPHA	param->p[MAXPAR-2]
#define BETA	param->p[MAXPAR-3]

scalar sinc(scalar x)
{
	if (fabs(x) <= 1e-4) {
		return 1.0-gsl_pow_2(x)/6.0
		          +gsl_pow_4(x)/120.0
		          -gsl_pow_6(x)/5040.0
		          +gsl_pow_8(x)/362880.0;
	} else {
		return sin(x)/x;
	}
}

scalar Kernel_P(const double *gam, void * pam) {
	sasfit_param *param;
	param = (sasfit_param *) pam;
	param->p[MAXPAR-3] = gam[0];
	param->p[MAXPAR-2] = gam[1];
	return 2./M_PI * gsl_pow_2(sinc(Q*A/2.0*sin(param->p[MAXPAR-2])*cos(param->p[MAXPAR-3])) *
				               sinc(Q*B/2.0*sin(param->p[MAXPAR-2])*sin(param->p[MAXPAR-3])) *
							   sinc(Q*C/2.0*cos(param->p[MAXPAR-2]))) * sin(param->p[MAXPAR-2]);
}

scalar Kernel_P_OOURA2(scalar x, void * pam) {
	sasfit_param *param;
	scalar gam[2];
	param = (sasfit_param *) pam;
	ALPHA = x;
	gam[1] = ALPHA;
	gam[0] = BETA;
	return Kernel_P(gam,pam);
}

scalar Kernel_P_OOURA1(scalar x, void * pam) {
	sasfit_param *param;
	scalar sum, err, *aw;
	int lenaw;
	
	lenaw=10000;
	param = (sasfit_param *) pam;
	BETA = x;
	switch(sasfit_get_int_strategy()) {
    case OOURA_DOUBLE_EXP_QUADRATURE: {
			aw = (scalar *)malloc((lenaw)*sizeof(scalar));
			sasfit_intdeini(lenaw,GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
			sasfit_intde(&Kernel_P_OOURA2, 0.0, M_PI/2., aw, &sum, &err,param);
			free(aw);
            break;
            } 
    case OOURA_CLENSHAW_CURTIS_QUADRATURE: {
			aw = (scalar *)malloc((lenaw+1)*sizeof(scalar));
			sasfit_intccini(lenaw, aw);
			sasfit_intcc(&Kernel_P_OOURA2, 0.0, M_PI/2., sasfit_eps_get_nriq(), lenaw, aw, &sum, &err,param);
			free(aw);
            break;
            }
	}
	return sum;
}

int K_cubature(unsigned ndim, const double *x, void *pam,
      unsigned fdim, double *fval) {
	fval[0] = 0;
	if ((ndim < 2) || (fdim < 1)) {
		sasfit_out("false dimensions fdim:%d ndim:%d\n",fdim,ndim);
		return 1;
	}
    fval[0] = Kernel_P(x,pam);
    return 0;
} 

scalar Psi_P_kernel(scalar y, sasfit_param * param)
{
	param->p[MAXPAR-2] = y;
	return 2./M_PI * gsl_pow_2(sinc(Q*A/2.0*sin(param->p[MAXPAR-2])*cos(param->p[MAXPAR-3])) *
				               sinc(Q*B/2.0*sin(param->p[MAXPAR-2])*sin(param->p[MAXPAR-3])) *
							   sinc(Q*C/2.0*cos(param->p[MAXPAR-2]))) * sin(param->p[MAXPAR-2]);
}

scalar Psi_kernel(scalar x, sasfit_param * param)
{
	param->p[MAXPAR-3] = x;
	return sasfit_integrate(0.0,M_PI/2.0,&Psi_P_kernel,param);
}

scalar sasfit_ff_parallelepiped_abc(scalar q, sasfit_param * param)
{
	scalar *aw, res,err,sum;
    scalar cubxmin[2], cubxmax[2], fval[1], ferr[1];
    gsl_integration_workspace * w;
    gsl_integration_cquad_workspace * wcquad;
    gsl_integration_glfixed_table * wglfixed;
    gsl_function F;
    size_t neval;
    int lenaw=4000;
	
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((A < 0.0), param, "a(%lg) < 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((B < 0.0), param, "b(%lg) < 0",B); // modify condition to your needs
	SASFIT_CHECK_COND1((C < 0.0), param, "c(%lg) < 0",C); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA < 0.0), param, "eta(%lg) < 0",ETA); // modify condition to your needs

	Q=q;
	// insert your code here
	switch(sasfit_get_int_strategy()) {
    case OOURA_DOUBLE_EXP_QUADRATURE: {
            aw = (scalar *)malloc((lenaw)*sizeof(scalar));
            sasfit_intdeini(lenaw, GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
            sasfit_intde(&Kernel_P_OOURA1, 0,M_PI/2.0, aw, &res, &err, param);
			sum=res;
            free(aw);
            break;
            } 
    case OOURA_CLENSHAW_CURTIS_QUADRATURE: {
            aw = (scalar *)malloc((lenaw+1)*sizeof(scalar));
            sasfit_intccini(lenaw, aw);
            sasfit_intcc(&Kernel_P_OOURA1, 0,M_PI/2.0, sasfit_eps_get_nriq(), lenaw, aw, &res, &err,param);
			sum=res;
            free(aw);
            break;
            }
    case H_CUBATURE: {
			cubxmin[0]=0;
			cubxmax[0]=M_PI/2.0;
			cubxmin[1]=0;
			cubxmax[1]=M_PI/2.0;
			hcubature(1, &K_cubature,param,2, cubxmin, cubxmax, 
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED, 
				fval, ferr);
			sum = fval[0];
            break;
            }
    case P_CUBATURE: {
			cubxmin[0]=0;
			cubxmax[0]=M_PI/2.0;
			cubxmin[1]=0;
			cubxmax[1]=M_PI/2.0;
			pcubature(1, &K_cubature,param,2, cubxmin, cubxmax, 
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED, 
				fval, ferr);
			sum = fval[0];
            break;
            }
    default: {
//		    sasfit_out("ise default sasfit_integrate routine\n");
            sum=sasfit_integrate(0.0,M_PI/2.0,&Psi_kernel,param);;
            break;
            }
    }
	return gsl_pow_2(ETA*sasfit_ff_parallelepiped_abc_v(q,param,1))	*sum;
}

scalar sasfit_ff_parallelepiped_abc_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_parallelepiped_abc_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return A*B*C;
}

