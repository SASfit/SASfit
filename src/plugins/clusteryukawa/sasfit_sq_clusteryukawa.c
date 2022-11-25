/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define CYR	param->p[0]
#define CYK	param->p[1]
#define CYLAMBDA	param->p[2]
#define CYPHI	param->p[3]
#define CYMOLARITY	param->p[4]
#define CYD	param->p[5]
#define CYNN	param->p[6]
#define CYDELTA	param->p[7]

scalar sasfit_sq_clusteryukawa(scalar q, sasfit_param * param)
{
    double S1_yukawa, S2_cluster;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((CYR < 0.0), param, "R(%lg) < 0",CYR); // modify condition to your needs
	SASFIT_CHECK_COND1((CYLAMBDA < 0.0), param, "lambda(%lg) < 0",CYLAMBDA); // modify condition to your needs
	SASFIT_CHECK_COND1((CYPHI <= 0.0), param, "phi(%lg) <= 0",CYPHI); // modify condition to your needs
	SASFIT_CHECK_COND1((CYMOLARITY < 0.0), param, "molarity(%lg) < 0",CYMOLARITY); // modify condition to your needs
	SASFIT_CHECK_COND1((CYD < 0.0), param, "D(%lg) < 0",CYD); // modify condition to your needs
	SASFIT_CHECK_COND1((CYNN < 0.0), param, "n(%lg) < 0",CYNN); // modify condition to your needs
	SASFIT_CHECK_COND1((CYDELTA < 0.0), param, "Delta(%lg) < 0",CYDELTA); // modify condition to your needs

	// insert your code here

    sasfit_init_param( &subParam );

	subParam.p[0] = CYR;
    subParam.p[1] = CYK;
    subParam.p[2] =	CYLAMBDA;
    subParam.p[5] = CYPHI;
    subParam.p[6] = CYMOLARITY;

	S1_yukawa=sasfit_sq_one_yukawa(q, &subParam);


	subParam.p[0] = CYD;
    subParam.p[2] = CYNN;
    subParam.p[1] =	CYDELTA;

	S2_cluster=sasfit_sq_paracrystal_1d_random(q, &subParam);

	return S1_yukawa*S2_cluster;
}

scalar sasfit_sq_clusteryukawa_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_clusteryukawa_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

