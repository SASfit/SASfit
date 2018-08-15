/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables

scalar sasfit_sq_freely_joined_chain_of_rods(scalar q, sasfit_param * param)
{
	scalar gmma, Gmma,Gmma2, cB,cC, qb;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((LB <= 0.0), param, "lb(%lg) <= 0",LB); // modify condition to your needs
	SASFIT_CHECK_COND1((L <= 0.0), param, "L(%lg) <= 0",L); // modify condition to your needs

	// insert your code here
	qb = q*LB;
	gmma = gsl_sf_bessel_j0(qb);
	if (fabs(qb<1e-6)) {
		Gmma = 1 - gsl_pow_2(qb)/18. + gsl_pow_4(qb)/600. - gsl_pow_6(qb)/35280. + gsl_pow_8(qb)/3265920.;
		Gmma2=Gmma*Gmma;
		cB = -(19./15.)*LB/L+(1./5.)*(-2*LB+5*L)/L+(-(31./525.)*LB/L+(1/50)*(-2*LB+5*L)/L-(1./630.)*(35*L*L-42*L*LB+16*LB*LB)/(LB*L))*qb*qb;
	} else {
		Gmma = gsl_sf_Si(qb)/qb;
		Gmma2=Gmma*Gmma;
		cB = 2*LB/L*Gmma2/(1-gmma) - 2*LB*LB/L/L *Gmma2*(1-pow(gmma,L/LB))/gsl_pow_2(1-gmma);
	}
	cC=0.5*gsl_pow_2(gsl_sf_bessel_j0(qb/2));
	return L*L*(2*LB/L*(Gmma-cC)+cB);
}

scalar sasfit_sq_freely_joined_chain_of_rods_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_freely_joined_chain_of_rods_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

