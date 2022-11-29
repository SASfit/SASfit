/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define CRCLUSTER	param->p[0]
#define CKY  	param->p[1]
#define CLAMBDA	param->p[2]
#define CPHI	param->p[3]
#define CMOLARITY	param->p[4]
#define CR	param->p[5]
#define CZ1	param->p[6]
#define CXI	param->p[7]
#define CD	param->p[8]
#define CN	param->p[9]

scalar sasfit_sq_oneyukawa_on_fractal_incl_nn(scalar q, sasfit_param * param)
{
    double S1_yukawa, S2_cluster;
	sasfit_param subParam;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((CRCLUSTER < 0.0), param, "Rcluster(%lg) < 0",CRCLUSTER); // modify condition to your needs
	SASFIT_CHECK_COND1((CLAMBDA < 0.0), param, "lambda(%lg) < 0",CLAMBDA); // modify condition to your needs
	SASFIT_CHECK_COND1((CPHI < 0.0), param, "phi(%lg) < 0",CPHI); // modify condition to your needs
	SASFIT_CHECK_COND1((CR < 0.0), param, "R(%lg) < 0",CR); // modify condition to your needs
	SASFIT_CHECK_COND1((CZ1 < 0.0), param, "z1(%lg) < 0",CZ1); // modify condition to your needs
	SASFIT_CHECK_COND1((CXI < 0.0), param, "xi(%lg) < 0",CXI); // modify condition to your needs
	SASFIT_CHECK_COND1((CD <= 1.0), param, "D(%lg) <= 1",CD); // modify condition to your needs
	SASFIT_CHECK_COND1((CN < 0.0), param, "N(%lg) < 0",CN); // modify condition to your needs

	// insert your code here
	    sasfit_init_param( &subParam );

	subParam.p[0] = CRCLUSTER;
    subParam.p[1] = CKY;
    subParam.p[2] =	CLAMBDA;
    subParam.p[5] = CPHI;
    subParam.p[6] = CMOLARITY;
    S1_yukawa=sasfit_sq_one_yukawa(q, &subParam);


    subParam.p[0] = CR;
    subParam.p[1] = CZ1;
    subParam.p[2] =	CXI;
    subParam.p[3] = CD;
    subParam.p[4] = CN;
    S2_cluster=sasfit_sq_mass_fractal_incl__nn_pertubation(q, &subParam);
	return S1_yukawa*S2_cluster;
}

scalar sasfit_sq_oneyukawa_on_fractal_incl_nn_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_oneyukawa_on_fractal_incl_nn_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

