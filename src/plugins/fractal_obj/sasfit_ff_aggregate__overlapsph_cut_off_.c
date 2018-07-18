/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define RG	param->p[0]
#define D	param->p[1]
#define I0	param->p[2]
#define	Q	param->p[MAXPAR-1]
#define	XI	param->p[MAXPAR-2]

scalar MassFractOverlappingSpheres_core(scalar x, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param);

	return 4.0/3.0*M_PI * XI*XI*XI * (1.0+x/(4.0*XI)) * pow(1.0-x/(2.0*XI), 2.0) * pow(x, D-3.0) * x*x * sin(Q*x)/(Q*x);
}

scalar sasfit_ff_aggregate__overlapsph_cut_off_(scalar q, sasfit_param * param)
{
    scalar res, norm, C;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((RG < 0.0), param, "Rg(%lg) < 0",RG); // modify condition to your needs
	SASFIT_CHECK_COND1((D < 0.0), param, "D(%lg) < 0",D); // modify condition to your needs

	// insert your code here


	XI = sqrt((D+2)*(D+5)/(2*D*(D+1))*RG*RG);

	if (q*XI > 50.) {
		Q = 50./XI;
		res = sasfit_integrate(0.0, 2.0*XI, MassFractOverlappingSpheres_core, param);
		C = res*pow(Q*XI,D);
		res = C*pow(q*XI,-D);
	} else {
		Q = q;
		res = sasfit_integrate(0.0, 2.0*XI, MassFractOverlappingSpheres_core, param);
	}

	norm = pow(2.0, 2.0+D) * M_PI*pow(XI, 3.0+D)/(D*(3.0 + 4.0*D + D*D));

	return I0 * res/norm;
}

scalar sasfit_ff_aggregate__overlapsph_cut_off__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_aggregate__overlapsph_cut_off__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

