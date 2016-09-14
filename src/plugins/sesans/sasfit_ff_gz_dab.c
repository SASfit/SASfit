/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define XI	param->p[0]
#define DUMMY1 param->p[1]
#define DUMMY2 param->p[2]
#define ETA	param->p[3]

scalar sasfit_ff_gz_dab(scalar z, sasfit_param * param)
{
    scalar u,Gz,G0;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((z < 0.0), param, "z(%lg) < 0",z);
	SASFIT_CHECK_COND1((XI < 0.0), param, "xi(%lg) < 0",XI); // modify condition to your needs

	// insert your code here
    u=z/XI;
    G0=16*M_PI*gsl_pow_4(XI)*ETA*ETA;
    if (fabs(u)<1e-6) {
        if (u*u==0) {
            Gz=G0;
        } else {
            Gz=G0*(1+u*u/4.*(2*M_EULER)-1+log(u*u/4.));
        }
    } else {
        Gz=G0*u*gsl_sf_bessel_K1(u);
    }
	return (Gz-G0)*gsl_pow_2(2*M_PI); // not clear yet wy /gsl_pow_2(2*M_PI); is needed
}

scalar sasfit_ff_gz_dab_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_gz_dab_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

