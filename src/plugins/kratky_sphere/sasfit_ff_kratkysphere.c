/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define DELTA_Q_PERP	param->p[1]
#define ETA	param->p[2]
#define R0	param->p[3]
#define XI	param->p[4]
#define D	param->p[5]
#define SCALE param->p[6]
#define Q	param->p[MAXPAR-1]

scalar FFPhiKratky(scalar qperp, sasfit_param * param) {
	scalar P,P16, q;
	q = sqrt(qperp*qperp+Q*Q);
	if (q*R == 0) {
		P = gsl_pow_2(ETA*4.*M_PI/3.*gsl_pow_3(R));
	} else {
		P= gsl_pow_2(ETA*4.*M_PI/3.*gsl_pow_3(R)* 3.*(sin(q*R)-q*R*cos(q*R))/gsl_pow_3(q*R));
	}
	if ((XI == 0) || (R0 == 0)) {
		P16=1.0;
	} else {
	    P16 = 1.0/pow(q*R0,D);
	    P16 = P16*D*gsl_sf_gamma(D-1.0)/pow(1.0+1.0/(q*q*XI*XI),(D-1.0)/2.0);
	    P16 = P16*sin((D-1.0)*atan(q*XI));
	    P16 = P16*SCALE;
	    P16 = (1.0+P16);
	}
	return P*P16;
}

scalar sasfit_ff_kratkysphere(scalar q, sasfit_param * param)
{

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((DELTA_Q_PERP < 0.0), param, "Delta_q_perp(%lg) < 0",DELTA_Q_PERP); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA < 0.0), param, "eta(%lg) < 0",ETA); // modify condition to your needs
	SASFIT_CHECK_COND1((D <= 1.0), param, "D(%lg) <= 1",D); // modify condition to your needs

	// insert your code here
	Q=q;

	return sasfit_integrate(0,DELTA_Q_PERP,FFPhiKratky,param)/DELTA_Q_PERP;
}

scalar sasfit_ff_kratkysphere_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_kratkysphere_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

