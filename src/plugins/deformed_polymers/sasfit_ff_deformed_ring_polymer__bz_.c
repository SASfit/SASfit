/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define RG_LL	param->p[0]
#define RG_PERP	param->p[1]
#define NU	param->p[2]
#define I0	param->p[3]
#define THETA_0	param->p[4]*M_PI/180.
#define PSIDEG	param->p[5]
#define PSI	param->p[MAXPAR-1]

scalar sasfit_ff_deformed_ring_polymer__bz_(scalar q, sasfit_param * param)
{

	scalar Dpsi, q_ll, q_perp, beta, u;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((RG_LL < 0.0), param, "Rg_ll(%lg) < 0",RG_LL); // modify condition to your needs
	SASFIT_CHECK_COND1((RG_PERP < 0.0), param, "Rg_perp(%lg) < 0",RG_PERP); // modify condition to your needs
	SASFIT_CHECK_COND1((NU < 0.0), param, "nu(%lg) < 0",NU); // modify condition to your needs
	SASFIT_CHECK_COND1((I0 < 0.0), param, "I0(%lg) < 0",I0); // modify condition to your needs
	SASFIT_CHECK_COND1((THETA_0 < 0.0), param, "theta_0(%lg) < 0",THETA_0); // modify condition to your needs
	SASFIT_CHECK_COND1((PSI < 0.0), param, "psi(%lg) < 0",PSI); // modify condition to your needs

	// insert your code here
    PSI = sasfit_param_override_get_psi(PSIDEG*M_PI/180.);
	Dpsi = PSI-THETA_0;
	q_ll = q*cos(Dpsi);
	q_perp = q*sin(Dpsi);
    beta = (gsl_pow_2(RG_LL*q_ll)+ gsl_pow_2(RG_PERP*q_perp));

	subParam.p[0] = sqrt(gsl_pow_2(RG_LL*cos(Dpsi))+gsl_pow_2(RG_PERP*sin(Dpsi)));
	subParam.p[2] = NU;
	subParam.p[3] = I0;
	return sasfit_ff_ringpolymerBZ(q,&subParam);
}

scalar sasfit_ff_deformed_ring_polymer__bz__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_deformed_ring_polymer__bz__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

