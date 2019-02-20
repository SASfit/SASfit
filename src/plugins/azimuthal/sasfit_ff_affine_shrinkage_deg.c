/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define B	param->p[0]
#define A	param->p[1]
#define KAPPA	fabs(param->p[2])
#define DELTA	param->p[3]
#define DPSI    param->p[MAXPAR-1]

scalar as_kernel_deg(scalar dpsi,sasfit_param *param) {
	scalar res;
	if (fabs(M_PI_2-dpsi) < 1e-6*M_PI_2) {
		res= 1/KAPPA;
	} else {
		res = gsl_pow_2(KAPPA)/(gsl_pow_3(cos(dpsi))*pow(1+gsl_pow_2(KAPPA*tan(dpsi)),1.5));
//		res = gsl_pow_2(KAPPA)*gsl_pow_3(cos(atan(KAPPA*tan(dpsi)))); // This should be the same than the line before as  cos(atan(x))=1/sqrt(1+x^2)
	}
	return res;
}

scalar sasfit_ff_affine_shrinkage_deg(scalar psi, sasfit_param * param)
{
	scalar dpsi,res;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid
	SASFIT_CHECK_COND1((KAPPA == 0.0), param, "lambda(%lg) == 0",KAPPA);

	// insert your code here

    DPSI=psi_quarter_deg(psi-DELTA)*M_PI/180.;
	res = as_kernel_deg(DPSI,param)/sasfit_integrate(0,M_PI_2,&as_kernel_deg,param) * M_PI_2;
	return B+A*res;
}

scalar sasfit_ff_affine_shrinkage_deg_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_affine_shrinkage_deg_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

