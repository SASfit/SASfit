/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables

scalar GPY(scalar fp, scalar A)
{
        scalar gamma, alpha, beta;

        alpha = pow(1.0+2.0*fp,2.0)/pow(1.0-fp,4.0);
        beta  = -6.0*fp*pow(1.0+fp/2.0,2.0)/pow(1.0-fp,4.0);
        gamma = fp*alpha/2.0;
        return  alpha*(sin(A)-A*cos(A))/pow(A,2.0)  +
                beta*(2.0*A*sin(A)+(2.0-pow(A,2.0)) * cos(A)-2.0)/pow(A,3.0)  +
                gamma * (-pow(A,4.0) * cos(A) + 4.0*((3.0*pow(A,2.0)-6.0)*cos(A)+(pow(A,3.0)-6.0*A)*sin(A)+6.0))/pow(A,5.0);
}

scalar sasfit_sq_hard_sphere__py_(scalar q, sasfit_param * param)
{
    scalar fp, A;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA < 0.0), param, "eta(%lg) < 0",ETA); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA >= 1.0), param, "eta(%lg) >= 1",ETA);

	// insert your code here

	fp=ETA;

	if (fp == 0.0) return 1.0;

	A=2.0*R*q;
	if (fabs(A) > 1E-6) {
		return 1.0/(1.0+24.0*fp*GPY(fp,A)/A);
	} else {
		return gsl_pow_4(fp-1.0)/gsl_pow_2(2.0*fp+1.0)
			+(gsl_pow_4(-1 + fp)*fp*(16 + fp*(-11 + 4*fp)))/(20.*gsl_pow_4(1 + 2*fp)) * gsl_pow_2(A)
			+(gsl_pow_4(-1 + fp)*fp*(-20 + fp*(386 + fp*(-627 + fp*(494 + fp*(-173 + 21*fp))))))/
			   (700.*gsl_pow_6(1 + 2*fp))* gsl_pow_4(A);
	}
}

scalar sasfit_sq_hard_sphere__py__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_hard_sphere__py__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

