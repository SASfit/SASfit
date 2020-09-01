/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define A	param->p[0]
#define CENTER	param->p[1]
#define WP	fabs(param->p[2])
#define WE	fabs(param->p[3])

#define PHI	1
#define N1	param->p[6]

#define C0	param->p[8]

#define ALPHA	(CENTER-WP/2-WE/2)
#define BETA	(CENTER-WP/2)
#define GAMMA	(CENTER+WP/2)
#define DELTA	(CENTER+WP/2+WE/2)
#define N3      N1

scalar sasfit_ff_symm__general__trapez__ampl_(scalar x, sasfit_param * param)
{
    scalar u,p;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND2((ALPHA>=BETA), param, "alpha(%lg)>=beta(%lg)",ALPHA,BETA); // modify condition to your needs
	SASFIT_CHECK_COND2((BETA>=GAMMA), param, "beta(%lg)>=gamma(%lg)",BETA,GAMMA); // modify condition to your needs
	SASFIT_CHECK_COND2((GAMMA>=DELTA), param, "gamma(%lg)>=delta(%lg)",GAMMA,DELTA); // modify condition to your needs

	SASFIT_CHECK_COND1((N1 <= 0.0), param, "n1(%lg) <= 0",N1); // modify condition to your needs

	u = 2*N1*N3/(2*PHI*(BETA-ALPHA)*N3+(PHI+1)*(GAMMA-BETA)*N1*N3+2*(DELTA-GAMMA)*N1);
	p=0;
	if (ALPHA<x && x<BETA) {
        p=PHI*u*pow((x-ALPHA)/(BETA-ALPHA),N1-1);
	} else if (BETA<=x && x<GAMMA) {
	    p=u*((PHI-1)*(GAMMA-x)/(GAMMA-BETA)+1);
	} else if (GAMMA<=x && x<DELTA) {
	    p=u*pow((DELTA-x)/(DELTA-GAMMA),N3-1);
	}
	// insert your code here
	return A*p/GSL_MAX(u*PHI,u)+C0;
}

scalar sasfit_ff_symm__general__trapez__ampl__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_symm__general__trapez__ampl__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

