/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define ETA	param->p[1]
#define Q param->p[MAXPAR-1]

scalar sasfit_sq_one_d_hard_spheres_random_orient_theta(scalar x, sasfit_param * param)
{
    scalar D,cQ,Sq,nn, xi, tmp,xq;
    xq = x*Q;
    D = 2. * R;
    nn = ETA / D;
    if (R==0 || ETA ==0) return 1;
    xi = (1 - ETA);
    if (fabs(xq)<1e-4) {
        tmp = gsl_pow_2(D/xi)*nn/2.-gsl_pow_2(D*D*xq/xi)*nn/24.+gsl_pow_6(D)*gsl_pow_4(xq)*nn/(720*xi*xi);
    } else {
        tmp = nn / gsl_pow_2(xq*xi) * (1 - cos(xq * D));
    }
    cQ = -2 * (D / xi * gsl_sf_sinc(xq * D/M_PI) + tmp);
    Sq = 1/(1 - cQ * nn);
	return Sq;
}

scalar sasfit_sq_one_d_hard_spheres_random_orient(scalar q, sasfit_param * param)
{
    scalar D,cQ,Sq,nn, xi, tmp;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA < 0.0), param, "eta(%lg) < 0",ETA); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA >= 1.0), param, "eta(%lg) >= 1",ETA); // modify condition to your needs

	// insert your code here
	Q = q;
 	return sasfit_integrate(0,1,&sasfit_sq_one_d_hard_spheres_random_orient_theta,param);
}

scalar sasfit_sq_one_d_hard_spheres_random_orient_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_one_d_hard_spheres_random_orient_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

