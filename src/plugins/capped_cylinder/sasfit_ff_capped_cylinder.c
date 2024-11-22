/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	    param->p[0]
#define H	    param->p[1]
#define L	    param->p[2]
#define ETA	    param->p[3]
#define Q       param->p[MAXPAR-1]
#define RCAP    param->p[MAXPAR-2]
#define P       param->p[MAXPAR-3]
#define ALPHA   param->p[MAXPAR-4]

scalar AcaptQ0(scalar t, sasfit_param *param) {
    return (1-t*t)/2;
}

scalar Acapt(scalar t, sasfit_param *param) {
    return cos(Q*cos(ALPHA)*(RCAP*t+H+L/2.))*(1-t*t)*sasfit_jinc(Q*RCAP*sin(ALPHA)*sqrt(1-t*t));
}

scalar Acapped(scalar alpha, sasfit_param *param) {
    scalar Acyl, Acap;
    ALPHA=alpha;
    Acyl=M_PI*R*R*L*gsl_sf_bessel_j0(Q*L/2.*cos(alpha))*2*sasfit_jinc(Q*R*sin(alpha));
    if (RCAP==0) {
        Acap = 0;
    } else if (-H/RCAP==1) {
        Acap = 0;
    } else {
        Acap=4*M_PI*RCAP*RCAP*RCAP*sasfit_integrate(-H/RCAP,1,&Acapt,param);
    }
    return gsl_pow_int(ETA*(Acyl+Acap),lround(P))*sin(alpha);
}

scalar sasfit_ff_capped_cylinder(scalar q, sasfit_param * param)
{
    scalar Rcap;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "r(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((L < 0.0), param, "L(%lg) < 0",L); // modify condition to your needs

	// insert your code here
	RCAP = gsl_hypot(H,R);
	Q=q;
	P=2;
	ALPHA=0;
	if (R==0 && L==0 && RCAP==0) return 0.0;
	return sasfit_integrate(0,M_PI_2,&Acapped, param);
}

scalar sasfit_ff_capped_cylinder_f(scalar q, sasfit_param * param)
{
	scalar Rcap;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "r(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((L < 0.0), param, "L(%lg) < 0",L); // modify condition to your needs

	// insert your code here
	RCAP = gsl_hypot(H,R);
	Q=q;
	P=1;
	ALPHA=0;
	if (R==0 && L==0 && RCAP==0) return 0.0;
	return sasfit_integrate(0,M_PI_2,&Acapped, param);
}

scalar sasfit_ff_capped_cylinder_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return M_PI*R*R*L+4*M_PI*gsl_pow_3(RCAP)*(1./3.-gsl_pow_3(H/RCAP)/6.+H/RCAP/2.);
}

