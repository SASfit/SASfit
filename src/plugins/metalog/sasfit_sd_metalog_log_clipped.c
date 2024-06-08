/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables

scalar sasfit_sd_metalog_log_clipped(scalar x, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND2((BL == BU), param, "bl(%lg) == bu(%lf)",BL, BU); // modify condition to your needs

	// insert your code here
	if (x<=BL||x>=BU) return 0;
	if (gsl_finite(pow(x,-ALPHA))) {
        return sasfit_invert_func_v(x,&sasfit_sd_metalog_log_clipped_v,DISTRIBUTION_PROBABILITY,0,1, param)*pow(x,-ALPHA);
//        return N*metalogLogPDF(x, param)*pow(x,-ALPHA);
    } else {
        return 0;
    }
}

scalar sasfit_sd_metalog_log_clipped_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sd_metalog_log_clipped_v(scalar u, sasfit_param * param, int dist)
{
    scalar Qy,qy,px,Fx,x,y,v;
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
        case DISTRIBUTION_QUANTILE:
                y = u;
                Qy =  MLog(10, y, &param->p[4], 10, BL, param);
                return Qy;
        case DISTRIBUTION_QUANTILE_DENS: y = u;
                qy =  mLog(10, y, &param->p[4], 10, param);
                return qy;
        case DISTRIBUTION_PROBABILITY:
                x = u;
                y = sasfit_invert_func_v(x,&sasfit_sd_metalog_log_clipped_v,DISTRIBUTION_QUANTILE,0,1, param);
                px = 1.0/sasfit_sd_metalog_log_clipped_v(u,param,DISTRIBUTION_QUANTILE_DENS);
                return N*px;
        case DISTRIBUTION_CUMULATIVE:
                x = u;
                y = sasfit_invert_func_v(x,&sasfit_sd_metalog_log_clipped_v,DISTRIBUTION_QUANTILE,0,1, param);
                Fx = y;
                return N*Fx;
        case DISTRIBUTION_MODE:

        case DISTRIBUTION_MEAN:

        case DISTRIBUTION_VARIANCE:

        case DISTRIBUTION_MEDIAN:

        case DISTRIBUTION_SKEWNESS:

        case DISTRIBUTION_EXCESS_KURTOSIS:

        default: sasfit_err("parameter distr=%d not defined",dist);
    }
    return 0;
}

