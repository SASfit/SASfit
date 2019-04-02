/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define MU	param->p[0]
#define SIGMA	param->p[1]
#define DUMMY	param->p[2]
#define SIGMATOT	(param->p[3]/(4.*M_PI*M_PI))
#define T	param->p[4]
#define LAMBDA	param->p[5]
#define Q   param->p[MAXPAR-1]
#define R   param->p[MAXPAR-2]

scalar grm0(scalar r, void * pam)
{
    sasfit_param * param;
	sasfit_param subParam;
	sasfit_param subParam2;
	param = (sasfit_param *) pam;
	sasfit_init_param( &subParam );
	sasfit_init_param( &subParam2 );
	subParam2.p[0] = r;
	subParam2.p[1] = SIGMA;
	subParam2.p[2] = DUMMY;
	subParam2.p[3] = SIGMATOT;

	subParam.p[0] = 1.0;
	subParam.p[1] = SIGMA;
	subParam.p[2] = 1.0;
	subParam.p[3] = MU;
	return sasfit_sd_LogNorm(r, &subParam)*sasfit_ff_gz_sphere(R,&subParam2);
}

scalar g0(scalar r, void * pam)
{
    sasfit_param * param;
	sasfit_param subParam;
	sasfit_param subParam2;
	param = (sasfit_param *) pam;
	sasfit_init_param( &subParam );
	sasfit_init_param( &subParam2 );
	subParam2.p[0] = r;
	subParam2.p[1] = SIGMA;
	subParam2.p[2] = DUMMY;
	subParam2.p[3] = SIGMATOT;

	subParam.p[0] = 1.0;
	subParam.p[1] = SIGMA;
	subParam.p[2] = 1.0;
	subParam.p[3] = MU;
	return sasfit_sd_LogNorm(r, &subParam)*sasfit_ff_gz_sphere_f(0,&subParam2);
}

scalar i1_polydisp_sphere(scalar r, void * pam)
{
    scalar i1rmir0,i10, imr, k0,k02,min,max;
    sasfit_param * param;
    scalar *aw, res,err,eps_nriq;
    int lenaw=4000;
	param = (sasfit_param *) pam;
	k0=2*M_PI/LAMBDA;
	k02=k0*k0;
	find_LogNorm_int_range(4,MU,SIGMA,&min, &max, param);
	if (r>2*max) return 0;
	R=r;
	aw = (scalar *)malloc((lenaw)*sizeof(scalar));
    eps_nriq=sasfit_eps_get_nriq();
//	eps_nriq=1e-60;
    sasfit_intdeini(lenaw, GSL_DBL_MIN, eps_nriq, aw);
    sasfit_intde(&g0,min,max,aw,&res,&err,param);
    i10=res;
    sasfit_intde(&grm0,min,max,aw,&res,&err,param);
    i1rmir0=res;
    free(aw);
 //       i10=sasfit_integrate(R/2.,GSL_POSINF,&g0,param);
 //       i1rmir0=sasfit_integrate(R/2.,GSL_POSINF,&grm0,param);

	i1rmir0=SIGMATOT*i1rmir0/i10*gsl_pow_2(2*M_PI)*T;
	i10=SIGMATOT*gsl_pow_2(2*M_PI)*T;
	imr = k02*(exp(i1rmir0/k02)-exp(-i10/k02));
	return gsl_sf_bessel_J0(Q*r)*imr*r/(2*M_PI*T);
}

scalar sasfit_ff_msas_polydisp_spheres(scalar q, sasfit_param * param)
{
    scalar *aw, res,err,eps_nriq;
    int lenaw=4000;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((MU < 0.0), param, "mu(%lg) < 0",MU); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA < 0.0), param, "sigma(%lg) < 0",SIGMA); // modify condition to your needs
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
    sasfit_intdeo(&i1_polydisp_sphere,0,q,aw,&res,&err,param);
    free(aw);
	return res;
}

scalar sasfit_ff_msas_polydisp_spheres_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_msas_polydisp_spheres_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

