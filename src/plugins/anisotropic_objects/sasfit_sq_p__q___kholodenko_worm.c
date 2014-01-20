/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define LB	param->p[0]
#define L	param->p[1]

#define	N	param->p[MAXPAR-1]
#define	Q	param->p[MAXPAR-2]

scalar sasfit_sq_Pprime_KholodenkoWorm_core(scalar x, sasfit_param * param)
{
	scalar E, F, fy;

	SASFIT_ASSERT_PTR(param);

        E = 1.0;
        F = 1.0;

        if (x == 0.0) return 1.0;

        if (Q == 3.0/LB) 
	{
                fy = x/sinh(x);
        } 
	else  if (Q < 3.0/LB) 
	{
                E = sqrt(1.0-pow(Q*LB/3.0, 2.0));
                fy = sinh(E*x)/(E*sinh(x));
        } else {
                F = sqrt(pow(Q*LB/3.0, 2.0)-1.0);
                fy = sin(F*x)/(F*sinh(x));
        }

        if (N == 0.0) return 1.0;
        return 2.0/(3.0*N) * (1.0-x/(3.0*N)) * fy;
}


scalar sasfit_sq_p__q___kholodenko_worm(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((LB < 0.0), param, "lb(%lg) < 0",LB); // modify condition to your needs
	SASFIT_CHECK_COND1((L < 0.0), param, "L(%lg) < 0",L); // modify condition to your needs

	N = L/LB;
	if (N == 0.0)
	{
		return 1.0;
	}

	Q = q;
//	return L*L*sasfit_integrate_ctm(0.0, 3.0*N, sasfit_sq_Pprime_KholodenkoWorm_core, param, 10000, intabseps, intreleps);
	if (N > 1) {
		if (q > 10.*M_PI/LB) {
			return L*L*M_PI/(L*q);
		} else {
			return L*L*sasfit_integrate(0.0, 3.0*N, sasfit_sq_Pprime_KholodenkoWorm_core,param);
		}
	} else {
		if (q > 10.*M_PI/L) {
			return L*L*M_PI/(L*q);
		} else {
			return L*L*sasfit_integrate(0.0, 3.0*N, sasfit_sq_Pprime_KholodenkoWorm_core,param);
		}
	}
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

