/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define RG	param->p[0]
#define NU	param->p[1]
#define DUMMY	param->p[2]
#define SIGMATOT	(param->p[3]/(4.*M_PI*M_PI))
#define T	param->p[4]
#define LAMBDA	param->p[5]
#define Q   param->p[MAXPAR-1]

scalar i1_ggc(scalar r, void * pam)
{
    scalar i1rmir0,i10, imr, k0,k02;
    sasfit_param * param;
	param = (sasfit_param *) pam;
	k0=2*M_PI/LAMBDA;
	k02=k0*k0;
	i10=sasfit_ff_gz_ggc_f(0,param);
	i1rmir0=SIGMATOT*sasfit_ff_gz_ggc(r,param)/i10*gsl_pow_2(2*M_PI)*T;
	i10=SIGMATOT*gsl_pow_2(2*M_PI)*T;
	imr = k02*(exp(i1rmir0/k02)-exp(-i10/k02));
	return gsl_sf_bessel_J0(Q*r)*imr*r/(2*M_PI*T);
}

scalar sasfit_ff_msas_ggc(scalar q, sasfit_param * param)
{
    scalar *aw, res,err,eps_nriq;
    int lenaw=4000;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((RG < 0.0), param, "Rg(%lg) < 0",RG); // modify condition to your needs
	SASFIT_CHECK_COND1((NU < 0.0), param, "nu(%lg) < 0",NU); // modify condition to your needs
	SASFIT_CHECK_COND1((DUMMY < 0.0), param, "dummy(%lg) < 0",DUMMY); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMATOT < 0.0), param, "Sigma_t(%lg) < 0",SIGMATOT); // modify condition to your needs
	SASFIT_CHECK_COND1((T < 0.0), param, "t(%lg) < 0",T); // modify condition to your needs
	SASFIT_CHECK_COND1((LAMBDA < 0.0), param, "lambda(%lg) < 0",LAMBDA); // modify condition to your needs

	// insert your code here
    Q=q;
	aw = (scalar *)malloc((lenaw)*sizeof(scalar));
	eps_nriq=sasfit_eps_get_nriq();
//	eps_nriq=1e-60;
    sasfit_intdeoini(lenaw, GSL_DBL_MIN, eps_nriq, aw);
    sasfit_intdeo(&i1_ggc,0,q, aw, &res, &err, param);
    free(aw);
	return res;
}

scalar sasfit_ff_msas_ggc_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_msas_ggc_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

