/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables
#define N	param->p[0]
#define BL	GSL_MIN(param->p[1],param->p[2])
#define BU	GSL_MAX(param->p[1],param->p[2])
#define ALPHA	param->p[3]
#define P	fabs(param->p[4])
#define Q	fabs(param->p[5])


scalar sasfit_sd_std_beta(scalar x, sasfit_param * param)
{
    /*
     *   THIS IS THE PROBABILITY DISTRIBUTION FUNCTION
     */
    int ierr,powx;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((x < 0.0), param, "x(%lg) < 0",x);
	SASFIT_CHECK_COND1((BL < 0.0), param, "bl(%lg) < 0",BL); // modify condition to your needs
	SASFIT_CHECK_COND1((BU <= 0.0), param, "bu(%lg) <= 0",BU); // modify condition to your needs
	SASFIT_CHECK_COND1((P <= 0.0), param, "p(%lg) <= 0",P); // modify condition to your needs
	SASFIT_CHECK_COND1((Q <= 0.0), param, "q(%lg) <= 0",Q); // modify condition to your needs
    powx = (x==0) ? 0 : pow(x,ALPHA);
    return N*powx*sasfit_sd_std_beta_v(x,param,DISTRIBUTION_PROBABILITY);
}

scalar sasfit_sd_std_beta_f(scalar x, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return N*sasfit_sd_std_beta_v(x,param,DISTRIBUTION_CUMULATIVE);
}

scalar sasfit_sd_std_beta_v(scalar u, sasfit_param * param, int dist)
{
    scalar x,y,z,za,zb, LQ, UQ, IQR, M,K;
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
        case DISTRIBUTION_PROBABILITY:
            x=u;
	    	if ((x<=BL) || (x>=BU)) return 0;
            z = (x-BL)/(BU-BL);
            za = pow(z,P-1.);
            zb = pow(1.-z,Q-1.);
            return za*zb/(BU-BL)/gsl_sf_beta(P,Q);
            break;
        case DISTRIBUTION_CUMULATIVE:
            if (x<=BL) return 0;
            if (x>=BU) return 1;
            z = (x-BL)/(BU-BL);
            return gsl_sf_beta_inc(P,Q,z);
            break;
        case DISTRIBUTION_VARIANCE:
            return P*Q/(gsl_pow_2(P+Q)*(P+Q+1))*gsl_pow_2(BU-BL);
        case DISTRIBUTION_MODE:
            SASFIT_CHECK_COND2(P<1 || Q<1,param,"p(%lg) || q(%lg) < 1",P,Q);
            return BL+(BU-BL)*(P-1)/(P+Q-2);
            break;
        case DISTRIBUTION_QUANTILE:
            y=u;
         case DISTRIBUTION_MEDIAN:
            return sasfit_sd_std_beta_v(0.5,param,DISTRIBUTION_QUANTILE);
            break;
        case DISTRIBUTION_SKEWNESS:
            LQ = sasfit_sd_std_beta_v(0.25,param,DISTRIBUTION_QUANTILE);
            UQ = sasfit_sd_std_beta_v(0.75,param,DISTRIBUTION_QUANTILE);
            M  = sasfit_sd_std_beta_v(0.5,param,DISTRIBUTION_QUANTILE);
            IQR = UQ - LQ;
            return (LQ+UQ-2*M)/IQR;
            break;
        case DISTRIBUTION_EXCESS_KURTOSIS:
            LQ = sasfit_sd_std_beta_v(0.25,param,DISTRIBUTION_QUANTILE);
            UQ = sasfit_sd_std_beta_v(0.75,param,DISTRIBUTION_QUANTILE);
            K  = sasfit_sd_std_beta_v(7./8.,param,DISTRIBUTION_QUANTILE);
            K  = K+sasfit_sd_std_beta_v(3./8.,param,DISTRIBUTION_QUANTILE);
            K  = K-sasfit_sd_std_beta_v(5./8.,param,DISTRIBUTION_QUANTILE);
            K  = K-sasfit_sd_std_beta_v(1./8.,param,DISTRIBUTION_QUANTILE);
            IQR = UQ - LQ;
            return K/IQR;
            break;
        case DISTRIBUTION_QUANTILE_DENS:
            y=u;
        case DISTRIBUTION_MEAN:
        default: sasfit_err("parameter distr=%d not defined",dist);
    }
}

