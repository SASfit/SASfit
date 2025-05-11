/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables

scalar w_l_e_sphere(scalar y, sasfit_param *param) {
	scalar fsp,Rc;
	Rc = sasfit_sd_w_l_e__v(y, param, DISTRIBUTION_QUANTILE);
	fsp=sasfit_sphere_fc(Q,Rc);
	return N*pow(Rc,6-DIM)*gsl_pow_2(4./3.*M_PI* fsp);
}

scalar sasfit_ff_w_l_e_sphere(scalar q, sasfit_param * param)
{
    scalar ystart, yend;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((THETA < 0.0), param, "theta(%lg) < 0",THETA); // modify condition to your needs
	SASFIT_CHECK_COND1((ALPHA < 0.0), param, "alpha(%lg) < 0",ALPHA); // modify condition to your needs
	SASFIT_CHECK_COND1((BETA < 0.0), param, "beta(%lg) < 0",BETA); // modify condition to your needs

	// insert your code here

	Q = q;

	ystart = sasfit_invert_func_v(BL,&sasfit_sd_w_l_e__v,DISTRIBUTION_QUANTILE,0,1,param);
	yend   = sasfit_invert_func_v(BU,&sasfit_sd_n_l_c__v,DISTRIBUTION_QUANTILE,0,1,param);
	return sasfit_integrate(ystart,yend,&w_l_e_sphere,param);
}

scalar sasfit_ff_w_l_e_sphere_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0;
}

scalar sasfit_ff_w_l_e_sphere_v(scalar u, sasfit_param * param, int dist)
{
    scalar tmp,x,y,LQ, UQ, IQR, M,K;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here

	// case 0: quantile distribution function
	// case 1: not yet defined (mode?, median?, ....)
	switch (dist) {
	    case DISTRIBUTION_TYPE: return DISTRIBUTION_QUANTILE;
        case DISTRIBUTION_QUANTILE:
                y=u;
                if (y<=0) return 0;
                if (y>=1) y = 1-sasfit_eps_get_nriq();
                return -(1+THETA)/THETA-1/THETA*gsl_sf_lambert_Wm1(-(THETA+1)*exp(-((THETA+1)+pow(-log(1-y),1/ALPHA)/BETA)));
                break;
        case DISTRIBUTION_QUANTILE_DENS:
                y=u;
                if (y<=0) return 0;
                if (y>=1) return gsl_posinf();
                return -1/THETA*(pow(-log(1 - y),-1 + 1/ALPHA)*
                        gsl_sf_lambert_Wm1(-((1 + THETA)/
                        exp((BETA + BETA*THETA + pow(-log(1 - y),1/ALPHA))/BETA))) )/
                        (ALPHA*BETA*(-1 + y)*
                        (1 + gsl_sf_lambert_Wm1(-((1 + THETA)/
                        exp((BETA + BETA*THETA +
                        pow(-log(1 - y),
                        1/ALPHA))/BETA)))));
                break;
        case DISTRIBUTION_PROBABILITY:
                x = u;
                if (x<0) return 0.0;
                tmp = -BETA*log(1-Lindley_F_R(x,THETA));
                return N*ALPHA*BETA*THETA*THETA*(1+x)/(1+THETA+THETA*x)
                        *pow(tmp,ALPHA-1)*exp(-pow(tmp,ALPHA));
                break;
        case DISTRIBUTION_CUMULATIVE:
                x = u;
                if (x<0) return 0.0;
                tmp = -BETA*log(1-Lindley_F_R(x,THETA));
                return 1-exp(-pow(tmp,ALPHA));
                break;
        case DISTRIBUTION_MEDIAN:
                return sasfit_sd_w_l_e__v(0.5,param,DISTRIBUTION_QUANTILE);
                break;
        case DISTRIBUTION_SKEWNESS:
                LQ = sasfit_sd_w_l_e__v(0.25,param,DISTRIBUTION_QUANTILE);
                UQ = sasfit_sd_w_l_e__v(0.75,param,DISTRIBUTION_QUANTILE);
                M  = sasfit_sd_w_l_e__v(0.5,param,DISTRIBUTION_QUANTILE);
                IQR = UQ - LQ;
                return (LQ+UQ-2*M)/IQR;
                break;
        case DISTRIBUTION_EXCESS_KURTOSIS:
                LQ = sasfit_sd_w_l_e__v(0.25,param,DISTRIBUTION_QUANTILE);
                UQ = sasfit_sd_w_l_e__v(0.75,param,DISTRIBUTION_QUANTILE);
                K  = sasfit_sd_w_l_e__v(7./8.,param,DISTRIBUTION_QUANTILE);
                K  = K+sasfit_sd_w_l_e__v(3./8.,param,DISTRIBUTION_QUANTILE);
                K  = K-sasfit_sd_w_l_e__v(5./8.,param,DISTRIBUTION_QUANTILE);
                K  = K-sasfit_sd_w_l_e__v(1./8.,param,DISTRIBUTION_QUANTILE);
                IQR = UQ - LQ;
                return K/IQR;
                break;
        case DISTRIBUTION_MODE:
        case DISTRIBUTION_MEAN:
        case DISTRIBUTION_VARIANCE:
        default: sasfit_err("parameter distr=%d not defined",dist);
    }
}

