/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define RG	param->p[0]
#define VM	param->p[1]
#define C	param->p[2]
#define I0	param->p[3]

scalar sasfit_ff_fract_branched_polym(scalar q, sasfit_param * param)
{
	scalar ub, norm_inv;
	scalar (*gamma) (scalar);
	scalar (*gamma_inc) (scalar, scalar);
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((RG < 0.0), param, "Rg(%lg) < 0",RG); // modify condition to your needs
	SASFIT_CHECK_COND1((VM <= 0.0), param, "vm(%lg) < 0",VM); // modify condition to your needs
	SASFIT_CHECK_COND1((C <= 0.0), param, "c(%lg) < 0",C); // modify condition to your needs

	// insert your code here
	ub = q*q * RG*RG * (2.*VM+C) *(2.*VM+C+1.)/6.;
	norm_inv = I0*0.5*(C*C+C);
	gamma=gsl_sf_gamma;
	gamma_inc=gsl_sf_gamma_inc;
	if (q*RG<0.01) return I0*exp(-q*q*RG*RG/3);
	return (  (gamma(0.5*  C     / VM) - gamma_inc(0.5*  C     / VM,ub)) / (VM*pow(ub, 0.5*  C    / VM))
	        - (gamma(0.5* (C+1.) / VM) - gamma_inc(0.5* (C+1.) / VM,ub)) / (VM*pow(ub, 0.5* (C+1) / VM))) * norm_inv;
}

scalar sasfit_ff_fract_branched_polym_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_fract_branched_polym_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

