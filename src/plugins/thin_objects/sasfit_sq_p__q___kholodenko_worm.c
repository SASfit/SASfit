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

