/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define DUMMY	param->p[1]
#define SIGMA	fabs(param->p[2])

#define Q		param->p[MAXPAR-1]

scalar ThinSphericalShell_core(scalar x, sasfit_param * param)
{
	scalar P, LNdistr;
	
	sasfit_param subParam;
	
	SASFIT_ASSERT_PTR(param);
	sasfit_init_param( &subParam );


	if (Q == 0.0) {
		P = pow(4.0*M_PI*x*x,2.0);
	} else {
		P = gsl_pow_2(4.0*M_PI*x*sin(Q*x)/(Q));
	}

	if (SIGMA == 0) return P;

        //LNdistr = LogNorm(x, 1, sigma, p, R0);
	subParam.p[0] = 1.0;
	subParam.p[1] = SIGMA;
	subParam.p[2] = 1.0;
	subParam.p[3] = R;

	LNdistr = sasfit_sd_LogNorm(x, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	return LNdistr*P;
}

scalar sasfit_sq_p__q___thin_spherical_shell(scalar q, sasfit_param * param)
{
	scalar Rstart, Rend;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((DUMMY < 0.0), param, "dummy(%lg) < 0",DUMMY); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA < 0.0), param, "sigma(%lg) < 0",SIGMA); // modify condition to your needs

	// insert your code here

	Q = q;
	if (SIGMA == 0) return ThinSphericalShell_core(R,param);
	find_LogNorm_int_range(2,R,SIGMA,&Rstart,&Rend,param);
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

