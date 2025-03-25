/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables

scalar sasfit_sd_p_guwelop(scalar x, sasfit_param * param)
{
    scalar pGUWELOP;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	if (x<BL || x>BU) return 0;
	return sasfit_sd_p_guwelop_v(x,param,DISTRIBUTION_PROBABILITY);
}

scalar sasfit_sd_p_guwelop_f(scalar x, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return sasfit_sd_p_guwelop_v(x,param,DISTRIBUTION_CUMULATIVE);
}

scalar sasfit_sd_p_guwelop_v(scalar u, sasfit_param * param, int dist)
{
    scalar x, y, QGUWELOP,FGUWELOP,qGUWELOP,pGUWELOP, LQ, UQ, IQR, M,K;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((LAMBDA <= 0.0), param, "lambda(%lg) <= 0",LAMBDA); // modify condition to your needs
	SASFIT_CHECK_COND1((ALPHA <= 0.0), param, "alpha(%lg) <= 0",ALPHA); // modify condition to your needs
	SASFIT_CHECK_COND1((BETA <= 0.0), param, "beta(%lg) <= 0",BETA); // modify condition to your needs
	SASFIT_CHECK_COND1((GAMMA <= 0.0), param, "gamma(%lg) <= 0",GAMMA); // modify condition to your needs



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
                QGUWELOP = LAMBDA*pow(log(1 + pow(-(log(-((-THETA +
                            log((1 - exp(THETA))*(-1 + 1/(1 - exp(THETA)) + y)))/THETA))/
                    BETA),-GAMMA)),1/ALPHA);
                if (gsl_isnan(QGUWELOP)) {
                    return gsl_posinf();
                } else {
                    return QGUWELOP;
                }
                break;
        case DISTRIBUTION_QUANTILE_DENS:
                y=u;
                if (y==0) return GSL_POSINF;
                qGUWELOP = ((-1 + exp(THETA))*GAMMA*LAMBDA*
                            pow(log(1 + pow(BETA,GAMMA)/
                        pow(-log(1 - log(exp(THETA) + y - exp(THETA)*y)/THETA),GAMMA)),
                        -1 + 1/ALPHA))/
                        (ALPHA*(exp(THETA)*(-1 + y) - y)*
                        (THETA - log(exp(THETA) + y - exp(THETA)*y))*
                        log(1 - log(exp(THETA) + y - exp(THETA)*y)/THETA)*
                        (1 + pow(-(log(1 - log(exp(THETA) + y - exp(THETA)*y)/THETA)/BETA),
                        GAMMA)));
                return qGUWELOP;
                break;
        case DISTRIBUTION_PROBABILITY:
                x=u;
                if (x<=0) return 0;
                pGUWELOP=(ALPHA*BETA*exp(-(BETA/pow(expm1(pow(x/LAMBDA,ALPHA)),1/GAMMA)) +
                        THETA - THETA*exp(-BETA/pow(expm1(pow(x/LAMBDA,ALPHA)),1/GAMMA)) +
                        pow(x/LAMBDA,ALPHA))*THETA*pow(x/LAMBDA,ALPHA))/
                        ((expm1(THETA))*pow(expm1(pow(x/LAMBDA,ALPHA)),
                        (1 + GAMMA)/GAMMA)*GAMMA*x);
                return N*pGUWELOP;
                break;
        case DISTRIBUTION_CUMULATIVE:
                x=u;
                if (x<=0) return 0;
                FGUWELOP = 1 - (expm1((-expm1(-(BETA/pow(expm1(pow(x/LAMBDA,ALPHA)),1/GAMMA))))*THETA))/(expm1(THETA));
                if (gsl_isnan(FGUWELOP)) {
                    return 1;
                } else {
                    return FGUWELOP;
                }
                break;
        case DISTRIBUTION_MEDIAN:
                return sasfit_sd_p_guwelop_v(0.5,param,DISTRIBUTION_QUANTILE);
                break;
        case DISTRIBUTION_SKEWNESS:
                LQ = sasfit_sd_p_guwelop_v(0.25,param,DISTRIBUTION_QUANTILE);
                UQ = sasfit_sd_p_guwelop_v(0.75,param,DISTRIBUTION_QUANTILE);
                M  = sasfit_sd_p_guwelop_v(0.5,param,DISTRIBUTION_QUANTILE);
                IQR = UQ - LQ;
                return (LQ+UQ-2*M)/IQR;
                break;
        case DISTRIBUTION_EXCESS_KURTOSIS:
                LQ = sasfit_sd_p_guwelop_v(0.25,param,DISTRIBUTION_QUANTILE);
                UQ = sasfit_sd_p_guwelop_v(0.75,param,DISTRIBUTION_QUANTILE);
                K  = sasfit_sd_p_guwelop_v(7./8.,param,DISTRIBUTION_QUANTILE);
                K  = K+sasfit_sd_p_guwelop_v(3./8.,param,DISTRIBUTION_QUANTILE);
                K  = K-sasfit_sd_p_guwelop_v(5./8.,param,DISTRIBUTION_QUANTILE);
                K  = K-sasfit_sd_p_guwelop_v(1./8.,param,DISTRIBUTION_QUANTILE);
                IQR = UQ - LQ;
                return K/IQR;
                break;
        case DISTRIBUTION_MEAN:
        case DISTRIBUTION_MODE:
        case DISTRIBUTION_VARIANCE:
        default: sasfit_err("parameter distr=%d not defined",dist);
    }

    return 0;
}

