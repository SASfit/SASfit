/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define RG	param->p[0]
#define M	param->p[1]

#define I0	param->p[3]

void calcPmr (long k, scalar u, scalar P1r, scalar *Pmr) {
    long i;
	for (i=1; i < k ;i++)
	{
		(*Pmr) = (*Pmr) + (k-i)*exp(-0.5*u*(i-1.0));
	}

	(*Pmr) = P1r/(1.0*k) + 2.0/(1.0*k*k)*P1r*P1r*(*Pmr);
}

scalar sasfit_ff_mmemberedtwistedring(scalar q, sasfit_param * param)
{
	scalar w, u, P1r, Pmr, Pmr_p1;
	long i, k;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((RG < 0.0), param, "Rg,1r(%lg) < 0",RG); // modify condition to your needs
	SASFIT_CHECK_COND1((M < 0.0), param, "m(%lg) < 0",M); // modify condition to your needs

	// insert your code here

	Pmr = 0.0;
	Pmr_p1 =0.0;
	k = lround(floor(M));
	w = M-floor(M);

	u = q*RG;

	if (u  == 0.0) {
		P1r = 1;
	} else {
		P1r = sqrt(2./(u*u))*gsl_sf_dawson(sqrt(u*u/2.));
	}
	calcPmr(k,u,P1r,&Pmr);
	calcPmr(k+1,u,P1r,&Pmr_p1);
	return I0*((1-w)*Pmr+w*Pmr_p1);
/*
	for (i=1; i < k ;i++)
	{
		Pmr = Pmr + (k-i)*exp(-0.5*u*(i-1.0));
	}

	Pmr = P1r/(1.0*k) + 2.0/(1.0*k*k)*P1r*P1r*Pmr;
*/
	return I0*Pmr;
}

scalar sasfit_ff_mmemberedtwistedring_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_mmemberedtwistedring_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

