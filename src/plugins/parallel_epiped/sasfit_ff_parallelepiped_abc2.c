/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include <sasfit.h>

#include <sasfit_common.h>
#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
scalar sasfit_ff_parallelepiped_abc2_Pnu(scalar x, sasfit_param * param) {
	scalar LNdistr;
	sasfit_param subParam;
	sasfit_init_param( &subParam );

	subParam.p[0] = 1.0;
	subParam.p[1] = SIGMA;
	subParam.p[2] = 1.0;
	subParam.p[3] = 1.0;
	LNdistr = sasfit_sd_LogNorm(x, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);
	if ( subParam.errStatus != FALSE ) return 42;
    sasfit_copy_param(&subParam,param->p);

	subParam.p[0] = x*A;
	subParam.p[1] = x*B;
	return LNdistr*sasfit_ff_parallelepiped_abc(Q,&subParam);
}

scalar sasfit_ff_parallelepiped_abc2(scalar q, sasfit_param * param)
{
	scalar sum;
    scalar cubxmin[3], cubxmax[3], fval[1], ferr[1];
    int intstrategy;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((A < 0.0), param, "a(%lg) < 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((B < 0.0), param, "b(%lg) < 0",B); // modify condition to your needs
	SASFIT_CHECK_COND1((C < 0.0), param, "c(%lg) < 0",C); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA < 0.0), param, "eta(%lg) < 0",ETA); // modify condition to your needs

    if (SIGMA==0) return sasfit_ff_parallelepiped_abc(q,param);
	Q=q;
	NUDIM = 2;
	// insert your code here

    cubxmin[0]=0;
    cubxmax[0]=M_PI/2.0;
    cubxmin[1]=0;
    cubxmax[1]=M_PI/2.0;
    find_LogNorm_int_range(4,1,SIGMA,&cubxmin[2], &cubxmax[2], param);

    if (   (sasfit_get_int_strategy()==H_CUBATURE   &&sasfit_get_sphavg_strategy()==SPHAVG_HCUBATURE)
        || (sasfit_get_int_strategy()==P_CUBATURE   &&sasfit_get_sphavg_strategy()==SPHAVG_PCUBATURE)
        || (sasfit_get_int_strategy()==GSL_MC_MISER &&sasfit_get_sphavg_strategy()==SPHAVG_MC_MISER)
        || (sasfit_get_int_strategy()==GSL_MC_VEGAS &&sasfit_get_sphavg_strategy()==SPHAVG_MC_VEGAS)
        || (sasfit_get_int_strategy()==GSL_MC_PLAIN &&sasfit_get_sphavg_strategy()==SPHAVG_MC_PLAIN)
        ) {
        sasfit_cubature(3,cubxmin,cubxmax,&K_SphAvg_P, param, sasfit_eps_get_nriq(),
				&fval[0], &ferr[0]);
        return fval[0];
    }
    return sasfit_integrate(cubxmin[2], cubxmax[2], &sasfit_ff_parallelepiped_abc2_Pnu, param);
}

scalar sasfit_ff_parallelepiped_abc2_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
    if (SIGMA==0) return sasfit_ff_parallelepiped_abc_f(q,param);
	return 0.0;
}

scalar sasfit_ff_parallelepiped_abc2_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
    if (SIGMA==0) return sasfit_ff_parallelepiped_abc_v(q,param,dist);
	return A*B*C;
}

