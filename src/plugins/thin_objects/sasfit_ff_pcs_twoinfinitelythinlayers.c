/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define T	param->p[0]
#define SIGMA_T	fabs(param->p[1])

#define Q	param->p[MAXPAR-1]


scalar thincentrosymmetriclayers_core(scalar x, sasfit_param *param)
{
	scalar Pcs, LNdistr;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);


	Pcs = gsl_pow_int(cos(Q*x/2),lround(P));
    if (SIGMA_T == 0.0) return Pcs;
	sasfit_init_param( &subParam );
	subParam.p[0] = 1.0;
	subParam.p[1] = SIGMA_T;
	subParam.p[2] = 1.0;
	subParam.p[3] = T;

	LNdistr = sasfit_sd_LogNorm(x, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	return LNdistr*Pcs;
}


scalar sasfit_ff_pcs_twoinfinitelythinlayers(scalar q, sasfit_param * param)
{
    scalar Pcs, tstart, tend;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((T < 0.0), param, "t(%lg) < 0",T); // modify condition to your needs

	Q = q;
    P = 2;

	// insert your code here
    if (SIGMA_T == 0.0) {
        Pcs = thincentrosymmetriclayers_core(T,param);
    } else {
        find_LogNorm_int_range(2,T,SIGMA_T,&tstart,&tend,param);
        Pcs 	= sasfit_integrate(tstart, tend, &thincentrosymmetriclayers_core, param);
    }

	return Pcs;
}

scalar sasfit_ff_pcs_twoinfinitelythinlayers_f(scalar q, sasfit_param * param)
{
    scalar Pcs, tstart, tend;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	Q = q;
    P = 1;
    if (SIGMA_T == 0.0) {
        Pcs = thincentrosymmetriclayers_core(T,param);
    } else {
        find_LogNorm_int_range(2,T,SIGMA_T,&tstart,&tend,param);
        Pcs 	= sasfit_integrate(tstart, tend, &thincentrosymmetriclayers_core, param);
    }

	return Pcs;
}

scalar sasfit_ff_pcs_twoinfinitelythinlayers_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

