/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R_C	param->p[0]
#define T_SH	param->p[1]
#define T_SOL	param->p[2]
#define ETA_SHELL	param->p[3]
#define ETA_SOL	param->p[4]
#define N	param->p[5]

scalar sasfit_ff_multilamellar_vesicle(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R_C < 0.0), param, "R_c(%lg) < 0",R_C); // modify condition to your needs
	SASFIT_CHECK_COND1((T_SH < 0.0), param, "t_sh(%lg) < 0",T_SH); // modify condition to your needs
	SASFIT_CHECK_COND1((T_SOL < 0.0), param, "t_sol(%lg) < 0",T_SOL); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA_SHELL < 0.0), param, "eta_shell(%lg) < 0",ETA_SHELL); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA_SOL < 0.0), param, "eta_sol(%lg) < 0",ETA_SOL); // modify condition to your needs
	SASFIT_CHECK_COND1((N < 0.0), param, "n(%lg) < 0",N); // modify condition to your needs

	// insert your code here

	scalar sum,w,sumw;
	int i, nlayers;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R_C < 0.0), param, "R_C(%lg) < 0",R_C);
	SASFIT_CHECK_COND1((T_SOL < 0.0), param, "T_SOL(%lg) < 0",T_SOL);
	SASFIT_CHECK_COND1((T_SH < 0.0), param, "T_SH(%lg) < 0",T_SH);
	SASFIT_CHECK_COND1((N < 1.0), param, "N(%lg) < 1",N);

	sasfit_init_param( &subParam );

	nlayers = (int) N;
	w = N - nlayers;
	sum = 0.;
	for (i=0; i < nlayers ;i++)
	{
		subParam.p[0] = R_C + i*T_SH + i*T_SOL;
		subParam.p[3] = -(ETA_SHELL-ETA_SOL);
		sum += sasfit_ff_sphere_f(q, &subParam);

		subParam.p[0] = R_C + (i+1)*T_SH + i*T_SOL;
		subParam.p[3] = ETA_SHELL-ETA_SOL;
	 	sum += sasfit_ff_sphere_f(q, &subParam);
	}
	sumw = sum;

	subParam.p[0] = R_C + nlayers*T_SH + nlayers*T_SOL;
	subParam.p[3] = -(ETA_SHELL-ETA_SOL);
	sumw += w*sasfit_ff_sphere_f(q, &subParam);

	subParam.p[0] = R_C + (nlayers+1)*T_SH + nlayers*T_SOL;
	subParam.p[3] = ETA_SHELL-ETA_SOL;
	sumw += w*sasfit_ff_sphere_f(q, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	SASFIT_CHECK_SUB_ERR(param, subParam);

	return sum*sum*(1-w)+sumw*sumw*w;
}

scalar sasfit_ff_multilamellar_vesicle_f(scalar q, sasfit_param * param)
{
	scalar sum,w;
	int i, nlayers;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	sasfit_init_param( &subParam );

	nlayers = (int) N;
	w = N - nlayers;
	sum = 0.;
	for (i=0; i < nlayers ;i++)
	{
		subParam.p[0] = R_C + i*T_SH + i*T_SOL;
		subParam.p[3] = -(ETA_SHELL-ETA_SOL);
		sum += sasfit_ff_sphere_f(q, &subParam);

		subParam.p[0] = R_C + (i+1)*T_SH + i*T_SOL;
		subParam.p[3] = ETA_SHELL-ETA_SOL;
		sum += sasfit_ff_sphere_f(q, &subParam);
	}
	subParam.p[0] = R_C + nlayers*T_SH + nlayers*T_SOL;
	subParam.p[3] = -(ETA_SHELL-ETA_SOL);
	sum += w*sasfit_ff_sphere_f(q, &subParam);

	subParam.p[0] = R_C + (nlayers+1)*T_SH + nlayers*T_SOL;
	subParam.p[3] = ETA_SHELL-ETA_SOL;
	sum += w*sasfit_ff_sphere_f(q, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	return sum;
}

scalar sasfit_ff_multilamellar_vesicle_v(scalar x, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	switch (dist) {
	    case 0: return  x  + (N+1)*T_SH + N*T_SOL;
	    case 1: return  R_C+ (N+1)*   x + N*T_SOL;
	    case 2: return  R_C+ (N+1)*T_SH + N*x;
	    default: return R_C+ (N+1)*T_SH + N*T_SOL;
	}
}

