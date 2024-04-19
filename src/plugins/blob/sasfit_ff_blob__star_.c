/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>
#include <math.h>

// define shortcuts for local parameters/variables
#define RG	param->p[0]
#define NU	param->p[1]
#define F	param->p[2]
#define ALPHA	param->p[3]

scalar sasfit_ff_blob__star_(scalar q, sasfit_param * param)
{
    scalar mu, qb, c, blob;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((RG < 0.0), param, "Rg(%lg) < 0",RG); // modify condition to your needs
	SASFIT_CHECK_COND1((NU <= 1./3.), param, "nu(%lg) < 1/3",NU); // modify condition to your needs
	SASFIT_CHECK_COND1((F <= 0.0), param, "f(%lg) <= 0",F); // modify condition to your needs
	SASFIT_CHECK_COND1((NU > 1.0), param, "nu(%lg) > 1",NU); // modify condition to your needs

	// insert your code here
	if (q==0) {
        return ALPHA*fabs(pow(2,1/NU)*pow(F,-0.5/NU)*(-1 + 1/NU)*pow(RG,1/NU)/cos(M_PI/(2*NU)));
	}
	if (NU==1) {
        return (2*ALPHA*atan((2*q*RG)/sqrt(F)))/(M_PI*q);
	}
	mu = 1/NU - 1;
    qb = q*2*RG/sqrt(F);
    c =  fabs(pow(2,2 - 1/NU)*pow(F,0.5/NU)*M_PI*pow(RG,-1/NU)
        *cos(M_PI/(2*NU))*gsl_sf_gamma(mu));
    blob = ALPHA*fabs(4*M_PI*gsl_sf_gamma(mu)/qb*sin(mu*atan(qb))/pow(1 + qb*qb,mu/2.)/c);
	return blob;
}

scalar sasfit_ff_blob__star__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_blob__star__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

