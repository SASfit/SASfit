/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables
#define N_1	param->p[0]
#define S_1	param->p[1]
#define P_1	param->p[2]
#define MU_1	param->p[3]
#define N_2	param->p[4]
#define S_2	param->p[5]
#define P_2	param->p[6]
#define MU_2	param->p[7]
#define X0	param->p[8]

scalar sasfit_sd_bilognorm(scalar x, sasfit_param * param)
{
	scalar LNf;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((S_1 < 0.0), param, "s_1(%lg) < 0",S_1); // modify condition to your needs
	SASFIT_CHECK_COND1((P_1 < 0.0), param, "p_1(%lg) < 0",P_1); // modify condition to your needs
	SASFIT_CHECK_COND1((MU_1 < 0.0), param, "mu_1(%lg) < 0",MU_1); // modify condition to your needs
	SASFIT_CHECK_COND1((S_2 < 0.0), param, "s_2(%lg) < 0",S_2); // modify condition to your needs
	SASFIT_CHECK_COND1((P_2 < 0.0), param, "p_2(%lg) < 0",P_2); // modify condition to your needs
	SASFIT_CHECK_COND1((MU_2 < 0.0), param, "mu_2(%lg) < 0",MU_2); // modify condition to your needs

	// insert your code here
    if (x<=0) return 0;

    sasfit_init_param( &subParam );
	subParam.p[0] = param->p[0]*param->p[4];	// N2
	subParam.p[1] = param->p[5];	// s2
	subParam.p[2] = param->p[6];	// p2
	subParam.p[3] = param->p[7];	// m2

	SASFIT_CHECK_SUB_ERR(param, subParam);
	LNf = sasfit_sd_LogNorm(x-X0, &subParam)+ sasfit_sd_LogNorm(x-X0, param);
	return LNf;
}

scalar sasfit_sd_bilognorm_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sd_bilognorm_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

