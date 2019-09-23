/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables

#define R	param->p[0]
#define SIGMA_R	fabs(param->p[1])
#define DUMMY2	param->p[2]
#define ALPHA	param->p[3]
#define DUMMY4	param->p[4]
#define DUMMY5	param->p[5]
#define DUMMY6	param->p[6]
#define DETA	param->p[7]

#define Q	param->p[MAXPAR-1]
#define P	param->p[MAXPAR-2]

scalar boucher_cyl(scalar x, sasfit_param *param) {
    return gsl_pow_int(DETA*2*M_PI*x*x*gsl_sf_hyperg_0F1(ALPHA*0.5,-0.25*gsl_pow_2(Q*x))/(ALPHA-2),lround(P));
}

scalar boucher_cyl_core(scalar x, sasfit_param *param) {
	scalar Pcs, u, LNdistr;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	Pcs = boucher_cyl(x,param);

	sasfit_init_param( &subParam );
	subParam.p[0] = 1.0;
	subParam.p[1] = SIGMA_R;
	subParam.p[2] = 1.0;
	subParam.p[3] = R;

	LNdistr = sasfit_sd_LogNorm(x, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	return LNdistr*Pcs;
}

scalar sasfit_ff_boucher_cyl(scalar q, sasfit_param * param)
{
    scalar Pcs, Rstart, Rend;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((ALPHA <= 2.0), param, "alpha(%lg) <= 2",ALPHA); // modify condition to your needs

	// insert your code here
	Q=q;
	P=2;
    if (SIGMA_R == 0.0) {
		Pcs=boucher_cyl(R,param);
	} else {
		find_LogNorm_int_range(2,R,SIGMA_R,&Rstart,&Rend,param);
		Pcs 	= sasfit_integrate(Rstart, Rend, &boucher_cyl_core, param);
	}
	return Pcs;
}

scalar sasfit_ff_boucher_cyl_f(scalar q, sasfit_param * param)
{
    scalar Pcs, Rstart, Rend;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here

	P=1;
    if (SIGMA_R == 0.0) {
		Pcs=boucher_cyl(R,param);
	} else {
		find_LogNorm_int_range(2,R,SIGMA_R,&Rstart,&Rend,param);
		Pcs 	= sasfit_integrate(Rstart, Rend, &boucher_cyl_core, param);
	}
	return Pcs;
}

scalar sasfit_ff_boucher_cyl_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return M_PI*R*R;
}

