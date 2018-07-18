/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define T	param->p[1]
#define H	param->p[2]
#define N	param->p[3]
#define ETA_C	param->p[4]
#define ETA_L	param->p[5]
#define SIGMA	fabs(param->p[6])
#define D	param->p[7]
#define Q   param->p[MAXPAR-1]
#define NN  param->p[MAXPAR-2]


scalar StackDiscs_core(scalar theta, sasfit_param * param)
{
	scalar SQ, tn, V_t, V_c, f_t, f_c;
	scalar bessarg, sinarg1, sinarg2, bess, tmp;
	int k;

    SASFIT_ASSERT_PTR(param);

	tn = NN;
	V_t = M_PI*R*R*(T+2*H);
	V_c = M_PI*R*R*T;

	bessarg = Q*R *sin(theta);
	sinarg1 = Q*(T/2.0+H)*cos(theta);
	sinarg2 = Q*T/2.0*cos(theta);

	SQ = 1.0;
	tmp = Q*cos(theta);
	for (k=1; k < tn ;k++)
	{
		SQ = SQ+2.0/tn * (tn-k)*cos(k*D*tmp) * exp(-k*gsl_pow_2(tmp*SIGMA)/2.0);
	}
	if (bessarg != 0.0)
	{
		bess=2.0*sasfit_bessj1(bessarg)/bessarg;
	} else
	{
		bess =1.0;
	}

	if (sinarg1 != 0.0)
	{
		f_t = sin(sinarg1)/sinarg1*bess;
	} else
	{
		f_t = bess;
	}

	if (sinarg2 != 0.0)
	{
		f_c = sin(sinarg2)/sinarg2*bess;
	} else
	{
		f_c = bess;
	}

	return    (ETA_L * (V_t*f_t - V_c*f_c) + ETA_C * V_c*f_c)
		* (ETA_L * (V_t*f_t - V_c*f_c) + ETA_C * V_c*f_c)
		* SQ*sin(theta);
}

scalar sasfit_ff_stackdiscs(scalar q, sasfit_param * param)
{
    scalar w,ftmp1,ftmp2;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((T < 0.0), param, "t(%lg) < 0",T); // modify condition to your needs
	SASFIT_CHECK_COND1((H < 0.0), param, "h(%lg) < 0",H); // modify condition to your needs
	SASFIT_CHECK_COND1((N < 1.0), param, "n(%lg) < 1",N); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA == 0.0), param, "sigma(%lg) == 0",SIGMA); // modify condition to your needs
	SASFIT_CHECK_COND1((D < 0.0), param, "D(%lg) < 0",D); // modify condition to your needs

	// insert your code here
	Q = q;
	NN = lround(floor(N));
	w = N-floor(N);
    NN=lround(floor(N))+1;
	ftmp1=sasfit_integrate(0.0, M_PI/2.0, StackDiscs_core, param);
	if (w>0) {
        ftmp2=sasfit_integrate(0.0, M_PI/2.0, StackDiscs_core, param);
	} else {
	    ftmp2=0;
	}
	return (1-w)*ftmp1+w*ftmp2;
}

scalar sasfit_ff_stackdiscs_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_stackdiscs_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

