/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define ETA	param->p[1]
#define Q   param->p[MAXPAR-1]
#define QX  param->p[MAXPAR-2]

scalar crJ0random(scalar rp,sasfit_param * param) {
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
    return 2*M_PI*sigma*sigma*cr*gsl_sf_bessel_J0(QX*sigma*rp)*rp;
}

scalar sasfit_sq__d_hard_disks__guo_random_x(scalar x, sasfit_param * param)
{
    scalar Cq;
	QX=Q*x;
    Cq=sasfit_integrate(0,1,&crJ0random,param);
    return 1/(1-ETA/(M_PI*R*R)*Cq);
}

scalar sasfit_sq__d_hard_disks__guo_random(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA < 0.0), param, "eta(%lg) < 0",ETA); // modify condition to your needs

	// insert your code here
	Q=q;
    return sasfit_integrate(0,1,&sasfit_sq__d_hard_disks__guo_random_x,param);
}

scalar sasfit_sq__d_hard_disks__guo_random_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq__d_hard_disks__guo_random_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

