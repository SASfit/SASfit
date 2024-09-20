/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include "float.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables

scalar sasfit_sd_fgld_(scalar x, sasfit_param * param)
{
    scalar y;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((KAPPA < 0.0), param, "kappa(%lg) < 0",KAPPA); // modify condition to your needs
	SASFIT_CHECK_COND1((BETA <= 0.0), param, "beta(%lg) <= 0",BETA); // modify condition to your needs
    SASFIT_CHECK_COND1((BU   <= 0.0), param, "bu(%lg) <= 0",BU);

	// insert your code here
	if (x<BL || x>BU) return 0;
	y = sasfit_sd_fgld__v(x,param,DISTRIBUTION_CUMULATIVE)/N;
	if (y<=0 || y>=1 ) return 0;
	return N/sasfit_sd_fgld__v(y,param,DISTRIBUTION_QUANTILE_DENS);
}

scalar sasfit_sd_fgld__f(scalar x, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid
	// insert your code here
	return sasfit_sd_fgld__v(x,param,DISTRIBUTION_CUMULATIVE);
}

scalar sasfit_sd_fgld__v(scalar u, sasfit_param * param, int dist)
{
    scalar Qfgld,qfgld,delta,x,y,LQ, UQ, IQR, M,K;
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
	    case DISTRIBUTION_TYPE: return DISTRIBUTION_QUANTILE;
        case DISTRIBUTION_QUANTILE:
                y=u;
                if (y<=0) y = DBL_MIN;
                if (y>=1) y = 1-sasfit_eps_get_nriq();
                delta = (atan(DELTA)+M_PI_2)/M_PI;
                Qfgld = ALPHA+BETA*((1-delta)*log(y) - delta*log(1-y) + KAPPA*y);
                if (gsl_isinf(Qfgld)==1 ) {
                    return gsl_posinf();
                } else if (gsl_isinf(Qfgld)==-1 ) {
                    return gsl_neginf();
                } else if (gsl_isnan(Qfgld) && y==0) {
                    return gsl_neginf();
                } else if (gsl_isnan(Qfgld) && y==1) {
                    return gsl_posinf();
                } else {
                    return Qfgld;
                }
                break;
        case DISTRIBUTION_QUANTILE_DENS:
                y=u;
                if (y<=0) return gsl_posinf();
                if (y>=1) return gsl_posinf();
                delta = (atan(DELTA)+M_PI_2)/M_PI;
                qfgld = BETA*((1-delta)/y + delta/(1-y) + KAPPA);
                if (gsl_isinf(qfgld)==1 ) {
                    return gsl_posinf();
                } else if (gsl_isinf(qfgld) == -1 ) {
                    return gsl_neginf();
                } else if (gsl_isnan(qfgld)) {
                    return gsl_posinf();
                } else {
                    return qfgld;
                }
                break;
        case DISTRIBUTION_PROBABILITY:
                x = u;
                y = sasfit_invert_func_v(x,&sasfit_sd_fgld__v,DISTRIBUTION_QUANTILE,0,1, param);
                if (y<=0) return 0;
                if (y>=1) return 0;
                return N/sasfit_sd_fgld__v(y,param,DISTRIBUTION_QUANTILE_DENS);
                break;
        case DISTRIBUTION_CUMULATIVE:
                x = u;
                return N*sasfit_invert_func_v(x,&sasfit_sd_fgld__v,DISTRIBUTION_QUANTILE,0,1, param);
                break;
        case DISTRIBUTION_MEAN:
                delta = (atan(DELTA)+M_PI_2)/M_PI;
                return ALPHA+BETA*(2*delta-1+KAPPA/2.);
                break;
        case DISTRIBUTION_VARIANCE:
                delta = (atan(DELTA)+M_PI_2)/M_PI;
                return gsl_pow_2(BETA)*(1-4*delta*(1-delta)
                                         +KAPPA/2.0*(1+KAPPA/6.0)
                                         +M_PI*M_PI/3.0*delta*(1-delta));
                break;
        case DISTRIBUTION_MEDIAN:
                return sasfit_sd_fgld__v(0.5,param,DISTRIBUTION_QUANTILE);
                break;
        case DISTRIBUTION_SKEWNESS:
                delta = (atan(DELTA)+M_PI_2)/M_PI;
                return (2*delta-1)*log(4./3.)/(log(3)+KAPPA/2.);
                break;
        case DISTRIBUTION_EXCESS_KURTOSIS:
                return (log(21./5.)+KAPPA/2)/(log(3)+KAPPA/2.);
                break;
        case DISTRIBUTION_MODE:
        default: sasfit_err("parameter distr=%d not defined",dist);
    }

}

