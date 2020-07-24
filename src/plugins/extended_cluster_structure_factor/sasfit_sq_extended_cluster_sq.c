/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define PHI	param->p[1]
#define XI	param->p[2]
#define EPSILON	param->p[3]

scalar sasfit_sq_extended_cluster_sq(scalar q, sasfit_param * param)
{
	sasfit_param subParam;
	scalar N,VC,PC,SHS;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((PHI < 0.0), param, "phi(%lg) < 0",PHI); // modify condition to your needs
	SASFIT_CHECK_COND1((PHI > 1.0), param, "phi(%lg) > 1",PHI); // modify condition to your needs
	SASFIT_CHECK_COND1((XI < 0.0), param, "xi(%lg) < 0",XI); // modify condition to your needs
	SASFIT_CHECK_COND1((EPSILON <= 0.0), param, "epsilon(%lg) <= 0",EPSILON); // modify condition to your needs

	// insert your code here
    sasfit_init_param( &subParam );
	subParam.p[0] = XI;
    subParam.p[1] = EPSILON;
    subParam.p[2] =	1.0; // dummy;
    subParam.p[3] = 1; // ETA
    VC=gsl_pow_3(XI)*8*M_PI*EPSILON;
    N = VC/(4./3.*M_PI*gsl_pow_3(R));
    PC = sasfit_ff_dab_epsilon(q,&subParam)/(VC*VC);
	subParam.p[0] = R;
    subParam.p[1] = PHI;
    subParam.p[2] =	1.0; // dummy;
    subParam.p[3] = 1; // ETA
    SHS = sasfit_sq_hard_sphere__py_(q,&subParam);
	return SHS+(N-SHS)*PC;
}

scalar sasfit_sq_extended_cluster_sq_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_extended_cluster_sq_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

