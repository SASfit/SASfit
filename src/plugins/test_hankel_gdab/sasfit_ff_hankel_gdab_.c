/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define A	param->p[0]
#define H	param->p[1]
#define DUMMY	param->p[2]
#define ETA	param->p[3]

scalar gdab_hankel0(scalar q, sasfit_param * param) {
    return q*sasfit_ff_g_dab(q,param);
}

scalar gdab_t(scalar q, void * param) {
    sasfit_param* t_param;
    t_param = (sasfit_param *) param;
    return sasfit_ff_g_dab(q,t_param);
}

scalar sasfit_ff_hankel_gdab_(scalar r, sasfit_param * param)
{
    scalar G0, Gr;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((A < 0.0), param, "a(%lg) < 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((H <= -0.5), param, "H(%lg) <= -0.5",H); // modify condition to your needs

	// insert your code here
	G0=sasfit_integrate(0,GSL_POSINF,&gdab_hankel0,param);
	Gr=sasfit_hankel(0,&gdab_t,fabs(r),param);
	return (Gr-G0)/(2*M_PI);
}

scalar sasfit_ff_hankel_gdab__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_hankel_gdab__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

