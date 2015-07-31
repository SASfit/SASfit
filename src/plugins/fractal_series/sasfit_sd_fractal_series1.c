/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables
#define N	param->p[0]
#define XMIN	param->p[1]
#define FD1	param->p[2]
#define XMAX	param->p[3]

scalar sasfit_sd_fractal_series1(scalar x, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((x < 0.0), param, "q(%lg) < 0",x);
	SASFIT_CHECK_COND1((N < 0.0), param, "N(%lg) < 0",N); // modify condition to your needs
	SASFIT_CHECK_COND1((XMIN < 0.0), param, "xmin(%lg) < 0",XMIN); // modify condition to your needs
	SASFIT_CHECK_COND1((FD1 < 0.0), param, "fd1(%lg) < 0",FD1); // modify condition to your needs
	SASFIT_CHECK_COND1((XMAX < 0.0), param, "xmax(%lg) < 0",XMAX); // modify condition to your needs

	// insert your code here
	return fractalSD_single(x,N,FD1,XMIN,XMAX,param);
}

scalar sasfit_sd_fractal_series1_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sd_fractal_series1_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

