/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define A	param->p[0]
#define ALPHA	param->p[1]
#define BETA	param->p[2]
#define GAMMA	param->p[3]
#define DELTA	param->p[4]
#define PHI	param->p[5]
#define N1	param->p[6]
#define N3	param->p[7]
#define C0	param->p[8]

scalar sasfit_ff_general__trapez__area(scalar x, sasfit_param * param)
{
    scalar u,p;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND2((ALPHA>=BETA), param, "alpha(%lg)>=beta(%lg)",ALPHA,BETA); // modify condition to your needs
	SASFIT_CHECK_COND2((BETA>=GAMMA), param, "beta(%lg)>=gamma(%lg)",BETA,GAMMA); // modify condition to your needs
	SASFIT_CHECK_COND2((GAMMA>=DELTA), param, "gammma(%lg)>=delta(%lg)",GAMMA,DELTA); // modify condition to your needs
	SASFIT_CHECK_COND1((PHI <= 0.0), param, "phi(%lg) <= 0",PHI); // modify condition to your needs
	SASFIT_CHECK_COND1((N1 <= 0.0), param, "n1(%lg) <= 0",N1); // modify condition to your needs
	SASFIT_CHECK_COND1((N3 <= 0.0), param, "n3(%lg) <= 0",N3); // modify condition to your needs

	u = 2*PHI*N1*N3/(2*PHI*(BETA-ALPHA)*N3+(PHI+1)*(GAMMA-BETA)*N1*N3+2*(DELTA-GAMMA)*N1);
	p=0;
	if (ALPHA<=x && x<BETA) {
        p=PHI*u*pow((x-ALPHA)/(BETA-ALPHA),N1-1);
	} else if (BETA<=x && x<GAMMA) {
	    p=u*((PHI-1)*(GAMMA-x)/(GAMMA-BETA)+1);
	} else if (GAMMA<=x && x<DELTA) {
	    p=u*pow((DELTA-x)/(DELTA-GAMMA),N3-1);
	}
	// insert your code here
	return A*p+C0;
}

scalar sasfit_ff_general__trapez__area_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_general__trapez__area_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

