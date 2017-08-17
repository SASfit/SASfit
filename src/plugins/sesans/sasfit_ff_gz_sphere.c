/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define DUMMY1 param->p[1]
#define DUMMY2 param->p[2]
#define ETA	param->p[3]
scalar sasfit_ff_gz_sphere(scalar z, sasfit_param * param)
{
	scalar xi,xi2,Gz,G0;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((z < 0.0), param, "z(%lg) < 0",z);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs

	// insert your code here
    G0 =  2*M_PI*gsl_pow_4(R)*ETA*ETA;
	if (z>2*R)return -G0*gsl_pow_2(2*M_PI);
	if (z==0) return 0;
    xi=z/(2*R);
    xi2=xi*xi;
    Gz = M_PI*gsl_pow_4(R)*ETA*ETA*(sqrt(1-xi2)*(2+xi2)+xi2*(4-xi2)*log(xi/(1+sqrt(1-xi2))));
	return (Gz-G0); 
}

scalar sasfit_ff_gz_sphere_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_gz_sphere_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

