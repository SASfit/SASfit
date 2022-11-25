/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define SIGMA_R	param->p[1]
#define DUMMY1	param->p[2]
#define DUMMY2	param->p[3]
#define DUMMY3	param->p[4]
#define DUMMY4	param->p[5]
#define DUMMY5	param->p[6]
#define ETA_CORE	param->p[7]
#define DUMMY6	param->p[8]
#define ETA_SOL		param->p[9]

#define Q	param->p[MAXPAR-1]

scalar homo_cyl_Ampl_core(scalar x, sasfit_param *param)
{
	scalar A,u;
	u = Q*x;
	return M_PI*x*x*(ETA_CORE-ETA_SOL)*2*sasfit_jinc(u);
}

scalar homo_cyl_I_core(scalar x, sasfit_param *param)
{
	return gsl_pow_2(homo_cyl_Ampl_core(x,param));
}

scalar Ampl_homoXS_core(scalar x, sasfit_param *param)
{
	scalar Pcs, u, LNdistr;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	Pcs = homo_cyl_Ampl_core(x,param);

	sasfit_init_param( &subParam );
	subParam.p[0] = 1.0;
	subParam.p[1] = SIGMA_R;
	subParam.p[2] = 1.0;
	subParam.p[3] = R;

	LNdistr = sasfit_sd_LogNorm(x, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	return LNdistr*Pcs;
}

scalar I_homoXS_core(scalar x, sasfit_param *param)
{
	scalar Pcs, u, LNdistr;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	Pcs = homo_cyl_I_core(x,param);

	sasfit_init_param( &subParam );
	subParam.p[0] = 1.0;
	subParam.p[1] = SIGMA_R;
	subParam.p[2] = 1.0;
	subParam.p[3] = R;

	LNdistr = sasfit_sd_LogNorm(x, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	return LNdistr*Pcs;
}

scalar sasfit_ff_pcs_homogeneouscyl(scalar q, sasfit_param * param)
{
	scalar Pcs, Rstart, Rend;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "r(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA_R < 0.0), param, "sigma_t(%lg) < 0",SIGMA_R); // modify condition to your needs

	// insert your code here

	Q = q;

	if (SIGMA_R == 0.0) {
		Pcs=homo_cyl_I_core(R,param);
	} else {
		find_LogNorm_int_range(2,R,SIGMA_R,&Rstart,&Rend,param);
		Pcs 	= sasfit_integrate(Rstart, Rend, &I_homoXS_core, param);
	}
	return Pcs;
}

scalar sasfit_ff_pcs_homogeneouscyl_f(scalar q, sasfit_param * param)
{
	scalar Pcs, Rstart, Rend;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here

	Q = q;

	if (SIGMA_R == 0.0) {
		Pcs=homo_cyl_Ampl_core(R,param);
	} else {
		find_LogNorm_int_range(2,R,SIGMA_R,&Rstart,&Rend,param);
		Pcs 	= sasfit_integrate(Rstart, Rend, &Ampl_homoXS_core, param);
	}
	return Pcs;
}

scalar sasfit_ff_pcs_homogeneouscyl_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

