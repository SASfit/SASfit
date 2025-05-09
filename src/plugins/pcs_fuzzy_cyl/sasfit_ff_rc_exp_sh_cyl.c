/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define RC	        param->p[0]
#define T	        param->p[1]
#define ALPHA	    param->p[2]
#define SIGMA_S     param->p[3]
#define ETAC	    param->p[4]
#define ETASHIN	    param->p[5]
#define ETASHOUT	param->p[6]
#define ETASOLV 	param->p[7]

#define Q	param->p[MAXPAR-1]


scalar rc_exp_cyl(scalar x,sasfit_param * param) {
    scalar r, JD;
//    r = x/(1-x);
//    JD=1/(1-x)+x/gsl_pow_2(1-x);
    r=x;
    JD=1;
    return 2*M_PI*r*gsl_sf_bessel_J0(Q*r)*JD*(sasfit_ff_rc_exp_sh_cyl_profile(r,param)-ETASOLV);
}

scalar sasfit_ff_rc_exp_sh_cyl(scalar q, sasfit_param * param)
{
    scalar Pcs, Rstart, Rend;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((RC < 0.0), param, "RC(%lg) < 0",RC); // modify condition to your needs
	SASFIT_CHECK_COND1((T < 0.0), param, "t(%lg) < 0",T); // modify condition to your needs


	// insert your code here
	return gsl_pow_2(sasfit_ff_rc_exp_sh_cyl_f(q,param));
}

scalar sasfit_ff_rc_exp_sh_cyl_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	Q=q;
    return sasfit_integrate(0,RC+T,&rc_exp_cyl,param);
}

scalar sasfit_ff_rc_exp_sh_cyl_v(scalar x, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	switch (dist) {
	    case 1:     return M_PI*gsl_pow_2(x+T);
	    case 2:     return M_PI*gsl_pow_2(RC+x);
        default:    return M_PI*gsl_pow_2(RC+T);
    }
}

