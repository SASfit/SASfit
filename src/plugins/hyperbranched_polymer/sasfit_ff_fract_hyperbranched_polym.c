/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

 // doi: 10.1021/ma049950l
 // Burchard, Walther
 // Angular Dependence of Scattered Light from Hyperbranched Structures in a Good Solvent. A Fractal Approach
 // 2004-04
 // Macromolecules , Vol. 37, No. 10
 // American Chemical Society (ACS)
 // p. 3841-3849

#include "include/private.h"
#include <sasfit_error_ff.h>
#include <math.h>


// define shortcuts for local parameters/variables
#define RG	param->p[0]
#define D	param->p[1]
#define C	param->p[2]
#define I0	param->p[3]

scalar P_linfract(scalar u, sasfit_param * param) {
    if (u<1e-5) return 1-(D*D+D)/6.*u*u;
    if ((D-1)<1e-5) return atan(u)/(2*u)*(2-(D-1)*log(1+u*u));
    return sin((D - 1)*atan(u))/((D - 1)*u*pow(1 + u*u,(D - 1.)/2.));
}

scalar P_DBfract(scalar u, sasfit_param * param) {
    return gsl_pow_2(P_linfract(u,param));
}

scalar sasfit_ff_fract_hyperbranched_polym(scalar q, sasfit_param * param)
{
    scalar u, xib2, xil2;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((RG <= 0.0), param, "Rg(%lg) <= 0",RG); // modify condition to your needs
	SASFIT_CHECK_COND1((D < 1.0), param, "d(%lg) < 1",D); // modify condition to your needs
	SASFIT_CHECK_COND1((C < 0.0), param, "C(%lg) < 0",C); // modify condition to your needs
	SASFIT_CHECK_COND1((C > 1.0), param, "C(%lg) > 1",C); // modify condition to your needs
	SASFIT_CHECK_COND1((I0 < 0.0), param, "I0(%lg) < 0",I0); // modify condition to your needs

	// insert your code here
	xib2 = (C + 1)/(D*(D + 1))*RG*RG;
	xil2 = 2*C/(D*(D + 1))*RG*RG;
	u = q*RG;
	return P_DBfract(sqrt(xib2*q), param)/P_linfract(sqrt(xil2*q), param);
}

scalar sasfit_ff_fract_hyperbranched_polym_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_fract_hyperbranched_polym_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

