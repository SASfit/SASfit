/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables

scalar sasfit_sq_Pprime_KholodenkoWorm_core(scalar x, sasfit_param *param)
{
	scalar E, F, fy;
	scalar maxExp;

	SASFIT_ASSERT_PTR(param);
    maxExp = 250;
    E = 1.0;
    F = 1.0;

    if (x == 0.0) return 1.0;

    if (Q == 3.0/LB)
	{
        if (fabs(x)<maxExp) {
            fy = x/sinh(x);;
        } else {
            fy=log(fabs(x))-gsl_sf_lnsinh(fabs(x));
            fy=exp(fy);
        }
    }
	else  if (Q < 3.0/LB)
	{
        E = sqrt(1.0-gsl_pow_2(Q*LB/3.0));
        if (fabs(x)<maxExp) {
            fy = sinh(E*x)/(E*sinh(x));
        } else {
            fy=gsl_sf_lnsinh(fabs(E*x))-gsl_sf_lnsinh(fabs(x));
            fy=exp(fy)/E;
        }
    } else {
        F = sqrt(gsl_pow_2(Q*LB/3.0)-1.0);
        if (fabs(x)<maxExp) {
            fy = sin(F*x)/(F*sinh(x));
        } else {
            fy=gsl_sf_lnsinh(fabs(x));
            fy=sin(F*x)*exp(-fy)/F;
        }
    }

    if (N == 0.0) return 1.0;
    return 2.0/(3.0*N) * (1.0-x/(3.0*N)) * fy;
}

scalar sasfit_sq_Pprime_KholodenkoWorm_OOURA(scalar x, void * pam)
{
	sasfit_param *param;
	param = (sasfit_param *) pam;
	return sasfit_sq_Pprime_KholodenkoWorm_core(x,param);
}

int sasfit_sq_Pprime_KholodenkoWorm_cubature(unsigned ndim, const double *x, void *pam,
      unsigned fdim, double *fval)
{
	sasfit_param *param;
	param = (sasfit_param *) pam;
    fval[0] = 0;
	if ((ndim < 1) || (fdim < 1)) {
		sasfit_out("false dimensions fdim:%d ndim:%d\n",fdim,ndim);
		return 1;
	}
	fval[0] = sasfit_sq_Pprime_KholodenkoWorm_core(x[0],param);
	return 0;
}
scalar sasfit_sq_p__q___kholodenko_worm(scalar q, sasfit_param * param)
{
    scalar *aw, res,err,sum;
    int intstrategy, lenaw=4000;
    scalar cubxmin[1], cubxmax[1], fval[1], ferr[1];

	SASFIT_ASSERT_PTR(param);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((LB <= 0.0), param, "lb(%lg) <= 0",LB); // modify condition to your needs
	SASFIT_CHECK_COND1((L < 0.0), param, "L(%lg) < 0",L); // modify condition to your needs

	N = L/LB;
	if (N == 0.0)
	{
		return L*L;
	}

	Q = q;
	cubxmin[0]=0;
	cubxmax[0]=3.0*N;;

    if (N > 1) {
        if (q > 10.*M_PI/LB) {
            return L*L*M_PI/(L*q);
        }
    } else {
        if (q > 10.*M_PI/L) {
            return L*L*M_PI/(L*q);
        }
    }
    intstrategy = sasfit_get_int_strategy();
	intstrategy=H_CUBATURE;
	switch(intstrategy) {
    case H_CUBATURE: {
			hcubature(1, &sasfit_sq_Pprime_KholodenkoWorm_cubature,param,1, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED,
				fval, ferr);
			sum = L*L*fval[0];
            break;
            }
    case P_CUBATURE: {
			pcubature(1, &sasfit_sq_Pprime_KholodenkoWorm_cubature,param,1, cubxmin, cubxmax,
				100000, 0.0, sasfit_eps_get_nriq(), ERROR_PAIRED,
				fval, ferr);
			sum = L*L*fval[0];
            break;
            }
    case OOURA_DOUBLE_EXP_QUADRATURE: {
 //       sasfit_out("this is the OOURA_DOUBLE_EXP_QUADRATURE integration routine\n");
            aw = (scalar *)malloc((lenaw)*sizeof(scalar));
            sasfit_intdeini(lenaw, GSL_DBL_MIN, sasfit_eps_get_nriq(), aw);
            sasfit_intde(&sasfit_sq_Pprime_KholodenkoWorm_OOURA, 0.0,3.0*N, aw, &res, &err, param);
			sum=L*L*res;
            free(aw);
            break;
            }
    case OOURA_CLENSHAW_CURTIS_QUADRATURE: {
//        sasfit_out("this is the OOURA_CLENSHAW_CURTIS_QUADRATURE integration routine\n");
            aw = (scalar *)malloc((lenaw+1)*sizeof(scalar));
            sasfit_intccini(lenaw, aw);
            sasfit_intcc(&sasfit_sq_Pprime_KholodenkoWorm_OOURA, 0.0,3.0*N, sasfit_eps_get_nriq(), lenaw, aw, &res, &err,param);
			sum=L*L*res;
            free(aw);
            break;
            }
    default: {
//		    sasfit_out("this is the default sasfit_integrate routine\n");
            if (N > 1) {
                if (q > 10.*M_PI/LB) {
                    sum=L*L*M_PI/(L*q);
                } else {
                    sum=L*L*sasfit_integrate(0.0, 3.0*N, sasfit_sq_Pprime_KholodenkoWorm_core,param);
                }
            } else {
                if (q > 10.*M_PI/L) {
                    sum=L*L*M_PI/(L*q);
                } else {
                    sum=L*L*sasfit_integrate(0.0, 3.0*N, sasfit_sq_Pprime_KholodenkoWorm_core,param);
                }
            }
            break;
            }
    }
//    sasfit_out("q:%lf\t I(q):%lf\n",q,sum);
    return sum;
}


scalar sasfit_sq_p__q___kholodenko_worm_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_p__q___kholodenko_worm_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

