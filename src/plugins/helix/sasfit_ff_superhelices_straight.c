/*
 * Author(s) of this file:
 *  Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *  10. Oct. 2017
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R1	param->p[0]
// #define DUMMY	param->p[1]
// #define DUMMY	param->p[2]
// #define DUMMY	param->p[3]
// #define DUMMY	param->p[4]
#define P	param->p[5]
#define H	param->p[6]
// #define DUMMY	param->p[7]
// #define DUMMY	param->p[8]
// #define DUMMY	param->p[9]
#define Q	param->p[MAXPAR-1]
#define L	param->p[MAXPAR-2]
#define A	param->p[MAXPAR-3]
#define GAM1 param->p[MAXPAR-4]
#define GAM2 param->p[MAXPAR-5]
#define ALPHA	param->p[MAXPAR-6]

scalar Kernel_w(scalar w, sasfit_param *param) {
	scalar Psi;
	Psi = sqrt(2.*R1*R1*(1.-cos(w/sqrt(R1*R1+A*A)))+A*A*w*w/(R1*R1+A*A));
//	sasfit_out("a:%lg\t P:%lg\t R:%lg\t H:%lg\t L:%lg\t q:%lg\t\n",A,P,R1,H,L,Q);
	return 2. * (L-w)*sinc(Q*Psi);
}

scalar sasfit_ff_superhelices_straight(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R1 < 0.0), param, "r1(%lg) < 0",R1); // modify condition to your needs
	SASFIT_CHECK_COND1((H < 0.0), param, "H(%lg) < 0",H); // modify condition to your needs
	SASFIT_CHECK_COND1((P < 0.0), param, "P(%lg) < 0",P); // modify condition to your needs

	// insert your code here
	Q=q;
	A = P/(2.*M_PI);
	L=H/A*sqrt(R1*R1+A*A);

	return sasfit_integrate(0,L,&Kernel_w,param);
}

scalar sasfit_ff_superhelices_straight_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_superhelices_straight_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	A = P/(2.*M_PI);
	return H/A*sqrt(R1*R1+A*A);
}

