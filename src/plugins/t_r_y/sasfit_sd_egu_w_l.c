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

	SASFIT_CHECK_COND1((fabs(LAMBDA) <= 0.0), param, "|lambda|(%lg) <= 0",fabs(LAMBDA)); // modify condition to your needs
	SASFIT_CHECK_COND1((fabs(ALPHA) <= 0.0), param, "|alpha|(%lg) <= 0",fabs(ALPHA)); // modify condition to your needs
	SASFIT_CHECK_COND1((fabs(BETA) <= 0.0), param, "|beta|(%lg) <= 0",fabs(BETA)); // modify condition to your needs
	SASFIT_CHECK_COND1((fabs(GAMMA) <= 0.0), param, "|c|(%lg) <= 0",fabs(GAMMA)); // modify condition to your needs
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
    scalar Qegu_w_l,qegu_w_l,delta,x,y,Fegu_w_l,z, t,LQ, UQ, IQR, M,K;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((fabs(LAMBDA) <= 0.0), param, "|lambda|(%lg) <= 0",fabs(LAMBDA)); // modify condition to your needs
	SASFIT_CHECK_COND1((fabs(ALPHA) <= 0.0), param, "|alpha|(%lg) <= 0",fabs(ALPHA)); // modify condition to your needs
	SASFIT_CHECK_COND1((fabs(BETA) <= 0.0), param, "|beta|(%lg) <= 0",fabs(BETA)); // modify condition to your needs
	SASFIT_CHECK_COND1((fabs(GAMMA) <= 0.0), param, "|c|(%lg) <= 0",fabs(GAMMA)); // modify condition to your needs
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
                Qegu_w_l = fabs(LAMBDA) * pow(log(pow(-exp(-KAPPA/fabs(GAMMA))*log(1-pow(1-y,1/fabs(BETA))),-fabs(GAMMA))+1),1./fabs(ALPHA));
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
                return (exp(KAPPA)*fabs(GAMMA)*fabs(LAMBDA)*pow(1 - y,-1 + 1/fabs(BETA))*
                        pow(log(1 + exp(KAPPA)/
                        pow(-log(1 - pow(1 - y,1/fabs(BETA))),fabs(GAMMA))),-1 + 1/fabs(ALPHA)))/
                        (fabs(ALPHA)*fabs(BETA)*(-1 + pow(1 - y,1/fabs(BETA)))*
                        (exp(KAPPA) + pow(-log(1 - pow(1 - y,1/fabs(BETA))),
                        fabs(GAMMA)))*log(1 - pow(1 - y,1/fabs(BETA))));
                break;
        case DISTRIBUTION_PROBABILITY:
                x = u;
                if (x<=0) return 0;
                return N*(fabs(ALPHA)*fabs(BETA)*exp(-(exp(KAPPA/fabs(GAMMA))/
                        pow(-1 + exp(pow(x/fabs(LAMBDA),fabs(ALPHA))),1/fabs(GAMMA))) +
                        KAPPA/fabs(GAMMA) + pow(x/fabs(LAMBDA),fabs(ALPHA)))*
                        pow(1 - exp(-(exp(KAPPA/fabs(GAMMA))/
                        pow(-1 + exp(pow(x/fabs(LAMBDA),fabs(ALPHA))),1/fabs(GAMMA)))),-1 + fabs(BETA))
                        *pow(x/fabs(LAMBDA),fabs(ALPHA)))/
                        (pow(-1 + exp(pow(x/fabs(LAMBDA),fabs(ALPHA))),(1 + fabs(GAMMA))/fabs(GAMMA))*fabs(GAMMA)*x);
                break;
        case DISTRIBUTION_CUMULATIVE:
                x = u;
                if (x<=0) return 0;
                z= pow(x/fabs(LAMBDA),fabs(ALPHA));
                t = exp(-exp(KAPPA/fabs(GAMMA))*pow(expm1(z),-1/fabs(GAMMA)));
                Fegu_w_l = 1-pow(1 - t,fabs(BETA));
                if (gsl_isnan(Fegu_w_l)) {
                    if (x<sasfit_sd_egu_w_l_v(0.5,param,DISTRIBUTION_QUANTILE)) {
                        return 0;
                    } else {
                        return 1;
                    }
                } else {
                    return Fegu_w_l;
                }
                break;
        case DISTRIBUTION_MEDIAN:
                return sasfit_sd_fgld__v(0.5,param,DISTRIBUTION_QUANTILE);
                break;
        case DISTRIBUTION_SKEWNESS:
                LQ = sasfit_sd_fgld__v(0.25,param,DISTRIBUTION_QUANTILE);
                UQ = sasfit_sd_fgld__v(0.75,param,DISTRIBUTION_QUANTILE);
                M  = sasfit_sd_fgld__v(0.5,param,DISTRIBUTION_QUANTILE);
                IQR = UQ - LQ;
                return (LQ+UQ-2*M)/IQR;
                break;
        case DISTRIBUTION_EXCESS_KURTOSIS:
                LQ = sasfit_sd_fgld__v(0.25,param,DISTRIBUTION_QUANTILE);
                UQ = sasfit_sd_fgld__v(0.75,param,DISTRIBUTION_QUANTILE);
                K  = sasfit_sd_fgld__v(7./8.,param,DISTRIBUTION_QUANTILE);
                K  = K+sasfit_sd_fgld__v(3./8.,param,DISTRIBUTION_QUANTILE);
                K  = K-sasfit_sd_fgld__v(5./8.,param,DISTRIBUTION_QUANTILE);
                K  = K-sasfit_sd_fgld__v(1./8.,param,DISTRIBUTION_QUANTILE);
                IQR = UQ - LQ;
                return K/IQR;
                break;
        case DISTRIBUTION_MEAN:
        case DISTRIBUTION_MODE:
        case DISTRIBUTION_VARIANCE:
        default: sasfit_err("parameter distr=%d not defined",dist);
    }

}

