/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define G2	param->p[0]
#define S2	param->p[1]
#define RG2	param->p[2]
#define S1	param->p[3]
#define RG1	param->p[4]
#define M	param->p[5]

scalar sasfit_ff_generalized_guinier_porod_law(scalar q, sasfit_param * param)
{
	scalar Q1, Q2, G1,D;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((G2 < 0.0), param, "G2(%lg) < 0",G2); // modify condition to your needs
	SASFIT_CHECK_COND1((S2 < 0.0), param, "s2(%lg) < 0",S2); // modify condition to your needs
	SASFIT_CHECK_COND1((RG2 < 0.0), param, "Rg2(%lg) < 0",RG2); // modify condition to your needs
	SASFIT_CHECK_COND1((S1 < 0.0), param, "s1(%lg) < 0",S1); // modify condition to your needs
	SASFIT_CHECK_COND1((RG1 < 0.0), param, "Rg1(%lg) < 0",RG1); // modify condition to your needs
	SASFIT_CHECK_COND1((M < 0.0), param, "m(%lg) < 0",M); // modify condition to your needs

	// insert your code here
	Q2 = sqrt((S1-S2)/(2./(3.-S2)*RG2*RG2-2./(3.-S1)*RG1*RG1));
	G1 = G2*exp(Q2*Q2*(RG1*RG1/(3.-S1)-RG2*RG2/(3.-S2)))*pow(Q2,S1-S2);
	Q1 = 1./RG1*sqrt((M-S1)*(3.-S1)/2.);
	D=G1*exp(-Q1*Q1*RG1*RG1/(3.-S1))*pow(Q1,M-S1);
	if (q<=Q2) {
		return G2/pow(q,S2)*exp(-q*q*RG2*RG2/(3.-S2));
	} else if ((Q2<q) && (q<Q1)) {
		return G1/pow(q,S1)*exp(-q*q*RG1*RG1/(3.-S1));
	} else {
		return D/pow(q,M);
	}
}

scalar sasfit_ff_generalized_guinier_porod_law_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_generalized_guinier_porod_law_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

