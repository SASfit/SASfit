/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define A	param->p[0]
#define CENTER	param->p[1]
#define W1	fabs(param->p[2])
#define W2	fabs(param->p[3])
#define SIGMA	param->p[4]
#define C0	param->p[5]

scalar sasfit_ff_smoothed_symm_trapez_ampl(scalar x, sasfit_param * param)
{
    scalar u;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return A*(sasfit_ff_smoothed_symm_trapez_area(x,param)-C0)/(sasfit_ff_smoothed_symm_trapez_area(CENTER,param)-C0)+C0;
}

scalar sasfit_ff_smoothed_symm_trapez_ampl_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_smoothed_symm_trapez_ampl_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

