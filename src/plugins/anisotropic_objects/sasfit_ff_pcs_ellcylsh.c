/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R0	param->p[0]
#define SIGMA_R0	fabs(param->p[1])
#define EPSILON	param->p[2]
#define T	param->p[3]
#define DUMMY1	param->p[4]
#define DUMMY2	param->p[5]
#define DUMMY3	param->p[6]
#define ETA_CORE	param->p[7]
#define ETA_SHELL	param->p[8]
#define ETA_SOLV	param->p[9]
#define R	param->p[MAXPAR-2]
#define Q	param->p[MAXPAR-1]


scalar ell_r(scalar radius, scalar epsilo, scalar t, scalar theta)
{
	return sqrt(gsl_pow_2((radius+t)*sin(theta))+gsl_pow_2((radius*epsilo+t)*cos(theta)));
}


scalar sasfit_ff_pcs_ellcylsh_I_core(scalar x, sasfit_param *param)
{
	scalar Ain, Aout,A,u1,u2,b1,b2;
	u1 = Q*ell_r(R,EPSILON,0,x);
	u2 = Q*ell_r(R,EPSILON,T,x);
	b1 = M_PI*R*R*EPSILON*(ETA_CORE-ETA_SHELL);
	b2 = M_PI*(R+T)*(R*EPSILON+T)*(ETA_SHELL-ETA_SOLV);

	if (u1==0) {
		Ain = 1.0;
	} else {
		Ain = 2.0*gsl_sf_bessel_J1(u1)/u1;
	}
	
	if (u2==0) {
		Aout = 1.0;
	} else {
		Aout = 2.0*gsl_sf_bessel_J1(u2)/u2;
	}

	A = b1*Ain+b2*Aout;
	return A*A;
}

scalar sasfit_ff_pcs_ellcylsh_Ampl_core(scalar x, sasfit_param *param)
{
	scalar Ain, Aout,A,u1,u2,b1,b2;
	u1 = Q*ell_r(R,EPSILON,0,x);
	u2 = Q*ell_r(R,EPSILON,T,x);
	b1 = M_PI*R*R*EPSILON*(ETA_CORE-ETA_SHELL);
	b2 = M_PI*(R+T)*(R*EPSILON+T)*(ETA_SHELL-ETA_SOLV);

	if (u1==0) {
		Ain = 1.0;
	} else {
		Ain = 2.0*gsl_sf_bessel_J1(u1)/u1;
	}
	
	if (u2==0) {
		Aout = 1.0;
	} else {
		Aout = 2.0*gsl_sf_bessel_J1(u2)/u2;
	}

	A = b1*Ain+b2*Aout;
	return A;
}

scalar sasfit_ff_pcs_ellcylsh_I_core_R0(scalar x, sasfit_param * param)
{
	scalar res, LNdistr;
	sasfit_param subParam;

	R=x;
	if (R==0 && T==0) {
		return 0.0;
	}
	if (EPSILON != 1) {
		res = 2.0/M_PI*sasfit_integrate(0.0,M_PI/2.0,&sasfit_ff_pcs_ellcylsh_I_core,param);
	} else {
		res = sasfit_ff_pcs_ellcylsh_I_core(M_PI/2.0,param);
	}

	if (SIGMA_R0 == 0) {
		LNdistr = 1.0;
	} else {
		sasfit_init_param( &subParam );
		subParam.p[0] = 1.0;
		subParam.p[1] = SIGMA_R0;
		subParam.p[2] = 1.0;
		subParam.p[3] = R0;

		LNdistr = sasfit_sd_LogNorm(x, &subParam);
		SASFIT_CHECK_SUB_ERR(param, subParam);
	}
	return LNdistr*res;
}

scalar sasfit_ff_pcs_ellcylsh_Ampl_core_R0(scalar x, sasfit_param * param)
{
	scalar res, LNdistr;
	sasfit_param subParam;

	R=x;
	if (R==0 && T==0) {
		return 0.0;
	}
	if (EPSILON != 1) {
		res = 2.0/M_PI*sasfit_integrate(0.0,M_PI/2.0,&sasfit_ff_pcs_ellcylsh_Ampl_core,param);
	} else {
		res = sasfit_ff_pcs_ellcylsh_Ampl_core(M_PI/2.0,param);
	}

	if (SIGMA_R0 == 0) {
		LNdistr = 1.0;
	} else {
		sasfit_init_param( &subParam );
		subParam.p[0] = 1.0;
		subParam.p[1] = SIGMA_R0;
		subParam.p[2] = 1.0;
		subParam.p[3] = R0;

		LNdistr = sasfit_sd_LogNorm(x, &subParam);
		SASFIT_CHECK_SUB_ERR(param, subParam);
	}
	return LNdistr*res;
}

scalar sasfit_ff_pcs_ellcylsh(scalar q, sasfit_param * param)
{
	scalar Pcs,Rstart, Rend;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R0 < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA_R0 < 0.0), param, "sigma_R(%lg) < 0",SIGMA_R0); // modify condition to your needs
	SASFIT_CHECK_COND1((EPSILON <= 0.0), param, "epsilon(%lg) <= 0",EPSILON); // modify condition to your needs
	SASFIT_CHECK_COND1((T < 0.0), param, "t(%lg) < 0",T); // modify condition to your needs

	// insert your code here	
	Q = q;

	if (SIGMA_R0 == 0.0) 
	{
		Pcs = sasfit_ff_pcs_ellcylsh_I_core_R0(R0,param);
	} else 
	{
		find_LogNorm_int_range(4,R0,SIGMA_R0,&Rstart,&Rend,param);
		Pcs = sasfit_integrate(Rstart, Rend, &sasfit_ff_pcs_ellcylsh_I_core_R0, param);
	}
	return Pcs;
}

scalar sasfit_ff_pcs_ellcylsh_f(scalar q, sasfit_param * param)
{
	scalar Pcs,Rstart, Rend;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	Q = q;

	if (SIGMA_R0 == 0.0) 
	{
		Pcs = sasfit_ff_pcs_ellcylsh_Ampl_core_R0(R0,param);
	} else 
	{
		find_LogNorm_int_range(4,R0,SIGMA_R0,&Rstart,&Rend,param);
		Pcs = sasfit_integrate(Rstart, Rend, &sasfit_ff_pcs_ellcylsh_Ampl_core_R0, param);
	}
	return Pcs;
}

scalar sasfit_ff_pcs_ellcylsh_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

