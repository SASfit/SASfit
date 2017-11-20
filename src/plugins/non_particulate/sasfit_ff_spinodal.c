/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define IMAX	param->p[0]
#define QMAX	param->p[1]
#define GAMMA	param->p[2]

scalar sasfit_ff_spinodal(scalar q, sasfit_param * param)
{
    scalar tmp,c;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((IMAX < 0.0), param, "Imax(%lg) < 0",IMAX); // modify condition to your needs
	SASFIT_CHECK_COND1((QMAX < 0.0), param, "Qmax(%lg) < 0",QMAX); // modify condition to your needs

	// insert your code here
    
    tmp = q/QMAX;
	c = GAMMA/2.+pow(tmp,2.+GAMMA);

	SASFIT_CHECK_COND((c == 0.0), param, "gamma/2+(q/Qmax)^(2+gamma) == 0");

	return IMAX*tmp*tmp*(1.+GAMMA/2.)/c;
}

scalar sasfit_ff_spinodal_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_spinodal_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

