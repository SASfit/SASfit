/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define ETA	param->p[1]
#define Q   param->p[MAXPAR-1]
scalar sasfit_sq__d_hard_disks__rosenfeld_random_x(scalar x, sasfit_param * param)
{
    scalar A, B, Z, XI, G, _1Sq, qR;


	qR=Q*R*sqrt(1-x*x);
    G = 1./(1.-ETA);
    XI = (1.+ETA)/gsl_pow_3(1.-ETA);
    B=((1.-ETA)*XI-1.-3.*ETA*G)/ETA;
	A=(1.+(2.*ETA-1.)*XI+2*ETA*G)/ETA;
	_1Sq= 1.+4.*ETA*(A*gsl_pow_2(sasfit_jinc(qR))
                    +B*gsl_sf_bessel_J0(qR)*sasfit_jinc(qR)
                    +G*0.5*sasfit_jinc(2.*qR));
	return 1./_1Sq;
}

scalar sasfit_sq__d_hard_disks__rosenfeld_random(scalar q, sasfit_param * param)
{
    scalar A, B, Z, XI, G, _1Sq, qR;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA <= 0.0), param, "eta(%lg) <= 0",ETA); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA >= 1.0), param, "eta(%lg) >= 1",ETA); // modify condition to your needs
	// insert your code here
	Q=q;
	return sasfit_integrate(0,1,&sasfit_sq__d_hard_disks__rosenfeld_random_x,param);
}

scalar sasfit_sq__d_hard_disks__rosenfeld_random_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq__d_hard_disks__rosenfeld_random_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

