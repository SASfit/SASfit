/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define T	param->p[0]
#define SIGMA_T	param->p[1]
#define DUMMY1	param->p[2]
#define DUMMY2	param->p[3]
#define ETA_L	param->p[4]
#define ETA_SOL	param->p[5]

#define Q	param->p[MAXPAR-1]

scalar homoXS_core(scalar x, sasfit_param *param) 
{
	scalar Pcs, u, LNdistr;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);


	u = Q*x*0.5;
	if (u  == 0.0) 
	{
		Pcs = gsl_pow_2((ETA_L-ETA_SOL)*x);
	} else {
		Pcs = gsl_pow_2((ETA_L-ETA_SOL)*x*sin(u)/u);
	}

	sasfit_init_param( &subParam );
	subParam.p[0] = 1.0;
	subParam.p[1] = SIGMA_T;
	subParam.p[2] = 1.0;
	subParam.p[3] = T;

	LNdistr = sasfit_sd_LogNorm(x, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	return LNdistr*Pcs;
}


scalar homogeneousXS(scalar q, sasfit_param * param)
{
	scalar Pcs, u;
	scalar tstart = 0.0, tend = 0.0; 

	SASFIT_ASSERT_PTR(param);

	Q = q;

	if (SIGMA_T == 0.0) {
		u = q*T*0.5;
		if (u  == 0.0) {
			Pcs = gsl_pow_2(T);
		} else {
			Pcs = gsl_pow_2(T*sin(u)/u);
		}
	} else {
		find_LogNorm_int_range(2,T,SIGMA_T,&tstart,&tend,param);
		Pcs 	= sasfit_integrate(tstart, tend, &homoXS_core, param);
	}
	return Pcs;
}

scalar sasfit_ff_pcs_homogeneousplate(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((T < 0.0), param, "t(%lg) < 0",T); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA_T < 0.0), param, "sigma_t(%lg) < 0",SIGMA_T); // modify condition to your needs

	// insert your code here
	return homogeneousXS(q,param)*gsl_pow_2(ETA_L-ETA_SOL);
}

scalar sasfit_ff_pcs_homogeneousplate_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_pcs_homogeneousplate_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

