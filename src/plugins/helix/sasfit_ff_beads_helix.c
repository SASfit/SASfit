/*
 * Author(s) of this file:
 *  Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *  10. Oct. 2017
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define H	param->p[1]
#define D	param->p[2]
#define L   100
#define N   30

#define RHO_S 0
#define RHO_H 1

#define nS 5

scalar hphi(scalar q, sasfit_param * param) {
	scalar V, u, F;
	u = q*R;
	if (fabs(u)<1e-6) {
		F = 1-0.1*u*u+1./280.*gsl_pow_4(u);
	} else {
		F = (sin(u)-u*cos(u))/gsl_pow_3(u);
	}
	V = 3./4. * M_PI *gsl_pow_3(R);
	return 3 * V * (RHO_H-RHO_S) * F;
}

scalar hPsi2(scalar q, int j, sasfit_param * param) {
	scalar a;
	a = 2*M_PI*j/(H*q);
	if (1.0<a) return 0.0;
	return gsl_pow_2(gsl_sf_bessel_Jn(j,q*D*0.5*sqrt(1-a*a))*N/(2*M_PI)*hphi(q,param));
}

scalar sasfit_ff_beads_helix(scalar q, sasfit_param * param)
{
	scalar sum, prefactor;
	int i;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((H < 0.0), param, "H(%lg) < 0",H); // modify condition to your needs
	SASFIT_CHECK_COND1((D < 0.0), param, "D(%lg) < 0",D); // modify condition to your needs

	// insert your code here
	prefactor = N*L/(H*H*q);
	sum = prefactor*hPsi2(q,0,param);
	for (i=1;i<=nS;i++) {
		sum = sum + 2*prefactor*hPsi2(q,i,param);
	}
	return sum;
}

scalar sasfit_ff_beads_helix_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_beads_helix_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

