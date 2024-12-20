/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define DUMMY	param->p[1]
#define SIGMA	fabs(param->p[2])


scalar ThinSphericalShell_core(scalar x, sasfit_param * param)
{
	scalar IP, LNdistr;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);
	sasfit_init_param( &subParam );


	if (Q == 0.0) {
		IP = pow(4.0*M_PI*x*x,2.0);
	} else {
		IP = gsl_pow_2(4.0*M_PI*x*sin(Q*x)/(Q));
	}

	if (SIGMA <= 1E-6 || x==0.0) return IP;

        //LNdistr = LogNorm(x, 1, sigma, p, R0);
	subParam.p[0] = 1.0;
	subParam.p[1] = SIGMA;
	subParam.p[2] = 1.0;
	subParam.p[3] = R;

	LNdistr = sasfit_sd_LogNorm(x, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);
    if ( subParam.errStatus != FALSE ) {
        sasfit_out("LogNormError: SIGMA:%lf\n",SIGMA);
        return 1;
    }

	return LNdistr*IP;
}

scalar sasfit_sq_p__q___thin_spherical_shell(scalar q, sasfit_param * param)
{
	scalar Rstart, Rend;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs

	// insert your code here

	Q = q;
	if (SIGMA <= 1E-6 || R==0.0) return ThinSphericalShell_core(R,param);
	find_LogNorm_int_range(4,R,SIGMA,&Rstart,&Rend,param);
	return sasfit_integrate(Rstart,Rend,&ThinSphericalShell_core,param);

}

scalar sasfit_sq_p__q___thin_spherical_shell_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_p__q___thin_spherical_shell_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

