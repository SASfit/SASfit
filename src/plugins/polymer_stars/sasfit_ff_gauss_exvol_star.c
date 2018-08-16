/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define RG	param->p[0]
#define F	param->p[1]
#define NU	param->p[2]
#define I0	param->p[3]
#define TEST	param->p[4]

scalar sasfit_ff_gauss_exvol_star(scalar q, sasfit_param * param)
{
	sasfit_param subParam;
    scalar u,P1f,F1f,P1f_2,Pib;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((F < 1), param, "f(%lg) < 1",F);
	SASFIT_CHECK_COND1((RG < 0.0), param, "Rg(%lg) < 0",RG);
	// insert your code here

	sasfit_init_param( &subParam );
	
	subParam.p[0] = RG;
	subParam.p[1] = NU;
	subParam.p[2] = 1.0;
	P1f=sasfit_ff_generalized_gaussian_coil(q,&subParam);
	F1f=sasfit_ff_generalized_gaussian_coil_f(q,&subParam);
	
	subParam.p[0] = pow(2,NU)*RG;
	subParam.p[2] = 1.0;
	P1f_2=sasfit_ff_generalized_gaussian_coil(q,&subParam);
//	TEST = 0;
	if (TEST==1) {
		Pib = F1f*F1f;
	} else {
		Pib = 2*P1f_2-P1f;
	}
	return I0/F*(P1f+(F-1)*Pib);
}

scalar sasfit_ff_gauss_exvol_star_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_gauss_exvol_star_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

