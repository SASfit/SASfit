/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include "float.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables

scalar sasfit_sd_egu_w_l(scalar x, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((LAMBDA <= 0.0), param, "lambda(%lg) <= 0",LAMBDA); // modify condition to your needs
	SASFIT_CHECK_COND1((ALPHA <= 0.0), param, "alpha(%lg) <= 0",ALPHA); // modify condition to your needs
	SASFIT_CHECK_COND1((BETA <= 0.0), param, "beta(%lg) <= 0",BETA); // modify condition to your needs
	SASFIT_CHECK_COND1((GAMMA <= 0.0), param, "c(%lg) <= 0",GAMMA); // modify condition to your needs
    SASFIT_CHECK_COND1((BU   <= 0.0), param, "bu(%lg) <= 0",BU);

	// insert your code here

	return   sasfit_sd_egu_w_l_v(x,param,DISTRIBUTION_PROBABILITY);
}

scalar sasfit_sd_egu_w_l_f(scalar x, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid
	// insert your code here
	return sasfit_sd_egu_w_l_v(x,param,DISTRIBUTION_CUMULATIVE);
}

scalar sasfit_sd_egu_w_l_v(scalar u, sasfit_param * param, int dist)
{
    scalar Qegu_w_l,qegu_w_l,delta,x,y,Fegu_w_l,z, t;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

    SASFIT_CHECK_COND1((LAMBDA <= 0.0), param, "lambda(%lg) <= 0",LAMBDA); // modify condition to your needs
	SASFIT_CHECK_COND1((ALPHA <= 0.0), param, "alpha(%lg) <= 0",ALPHA); // modify condition to your needs
	SASFIT_CHECK_COND1((BETA <= 0.0), param, "beta(%lg) <= 0",BETA); // modify condition to your needs
	SASFIT_CHECK_COND1((GAMMA <= 0.0), param, "c(%lg) <= 0",GAMMA); // modify condition to your needs
    SASFIT_CHECK_COND1((BU   <= 0.0), param, "bu(%lg) <= 0",BU);

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

	// case 1: not yet defined (mode?, median?, ....)
	switch (dist) {
	    case DISTRIBUTION_TYPE: return DISTRIBUTION_QUANTILE;
        case DISTRIBUTION_QUANTILE:
                y=u;
                if (y<=0) return 0;
                if (y>=1) y = 1-sasfit_eps_get_nriq();
                Qegu_w_l = LAMBDA * pow(log(pow(-exp(-KAPPA/GAMMA)*log(1-pow(1-y,1/BETA)),-GAMMA)+1),1./ALPHA);
                if (gsl_isnan(Qegu_w_l)) {
                    return gsl_posinf();
                } else {
                    return Qegu_w_l;
                }
                break;
        case DISTRIBUTION_QUANTILE_DENS:
                y=u;
                if (y<=0) return 0;
                if (y>=1) return gsl_posinf();
                return (exp(KAPPA)*GAMMA*LAMBDA*pow(1 - y,-1 + 1/BETA)*
                        pow(log(1 + exp(KAPPA)/
                        pow(-log(1 - pow(1 - y,1/BETA)),GAMMA)),-1 + 1/ALPHA))/
                        (ALPHA*BETA*(-1 + pow(1 - y,1/BETA))*
                        (exp(KAPPA) + pow(-log(1 - pow(1 - y,1/BETA)),
                        GAMMA))*log(1 - pow(1 - y,1/BETA)));
                break;
        case DISTRIBUTION_PROBABILITY:
                x = u;
                if (x<=0) return 0;
                return (ALPHA*BETA*exp(-(exp(KAPPA/GAMMA)/
                        pow(-1 + exp(pow(x/LAMBDA,ALPHA)),1/GAMMA)) +
                        KAPPA/GAMMA + pow(x/LAMBDA,ALPHA))*
                        pow(1 - exp(-(exp(KAPPA/GAMMA)/
                        pow(-1 + exp(pow(x/LAMBDA,ALPHA)),1/GAMMA))),-1 + BETA)
                        *pow(x/LAMBDA,ALPHA))/
                        (pow(-1 + exp(pow(x/LAMBDA,ALPHA)),(1 + GAMMA)/GAMMA)*GAMMA*x);
                break;
        case DISTRIBUTION_CUMULATIVE:
                x = u;
                if (x<=0) return 0;
                z= pow(x/LAMBDA,ALPHA);
                t = exp(-exp(KAPPA/GAMMA)*pow(expm1(z),-1/GAMMA));
                Fegu_w_l = 1-pow(1 - t,BETA);
                if (gsl_isnan(Fegu_w_l)) {
                    if (x<sasfit_sd_egu_w_l_v(0.1,param,DISTRIBUTION_QUANTILE)) {
                        return 0;
                    } else {
                        return 1;
                    }
                } else {
                    return Fegu_w_l;
                }
                break;
        case DISTRIBUTION_MODE:
        case DISTRIBUTION_MEAN:
        case DISTRIBUTION_VARIANCE:
        case DISTRIBUTION_SKEWNESS:
        case DISTRIBUTION_MEDIAN:
        case DISTRIBUTION_EXCESS_KURTOSIS:
        default: sasfit_err("parameter distr=%d not defined",dist);
    }

}

