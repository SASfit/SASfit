/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables

scalar sasfit_ff_n_l_c_profile(scalar x, sasfit_param * param)
{
    scalar tmp;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((x < 0.0), param, "x(%lg) < 0",x);
	SASFIT_CHECK_COND1((THETA < 0.0), param, "theta(%lg) < 0",THETA); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA < 0.0), param, "sigma(%lg) < 0",SIGMA); // modify condition to your needs

	// insert your code here

    if (x<BL || x>BU) return 0;
    return sasfit_sd_n_l_c__v(x,param,DISTRIBUTION_PROBABILITY);
}

scalar sasfit_ff_n_l_c_profile_f(scalar x, sasfit_param * param)
{
    scalar tmp;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here

}

scalar sasfit_ff_n_l_c_profile_v(scalar u, sasfit_param * param, int dist)
{
    scalar QN;
    scalar tmp,x,y,LQ, UQ, IQR, M,K;
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
                if (y<=0) return 0;
                if (y>=1) y = 1-sasfit_eps_get_nriq();
                QN = MU+SIGMA*sqrt(2)*sasfit_erfinv(2*y-1);
                return KWm1(0.5-atan( QN )/M_PI,THETA);
                break;
        case DISTRIBUTION_PROBABILITY:
                x = u;
                tmp=(tan(M_PI*(Lindley_F_R(x,THETA)-0.5))-MU)/SIGMA;
                return N*sqrt(M_PI_2)*THETA*THETA/(SIGMA*(1+THETA))*(1+x)
                        *exp(-THETA*x)/gsl_pow_2(sin(M_PI*(1-Lindley_F_R(x,THETA))))
                        *exp(-gsl_pow_2(tmp/2));
                break;
        case DISTRIBUTION_CUMULATIVE:
                x = u;
                tmp=(tan(M_PI*(Lindley_F_R(x,THETA)-0.5))-MU)/SIGMA;
                return 0.5*(1+gsl_sf_erf(tmp/sqrt(2)));
                break;
        case DISTRIBUTION_MEDIAN:
                return sasfit_sd_n_l_c__v(0.5,param,DISTRIBUTION_QUANTILE);
                break;
        case DISTRIBUTION_SKEWNESS:
                LQ = sasfit_sd_n_l_c__v(0.25,param,DISTRIBUTION_QUANTILE);
                UQ = sasfit_sd_n_l_c__v(0.75,param,DISTRIBUTION_QUANTILE);
                M  = sasfit_sd_n_l_c__v(0.5,param,DISTRIBUTION_QUANTILE);
                IQR = UQ - LQ;
                return (LQ+UQ-2*M)/IQR;
                break;
        case DISTRIBUTION_EXCESS_KURTOSIS:
                LQ = sasfit_sd_n_l_c__v(0.25,param,DISTRIBUTION_QUANTILE);
                UQ = sasfit_sd_n_l_c__v(0.75,param,DISTRIBUTION_QUANTILE);
                K  = sasfit_sd_n_l_c__v(7./8.,param,DISTRIBUTION_QUANTILE);
                K  = K+sasfit_sd_n_l_c__v(3./8.,param,DISTRIBUTION_QUANTILE);
                K  = K-sasfit_sd_n_l_c__v(5./8.,param,DISTRIBUTION_QUANTILE);
                K  = K-sasfit_sd_n_l_c__v(1./8.,param,DISTRIBUTION_QUANTILE);
                IQR = UQ - LQ;
                return K/IQR;
                break;
        case DISTRIBUTION_QUANTILE_DENS:
        case DISTRIBUTION_MEAN:
        case DISTRIBUTION_MODE:
        case DISTRIBUTION_VARIANCE:
        default: sasfit_err("parameter distr=%d not defined",dist);
    }
	switch (dist) {
        case 0:
        default: sasfit_err("parameter distr=%d not defined",dist);
    }
}

