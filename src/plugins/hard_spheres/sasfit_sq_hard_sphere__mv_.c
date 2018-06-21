/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
double Z_MV(double volf, void * pam){
    sasfit_param *param;
    param = (sasfit_param *) pam;
    scalar ETA2,ETA3;
    ETA2=gsl_pow_2(volf);
    ETA3=gsl_pow_3(volf);
    return     (1+1.0560*volf+1.6539*ETA2+0.3262*ETA3)
            / ((1+0.0560*volf+0.5979*ETA2+0.3076*ETA3)*gsl_pow_3(1-volf));
}
scalar sasfit_sq_hard_sphere__mv_(scalar q, sasfit_param * param)
{
  gsl_function F;
  double result, abserr;

    F.function = &Z_MV;
    F.params = param;
    gsl_deriv_central (&F, ETA, 1e-8, &result, &abserr);
    ZHS=Z_MV(ETA,param);
    CHI=1./(ETA*result+ZHS);
    set_params(param);

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA < 0.0), param, "eta(%lg) < 0",ETA); // modify condition to your needs

	// insert your code here
	return 1-24*ETA*ReGfunc(q,param);
}

scalar sasfit_sq_hard_sphere__mv__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_hard_sphere__mv__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

