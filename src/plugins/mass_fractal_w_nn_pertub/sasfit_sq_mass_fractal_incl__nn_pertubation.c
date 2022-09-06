/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define Z1	param->p[1]
#define XI	param->p[2]
#define D	param->p[3]
#define N	param->p[4]
#define Q   param->p[MAXPAR-1]


//
// P. Dimon, S. K. Sinha, D. A. Weitz, C. R. Safinya, G. S. Smith, W. A. Varady, and H. M. Lindsay
// Phys. Rev. Lett. 57, 595 – Published 4 August 1986
//

scalar zAfunc(scalar r, void * pam)
{
    sasfit_param * param;
	param = (sasfit_param *) pam;
	return  pow(r,D-1)*exp(-r/XI)*gsl_sf_bessel_j0(Q*r);
}

scalar sasfit_sq_mass_fractal_incl__nn_pertubation(scalar q, sasfit_param * param)
{
    scalar zA, qR;
    scalar *aw, res,err,eps_nriq;
    int lenaw=4000;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((Z1 < 0.0), param, "z1(%lg) < 0",Z1); // modify condition to your needs
	SASFIT_CHECK_COND1((XI <= 0.0), param, "xi(%lg) <= 0",XI); // modify condition to your needs
	SASFIT_CHECK_COND1((D <= 1.0), param, "D(%lg) <= 1",D); // modify condition to your needs
	SASFIT_CHECK_COND1((N <= 0.0), param, "N(%lg) <= 0",N); // modify condition to your needs

	// insert your code here
	Q=q;
    qR= q*R;
    zA = (N-Z1-1)/(pow(XI,D)*gsl_sf_gamma_inc(D,2*R/XI));

	aw = (scalar *)malloc((lenaw)*sizeof(scalar));
	eps_nriq=sasfit_eps_get_nriq();
    sasfit_intdeoini(lenaw, GSL_DBL_MIN, eps_nriq, aw);
    sasfit_intdeo(&zAfunc,2*R,q, aw, &res, &err, param);
    free(aw);

	return 1+Z1*gsl_sf_bessel_j0(2*qR)+zA*res;
}

scalar sasfit_sq_mass_fractal_incl__nn_pertubation_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_mass_fractal_incl__nn_pertubation_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

