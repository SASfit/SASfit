/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables

scalar sasfit_sd_metalog_clipped(scalar x, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND2((BL == BU), param, "bl(%lg) == bu(%lf)",BL, BU); // modify condition to your needs

	// insert your code here
	if (x>=BU || x<=BL) return 0;
    if (gsl_finite(pow(x,-ALPHA))) {
        return sasfit_sd_metalog_clipped_v(x,param,DISTRIBUTION_PROBABILITY)*pow(x,-ALPHA);
    } else {
        return 0;
    }
}

scalar sasfit_sd_metalog_clipped_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sd_metalog_clipped_v(scalar u, sasfit_param * param, int dist)
{
    scalar Qy,qy,px,Fx,x,y,v, LQ, UQ, IQR, M,K,p0tmp;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid
		// insert your code here
		// case 0: quantile distribution function Q(y)=x
		// case 1: quantile density distribution function q(y)=dQ(y)/dy = 1/p(y)
		// case 2: probability distribution function p(x)
		// case 3: cumulative distribution function F(x)=y
		// case 4: mode
		// case 5: mean
		// case 5: variance
		// case 6: median
		// case 7: skewness
		// case 8: excess kurtosis

	switch (dist) {
	    case DISTRIBUTION_TYPE: return DISTRIBUTION_QUANTILE;
        case DISTRIBUTION_QUANTILE:
                y = u;
                Qy =  Mk(10, y, &param->p[4], 10, param);
                return Qy;
        case DISTRIBUTION_QUANTILE_DENS:
                y = u;
                qy =  mk(10, y, &param->p[4], 10, param);
                return qy;
        case DISTRIBUTION_PROBABILITY:
                x = u;
                p0tmp=N;
                N = 1;
                y = sasfit_invert_func_v(x,&sasfit_sd_metalog_clipped_v,DISTRIBUTION_QUANTILE,0,1, param);
                px = 1.0/sasfit_sd_metalog_clipped_v(y,param,DISTRIBUTION_QUANTILE_DENS);
                N = p0tmp;
                return N*px;
        case DISTRIBUTION_CUMULATIVE:
                x = u;
                p0tmp=N;
                N = 1;
                y = sasfit_invert_func_v(x,&sasfit_sd_metalog_clipped_v,DISTRIBUTION_QUANTILE,0,1, param);
                Fx = y;
                N = p0tmp;
                return N*Fx;

        case DISTRIBUTION_MEDIAN:
                return sasfit_sd_metalog_clipped_v(0.5,param,DISTRIBUTION_QUANTILE);
                break;
        case DISTRIBUTION_SKEWNESS:
                LQ = sasfit_sd_metalog_clipped_v(0.25,param,DISTRIBUTION_QUANTILE);
                UQ = sasfit_sd_metalog_clipped_v(0.75,param,DISTRIBUTION_QUANTILE);
                M  = sasfit_sd_metalog_clipped_v(0.5,param,DISTRIBUTION_QUANTILE);
                IQR = UQ - LQ;
                return (LQ+UQ-2*M)/IQR;
                break;
        case DISTRIBUTION_EXCESS_KURTOSIS:
                LQ = sasfit_sd_metalog_clipped_v(0.25,param,DISTRIBUTION_QUANTILE);
                UQ = sasfit_sd_metalog_clipped_v(0.75,param,DISTRIBUTION_QUANTILE);
                K  = sasfit_sd_metalog_clipped_v(7./8.,param,DISTRIBUTION_QUANTILE);
                K  = K+sasfit_sd_metalog_clipped_v(3./8.,param,DISTRIBUTION_QUANTILE);
                K  = K-sasfit_sd_metalog_clipped_v(5./8.,param,DISTRIBUTION_QUANTILE);
                K  = K-sasfit_sd_metalog_clipped_v(1./8.,param,DISTRIBUTION_QUANTILE);
                IQR = UQ - LQ;
                return K/IQR;
                break;
        case DISTRIBUTION_MODE:

        case DISTRIBUTION_MEAN:

        case DISTRIBUTION_VARIANCE:

        default: sasfit_err("parameter distr=%d not defined",dist);
    }
    return 0;
}

