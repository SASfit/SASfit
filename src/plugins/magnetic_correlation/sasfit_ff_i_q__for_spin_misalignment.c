/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define K	param->p[0]
#define HI	param->p[1]
#define L_H	param->p[2]
#define R   fabs(param->p[3])
#define Q   param->p[MAXPAR-2]
#define lR  param->p[MAXPAR-1]

scalar Iq_kernel(scalar r, sasfit_param * param) {
    if (fabs(Q*r)<=1e-3) {
        return 4*M_PI*r*r
                *(1-gsl_pow_2(Q*r)/6.0+gsl_pow_4(Q*r)/120.0-gsl_pow_6(Q*r)/5040.0)
                *sasfit_ff_c_r__for_spin_misalignment(r,param);
    } else {
        return 4*M_PI*r*r
                *sin(Q*r)/(Q*r)
                *sasfit_ff_c_r__for_spin_misalignment(r,param);
    }
}
scalar sasfit_ff_i_q__for_spin_misalignment(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((K < 0.0), param, "K(%lg) < 0",K); // modify condition to your needs
	SASFIT_CHECK_COND1((HI < 0.0), param, "Hi(%lg) < 0",HI); // modify condition to your needs
//	SASFIT_CHECK_COND1((L_H < 0.0), param, "l_H(%lg) < 0",L_H); // modify condition to your needs

	// insert your code here
	if (R==0) return 0;
//	if (R>0) {
	    if (fabs(q*R)< 1e-3) {
	        return K*gsl_pow_6(R)/gsl_pow_2(HI)*
                (2*M_PI*M_PI)*
                (1./9.-gsl_pow_2(q*R)/45.+gsl_pow_4(q*R)/525.-4*gsl_pow_6(q*R)/42525.)
                /gsl_pow_2(1.0+gsl_pow_2(L_H*q));
	    } else {
            return K*gsl_pow_6(R)/gsl_pow_2(HI)*
                (2*M_PI*M_PI)*
                1.0/gsl_pow_2(q*R)*
                gsl_pow_2(gsl_sf_bessel_j1(q*fabs(R)))
                /gsl_pow_2(1.0+gsl_pow_2(L_H*q));
	    }
//	} else {
//       Q = q;
        // GSL_POSINF
//        R = fabs(R);
//        return sasfit_integrate(0,GSL_POSINF,&Iq_kernel, param);
//	}
}

scalar sasfit_ff_i_q__for_spin_misalignment_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_i_q__for_spin_misalignment_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

