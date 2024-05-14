/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define ETA	param->p[1]
#define THETA_DEG param->p[2]
#define PHI_DEG param->p[3]
#define PSI_DEG param->p[4]
#define Q   param->p[MAXPAR-1]

scalar sasfit_sq__d_hard_disks__rosenfeld_aligned(scalar q, sasfit_param * param)
{
    scalar A, B, Z, XI, G, _1Sq, qR, psi, gamma;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA <= 0.0), param, "eta(%lg) <= 0",ETA); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA >= 1.0), param, "eta(%lg) >= 1",ETA); // modify condition to your needs
	// insert your code here
    psi   = sasfit_param_override_get_psi(PSI_DEG * M_PI/180.);
	gamma = acos(cos(psi)*sin(THETA_DEG*M_PI/180.)*cos(PHI_DEG*M_PI/180.)
                +sin(psi)*sin(THETA_DEG*M_PI/180.)*sin(PHI_DEG*M_PI/180.));
	Q=q*sin(gamma);
	qR=Q*R;
    G = 1./(1.-ETA);
    XI = (1.+ETA)/gsl_pow_3(1.-ETA);
    B=((1.-ETA)*XI-1.-3.*ETA*G)/ETA;
	A=(1.+(2.*ETA-1.)*XI+2*ETA*G)/ETA;
	_1Sq= 1.+4.*ETA*(A*gsl_pow_2(sasfit_jinc(qR))
                    +B*gsl_sf_bessel_J0(qR)*sasfit_jinc(qR)
                    +G*2*sasfit_jinc(2.*qR));
	return 1./_1Sq;
}

scalar sasfit_sq__d_hard_disks__rosenfeld_aligned_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq__d_hard_disks__rosenfeld_aligned_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

