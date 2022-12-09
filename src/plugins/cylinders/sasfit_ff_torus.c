/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define A	param->p[1]
#define DA	param->p[2]
#define NU	param->p[3]
#define ETAC	param->p[4]
#define ETASH	param->p[5]
#define ETASOL	param->p[6]
#define Q		param->p[MAXPAR-1]
#define THETA	param->p[MAXPAR-2]
#define	ETA		param->p[MAXPAR-3]
#define ATOT	param->p[MAXPAR-4]
#define P       param->p[MAXPAR-5]

scalar Torus_kernel_int_r(scalar r, sasfit_param *param)
{
	scalar gamma, KTcore, s,Q_2;

	SASFIT_ASSERT_PTR(param);

	s = Q/(2.0*M_PI);
	Q_2=Q/2.0;
	gamma = NU*sqrt(ATOT*ATOT - gsl_pow_2(r-R));
	KTcore = ETA*4.*M_PI*r
		* gsl_sf_bessel_J0(Q*r*sin(THETA))
		* gamma * gsl_sf_bessel_j0(Q*gamma*cos(THETA));

	return KTcore;
}

scalar Torus_kernel_int_theta(scalar theta,  sasfit_param *param)
{
	scalar TorusShell, int_r, int_r_delta;

	SASFIT_ASSERT_PTR(param);

	THETA = theta;
	ETA = 1.0; // eta

    int_r_delta	= 0;
    int_r=0;
	ATOT = A;
    int_r	= sasfit_integrate(R-ATOT, R+ATOT, Torus_kernel_int_r, param);
    if (!(DA == 0.0 || ETASH == ETASOL)) {
        ATOT = A+DA;
        int_r_delta	= sasfit_integrate(GSL_MAX(0,R-ATOT), R+ATOT, Torus_kernel_int_r, param);
    }

	if (DA == 0.0 || ETASH == ETASOL)	{
		return gsl_pow_int((ETAC-ETASOL)*int_r,lround(P)) * sin(theta);
	}
	if (ETASH == ETAC)
	{
		return gsl_pow_int((ETASH-ETASOL)*int_r_delta,lround(P)) * sin(theta);
	}

	TorusShell = (ETASH-ETASOL)*int_r_delta + (ETAC-ETASH)*int_r;

	return gsl_pow_int(TorusShell,lround(P)) * sin(theta);
}

scalar sasfit_ff_torus(scalar q, sasfit_param * param)
{
	scalar *aw, res,err,sum;
    scalar cubxmin[1], cubxmax[1], fval[1], ferr[1];
    int intstrategy, lenaw=4000;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((A < 0.0), param, "a(%lg) < 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((DA < 0.0), param, "Delta_a(%lg) < 0",DA); // modify condition to your needs
	SASFIT_CHECK_COND1((NU < 0.0), param, "nu(%lg) < 0",NU); // modify condition to your needs

	// insert your code here
	Q=q;
	P=2;
	sum = sum=sasfit_integrate(0.0, M_PI_2, Torus_kernel_int_theta, param);
	return sum;
}

scalar sasfit_ff_torus_f(scalar q, sasfit_param * param)
{
	scalar *aw, res,err,sum;
    scalar cubxmin[1], cubxmax[1], fval[1], ferr[1];
    int intstrategy, lenaw=4000;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here

	Q=q;
	P=1;
    sum=sasfit_integrate(0.0, M_PI_2, Torus_kernel_int_theta, param);
	return sum;
}

scalar sasfit_ff_torus_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

