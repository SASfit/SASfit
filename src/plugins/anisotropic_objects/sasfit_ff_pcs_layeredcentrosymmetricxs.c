/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define LC	param->p[0]
#define SIGMA_LC	fabs(param->p[1])
#define LSH	param->p[2]
#define SIGMA_LSH	fabs(param->p[3])
#define ETA_C	param->p[4]
#define ETA_SH	param->p[5]
#define ETA_SOL	param->p[6]

#define SH	param->p[MAXPAR-3]
#define LL	param->p[MAXPAR-2]
#define Q	param->p[MAXPAR-1]


scalar layeredcentrosymmetric_FF(sasfit_param *param)
{
	scalar Fc, Fsh, Pcs, LNdistr, u, v;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	u = LL*Q/2.0;
	v = (LL+2*SH)*Q/2.0;

	if (v == 0) {
		Fc = (ETA_C-ETA_SOL)*(LL+2.0*SH);
	} else {
		Fc = (ETA_C-ETA_SOL)*(LL+2.0*SH)*sin(v)/v;
	}

	if (u == 0) {
		Fsh = (ETA_C-ETA_SH)*LL;
	} else {
		Fsh = (ETA_C-ETA_SH)*LL*sin(u)/u;
	}

	Pcs = gsl_pow_2(Fc-Fsh);

	return Pcs;

}
scalar layeredcentrosymmetric_core(scalar x, sasfit_param *param)
{
	scalar Fc, Fsh, Pcs, LNdistr, u, v;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	SH = x;


	Pcs = layeredcentrosymmetric_FF(param);

	sasfit_init_param( &subParam );
	subParam.p[0] = 1.0;
	subParam.p[1] = SIGMA_LSH;
	subParam.p[2] = 1.0;
	subParam.p[3] = LSH;

	LNdistr = sasfit_sd_LogNorm(x, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	return LNdistr*Pcs;
}


scalar layeredcentrosymmetric_SH(scalar x, sasfit_param * param)
{
	scalar SHstart = 0.0, SHend = 0.0;
	scalar Pcs;

	scalar LNdistr;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	if (SIGMA_LC == 0) {
		LL=LC;
		LNdistr = 1;
	} else {
		LL = x;
		sasfit_init_param( &subParam );
		subParam.p[0] = 1.0;
		subParam.p[1] = SIGMA_LC;
		subParam.p[2] = 1.0;
		subParam.p[3] = LC;
		LNdistr = sasfit_sd_LogNorm(x, &subParam);
		SASFIT_CHECK_SUB_ERR(param, subParam);
	}

	if (SIGMA_LSH == 0.0) {
		SH = LSH;
		Pcs = layeredcentrosymmetric_FF(param);
	} else {
		find_LogNorm_int_range(2,LSH,SIGMA_LSH,&SHstart,&SHend,param);
		Pcs = sasfit_integrate(SHstart, SHend, &layeredcentrosymmetric_core, param);
	}


	return LNdistr*Pcs;
	return Pcs;
}

scalar layeredcentrosymmetric_L(scalar q, sasfit_param * param)
{
	scalar Lstart = 0.0, Lend = 0.0;
	scalar Pcs;

	SASFIT_ASSERT_PTR(param);

	Q = q;

	if (SIGMA_LC == 0.0) {
		LL = LC;
		Pcs = layeredcentrosymmetric_SH(q,param);
	} else {
		find_LogNorm_int_range(2,LC,SIGMA_LC,&Lstart,&Lend,param);
		Pcs = sasfit_integrate(Lstart, Lend, &layeredcentrosymmetric_SH, param);
	}

	return Pcs;
}
scalar sasfit_ff_pcs_layeredcentrosymmetricxs(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((LC < 0.0), param, "t(%lg) < 0",LC); // modify condition to your needs
	SASFIT_CHECK_COND1((LSH < 0.0), param, "t(%lg) < 0",LSH); // modify condition to your needs

	// insert your code here
	return layeredcentrosymmetric_L(q,param);
}

scalar sasfit_ff_pcs_layeredcentrosymmetricxs_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_pcs_layeredcentrosymmetricxs_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

