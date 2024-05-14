/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define A	param->p[0]
#define Q	param->p[1]
#define LAMBDA	param->p[2]
#define KAPPA	param->p[3]
#define SHIFT	param->p[4]
#define Y0	param->p[5]

scalar sasfit_ff_q_weibull__ampl_(scalar x, sasfit_param * param)
{
    scalar z;
    // https://en.wikipedia.org/wiki/Q-Weibull_distribution
    /*
     *   THIS IS THE PROBABILITY DISTRIBUTION FUNCTION
     */
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((Q >= 2.0), param, "q(%lg) >= 2",Q); // modify condition to your needs
	SASFIT_CHECK_COND1((LAMBDA <= 0.0), param, "lambda(%lg) <= 0",LAMBDA); // modify condition to your needs
	SASFIT_CHECK_COND1((KAPPA <= 0.0), param, "kappa(%lg) <= 0",KAPPA); // modify condition to your needs

	// insert your code here
	z=x-SHIFT;
	if (z<0 || A==0) return Y0;
	return A*(sasfit_ff_q_weibull__area_(z,param)-Y0)/(sasfit_ff_q_weibull__area_(z,param)-Y0)+Y0;
}

scalar sasfit_ff_q_weibull__ampl__f(scalar x, sasfit_param * param)
{
    /*
     *   THIS IS THE CUMULATIVE DISTRIBUTION FUNCTION
     */
    scalar z, qp, lp;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	z=x-SHIFT;
	qp=1./(2-Q);
	lp=LAMBDA/pow(2-Q,1/KAPPA);
	return A*(1-eq(qp,-pow(z/lp,KAPPA)));
}

scalar sasfit_ff_q_weibull__ampl__v(scalar p, sasfit_param * param, int dist)
{
    /*
     *   THIS IS THE QUANTILE DISTRIBUTION FUNCTION
     */
    scalar qp, lp;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	qp=1./(2-Q);
	lp=LAMBDA/pow(2-Q,1/KAPPA);
	return lp*pow(-lnq(qp,1-p),1./KAPPA)+SHIFT;
}

