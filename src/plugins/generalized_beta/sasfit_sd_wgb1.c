/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables
#define N	param->p[0]
#define A	param->p[1]
#define B	fabs(param->p[2])
#define K	param->p[3]
#define P	fabs(param->p[4])
#define Q	fabs(param->p[5])

scalar sasfit_sd_wgb1(scalar x, sasfit_param * param)
{
    /*
     *   THIS IS THE PROBABILITY DISTRIBUTION FUNCTION
     */
    int ierr;
    gsl_sf_result Bpq;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((B == 0.0), param, "b(%lg) == 0",B); // modify condition to your needs
	SASFIT_CHECK_COND1((P+K/A <= 0.0), param, "ap+k<=0, k(%lg)",K); // modify condition to your needs
	SASFIT_CHECK_COND1((P == 0.0), param, "p(%lg) == 0",P); // modify condition to your needs
	SASFIT_CHECK_COND1((Q == 0.0), param, "q(%lg) == 0",Q); // modify condition to your needs

	// insert your code here
	if (x<=0) return 0.0;
	if (x>=B) return 0.0;
	gsl_set_error_handler_off();
	ierr = gsl_sf_beta_e(P+K/A,Q,&Bpq);
	if (ierr) {
        sasfit_err("%s B(%lf,%lf)+-%lf\n", gsl_strerror (ierr),P,Q,Bpq.err);
        return 0;
	}
	return N*fabs(A)*pow(x,A*P+K-1)*pow(1-pow(x/B,A),Q-1)/(pow(B,A*P+K)*Bpq.val);
}

scalar sasfit_sd_wgb1_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sd_wgb1_v(scalar u, sasfit_param * param, int dist)
{
    scalar x,y;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
		// case 0: quantile distribution function Q(y)=x
		// case 1: quantile density distribution function q(y)=dQ(y)/dy = 1/p(y)
		// case 2: probability distribution function p(x)
		// case 3: cumulative distribution function F(x)=y
		// case 4: mode
		// case 5: mean
		// case 6: variance
		// case 7: median
		// case 8: skewness
		// case 9: excess kurtosis

	switch (dist) {
	    case DISTRIBUTION_TYPE: return DISTRIBUTION_PROBABILITY;
        case DISTRIBUTION_QUANTILE:
                y=u;
        case DISTRIBUTION_QUANTILE_DENS:
                y=u;
        case DISTRIBUTION_PROBABILITY:
        case DISTRIBUTION_CUMULATIVE:
        case DISTRIBUTION_MEAN:
        case DISTRIBUTION_VARIANCE:
        case DISTRIBUTION_MEDIAN:
        case DISTRIBUTION_SKEWNESS:
        case DISTRIBUTION_EXCESS_KURTOSIS:
        case DISTRIBUTION_MODE:
        default: sasfit_err("parameter distr=%d not defined",dist);
    }
}

