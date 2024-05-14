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

scalar crJ0align(scalar rp,sasfit_param * param) {
    scalar cr,a,p,sigma;

    p=(4.*sqrt(3.)*M_PI-12.)/gsl_pow_2(M_PI);
    a= 0.3699*gsl_pow_4(ETA)
      -1.2511*gsl_pow_3(ETA)
      +2.0199*gsl_pow_2(ETA)
      -2.2373*ETA
      +2.1;
    sigma=2*R;
    if (rp>1) return 0;
    cr=  (-(1-p*ETA*ETA)/gsl_pow_2(1-2*ETA+p*ETA*ETA))
        *(1-a*a*ETA+a*a*ETA/M_PI_2*(acos(rp/a)-rp/a*sqrt(1-gsl_pow_2(rp/a))));
    return 2*M_PI*sigma*sigma*cr*gsl_sf_bessel_J0(Q*sigma*rp)*rp;
}

scalar sasfit_sq__d_hard_disks__guo_aligned(scalar q, sasfit_param * param)
{
    scalar Cq, psi,gamma;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA < 0.0), param, "eta(%lg) < 0",ETA); // modify condition to your needs

	// insert your code here

	psi   = sasfit_param_override_get_psi(PSI_DEG * M_PI/180.);
	gamma = acos(cos(psi)*sin(THETA_DEG*M_PI/180.)*cos(PHI_DEG*M_PI/180.)
                +sin(psi)*sin(THETA_DEG*M_PI/180.)*sin(PHI_DEG*M_PI/180.));
	Q=q*sin(gamma);
    Cq=sasfit_integrate(0,1,&crJ0align,param);
    return 1/(1-ETA/(M_PI*R*R)*Cq);
}

scalar sasfit_sq__d_hard_disks__guo_aligned_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq__d_hard_disks__guo_aligned_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

