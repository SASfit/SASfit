/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define A	param->p[0]
#define B	param->p[1]
#define KAPPA	param->p[2]
#define DELTA	(param->p[3]*M_PI/180.)


scalar sasfit_ff_maiersaupe__deg_(scalar psi, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return psi_quarter_deg(PSI);
	return A+B*MaierSaupeODF(psi*M_PI/180-DELTA,KAPPA);
}

scalar sasfit_ff_maiersaupe__deg__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_maiersaupe__deg__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

