/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables

double Z_4_3(double volf, void * pam){
    sasfit_param *param;
    param = (sasfit_param *) pam;
    scalar ETA2,ETA3,ETA4;
    ETA2=gsl_pow_2(volf);
    ETA3=gsl_pow_3(volf);
    ETA4=gsl_pow_4(volf);
    return    (1+1.024385*volf+1.104537*ETA2-0.4611472*ETA3-0.7430382*ETA4)
            / (1-2.975615*volf+3.007000*ETA2-1.097758*ETA3);
}

scalar sasfit_sq_hard_sphere__4_3_(scalar q, sasfit_param * param)
{
  gsl_function F;
  double result, abserr;

    F.function = &Z_4_3;
    F.params = param;
    gsl_deriv_central (&F, ETA, 1e-8, &result, &abserr);
    ZHS=Z_4_3(ETA,param);
    CHI=1./(ETA*result+ZHS);
    set_params(param);

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA < 0.0), param, "eta(%lg) < 0",ETA); // modify condition to your needs

	// insert your code here
	return 1-24*ETA*ReGfunc(q,param);
}

scalar sasfit_sq_hard_sphere__4_3__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_hard_sphere__4_3__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

