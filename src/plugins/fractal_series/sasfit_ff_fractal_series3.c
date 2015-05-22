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
#define X23	param->p[5]
#define FD3	param->p[6]
#define XMAX	param->p[7]

scalar sasfit_ff_fractal_series3(scalar q, sasfit_param * param)
{
    scalar a1,a2,a3;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((XMIN <= 0.0), param, "xmin(%lg) <= 0",XMIN); // modify condition to your needs
	SASFIT_CHECK_COND1((FD1 <= 0.0), param, "fd1(%lg) <= 0",FD1); // modify condition to your needs
	SASFIT_CHECK_COND1((X12 <= 0.0), param, "x12(%lg) <= 0",X12); // modify condition to your needs
	SASFIT_CHECK_COND1((FD2 <= 0.0), param, "fd2(%lg) <= 0",FD2); // modify condition to your needs
	SASFIT_CHECK_COND1((X23 <= 0.0), param, "x23(%lg) <= 0",X23); // modify condition to your needs
	SASFIT_CHECK_COND1((FD3 <= 0.0), param, "fd3(%lg) <= 0",FD3); // modify condition to your needs
	SASFIT_CHECK_COND1((XMAX <= 0.0), param, "xmax(%lg) <= 0",XMAX); // modify condition to your needs

	// insert your code here
	a1 = (FD2*FD3*N*pow(X23,FD2)*pow(XMAX,FD3)*
         (pow(X12,FD1) - pow(XMIN,FD1)))/
       (FD2*FD3*pow(X12,FD1)*pow(X23,FD2)*pow(XMAX,FD3) -
         (FD1*FD2*pow(X12,FD2)*pow(X23,FD3) +
            (-(FD1*(FD2 - FD3)*pow(X12,FD2)) +
               (-FD1 + FD2)*FD3*pow(X23,FD2))*pow(XMAX,FD3))*
          pow(XMIN,FD1));
    a2 = (FD1*FD3*N*(pow(X12,FD2) - pow(X23,FD2))*
         pow(XMAX,FD3)*pow(XMIN,FD1))/
       (-(FD2*FD3*pow(X12,FD1)*pow(X23,FD2)*pow(XMAX,FD3)) +
         (FD1*FD2*pow(X12,FD2)*pow(X23,FD3) +
            (-(FD1*(FD2 - FD3)*pow(X12,FD2)) +
               (-FD1 + FD2)*FD3*pow(X23,FD2))*pow(XMAX,FD3))*
          pow(XMIN,FD1));
    a3 = (FD1*FD2*N*pow(X12,FD2)*
         (pow(X23,FD3) - pow(XMAX,FD3))*pow(XMIN,FD1))/
       (-(FD2*FD3*pow(X12,FD1)*pow(X23,FD2)*pow(XMAX,FD3)) +
         (FD1*FD2*pow(X12,FD2)*pow(X23,FD3) +
            (-(FD1*(FD2 - FD3)*pow(X12,FD2)) +
               (-FD1 + FD2)*FD3*pow(X23,FD2))*pow(XMAX,FD3))*
          pow(XMIN,FD1));
	return fractalFF_single(q,a1,FD1,XMIN,X12,param)
         + fractalFF_single(q,a2,FD2,X12 ,X23,param)
         + fractalFF_single(q,a3,FD3,X23,XMAX,param);
}

scalar sasfit_ff_fractal_series3_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_fractal_series3_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

