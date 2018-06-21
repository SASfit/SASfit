/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define THICK	param->p[0]
#define SIGMA_T	fabs(param->p[1])


#define ETA_L	param->p[4]
#define ETA_SOL	param->p[5]

#define Q	param->p[MAXPAR-1]

scalar homoXScore(scalar x, sasfit_param * param)
{
	scalar Pcs, u, LNdistr;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);
	sasfit_init_param( &subParam );

   	u = Q*x*0.5;
	Pcs = gsl_pow_int(x*gsl_sf_bessel_j0(u),lround(P));

	subParam.p[0] = 1.0;
	subParam.p[1] = SIGMA_T;
	subParam.p[2] = 1.0;
	subParam.p[3] = THICK;

	LNdistr = sasfit_sd_LogNorm(x, &subParam);

	SASFIT_CHECK_SUB_ERR(param, subParam);

	return LNdistr*Pcs;
}


scalar homogeneousXS(scalar q, sasfit_param * param)
{
	scalar Pcs, u;
	scalar tstart, tend;

	SASFIT_ASSERT_PTR(param);


    Q = q;
	if (SIGMA_T <= 1.0e-6 || THICK == 0.0) {
		Pcs = gsl_pow_int(THICK*gsl_sf_bessel_j0(Q*THICK/2.0),lround(P));
	} else {
		find_LogNorm_int_range(2,THICK,SIGMA_T,&tstart, &tend, param);
		Pcs = sasfit_integrate(tstart, tend, &homoXScore, param);
	}
	return Pcs;
}

scalar sasfit_ff_pcs_homogeneousplate(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((THICK < 0.0), param, "t(%lg) < 0",THICK); // modify condition to your needs

	// insert your code here
    P=2;
	return homogeneousXS(q,param)*gsl_pow_int(ETA_L-ETA_SOL,lround(P));
}

scalar sasfit_ff_pcs_homogeneousplate_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
    P=1;
	return homogeneousXS(q,param)*gsl_pow_int(ETA_L-ETA_SOL,lround(P));
}

scalar sasfit_ff_pcs_homogeneousplate_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return THICK;
}

