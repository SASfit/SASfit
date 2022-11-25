/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define N	param->p[0]
#define XMIN	param->p[1]
#define FD1	param->p[2]
#define X12	param->p[3]
#define FD2	param->p[4]
#define XMAX	param->p[5]

scalar sasfit_ff_fractal_series2(scalar q, sasfit_param * param)
{
    scalar a1,a2;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((N < 0.0), param, "N(%lg) < 0",N); // modify condition to your needs
	SASFIT_CHECK_COND1((XMIN < 0.0), param, "xmin(%lg) < 0",XMIN); // modify condition to your needs
	SASFIT_CHECK_COND1((FD1 < 0.0), param, "fd1(%lg) < 0",FD1); // modify condition to your needs
	SASFIT_CHECK_COND1((X12 < 0.0), param, "x12(%lg) < 0",X12); // modify condition to your needs
	SASFIT_CHECK_COND1((FD2 < 0.0), param, "fd2(%lg) < 0",FD2); // modify condition to your needs
	SASFIT_CHECK_COND1((XMAX < 0.0), param, "xmax(%lg) < 0",XMAX); // modify condition to your needs

    a1 = (FD2*N*pow(XMAX,FD2)*(pow(X12,FD1) - pow(XMIN,FD1)))
        /(   FD2*pow(X12,FD1)*pow(XMAX,FD2)
          - (FD1*pow(X12,FD2) + (FD2-FD1)*pow(XMAX,FD2))*pow(XMIN,FD1));
    a2 = (FD1*N*(pow(X12,FD2) - pow(XMAX,FD2))*pow(XMIN,FD1))
        /(  -FD2*pow(X12,FD1)*pow(XMAX,FD2)
          + (FD1*pow(X12,FD2) + (FD2-FD1)*pow(XMAX,FD2))*pow(XMIN,FD1));
	// insert your code here
	return fractalFF_single(q,a1,FD1,XMIN,X12,param)
         + fractalFF_single(q,a2,FD2,X12 ,XMAX,param);
}

scalar sasfit_ff_fractal_series2_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_fractal_series2_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

