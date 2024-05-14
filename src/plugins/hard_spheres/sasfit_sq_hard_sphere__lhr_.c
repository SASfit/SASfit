/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
double Z_LHR(double volf, void * pam){
    sasfit_param *param;
    param = (sasfit_param *) pam;
    scalar ETA2,ETA3,ETA4,ETA5;
    ETA2=gsl_pow_2(volf);
    ETA3=gsl_pow_3(volf);
    ETA4=gsl_pow_4(volf);
    ETA5=gsl_pow_5(volf);
    return    (1 + 0.153555*volf - 0.428476*ETA2 - 2.7987*ETA3 - 0.317417*ETA4 - 0.105806*ETA5)
            / (1 - 3.84644*volf  + 4.9574*ETA2   - 2.16386*ETA3);
}

scalar sasfit_sq_hard_sphere__lhr_(scalar q, sasfit_param * param)
{
  gsl_function F;
  double result, abserr;

    F.function = &Z_LHR;
    F.params = param;
    gsl_deriv_central (&F, ETA, 1e-8, &result, &abserr);
    ZHS=Z_LHR(ETA,param);
    CHI=1./(ETA*result+ZHS);
    set_params(param);
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA < 0.0), param, "eta(%lg) < 0",ETA); // modify condition to your needs

	// insert your code here
	return 1-24*ETA*ReGfunc(q,param);
}

scalar sasfit_sq_hard_sphere__lhr__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_hard_sphere__lhr__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

