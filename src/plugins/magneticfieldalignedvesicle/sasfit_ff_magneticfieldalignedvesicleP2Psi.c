/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */


#include <sasfit_error_ff.h>
#include "include/private.h"
#include <sasfit_utils.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_monte.h>
#include <gsl/gsl_monte_miser.h>
#include <gsl/gsl_rng.h>


// define shortcuts for local parameters/variables
#define RSH0	param->p[0]
#define THETA1	param->p[1]*M_PI/180.0
#define THETA2	param->p[2]*M_PI/180.0
#define T_SH	param->p[3]
#define T_C		param->p[4]
#define DETA_SH	param->p[5]
#define DETA_CE	param->p[6]
#define SIGMA   param->p[7]
#define PSI_DEG	param->p[8]
#define KAPPA	param->p[9]
#define Q		param->p[MAXPAR-1]
#define RKERNEL param->p[MAXPAR-2]
#define GAMA	param->p[MAXPAR-3]
#define _THETA	param->p[MAXPAR-4]
#define PHI		param->p[MAXPAR-5]
#define PSI		param->p[MAXPAR-6]
#define RSH		param->p[MAXPAR-7]

#define Power pow
#define Cos cos
#define Sin sin
#define Pi M_PI
#define Sec 1./cos

scalar probability(scalar *x, sasfit_param * param) 
{
	scalar p, norm, u, c, LNf, theta, phi, radius;
 
	theta	= x[0];
	phi		= x[1];
	radius	= x[2];
	if (KAPPA < 0) {
		u = sqrt(-KAPPA);
		norm = sqrt(M_PI)*gsl_sf_erf(u)/u;
		} 
	else if (KAPPA == 0.0) {
		norm = 2;
		} 
	else {
		u = sqrt(KAPPA);
		norm = 2.0*exp(KAPPA)*gsl_sf_dawson(u)/u; // maple: `assuming`([convert(int(exp(kappa*cos(theta)^2)*sin(theta), theta = 0 .. Pi), dawson)], [kappa > 0])
		}
	if (radius == 0.0) {
		LNf = 0.0;
		} 
	else {
		c = 1.0/(sqrt(M_PI*2.0)*fabs(SIGMA));
		LNf = c/radius * exp(-0.5*pow(log(radius/RSH0)/SIGMA,2.0));
		}
	p = LNf*exp(KAPPA*gsl_pow_2(cos(theta)))/(norm*2.0*M_PI);
	return p;
}

scalar sasfit_ff_magneticfieldalignedvesicle_p2psi_kernel(scalar *x, size_t dim, sasfit_param * param)
{
	scalar res,p;

	_THETA	= x[0];
	PHI		= x[1];
	RSH		= x[2];

	p = probability(x,param);
	GAMA = acos( sin(_THETA) * cos(PHI) * sin(PSI) + cos(_THETA) * cos(PSI) );
	res = sin(_THETA)*p*sasfit_ff_magneticfieldalignedvesicle_util_psi(Q,param);
	return res;
}

int sasfit_integrate_plugin_P2Psi(sasfit_func_monte_t intKern_fct,
							   scalar *xl, 
							   scalar *xu, 
							   size_t dim,  
							   sasfit_param * param, 
							   size_t calls, 
							   scalar *res, 
							   scalar *err)
{
	int status;
	static gsl_rng * r;
	static int rng_init=-1;
	gsl_monte_function F;
	gsl_monte_miser_state *s;

	if (rng_init < 0) {
		r = gsl_rng_alloc (gsl_rng_default);
		rng_init = 1;
	}

	s = gsl_monte_miser_alloc(dim);

	gsl_set_error_handler_off ();

	F.f = (double (*) (double*, size_t, void*)) intKern_fct;
	F.dim = dim;
	F.params = param;

	status = gsl_monte_miser_integrate(&F,xl,xu,dim,calls,r,s,res, err);

    gsl_monte_miser_free(s);

	return status;
 }


scalar sasfit_ff_magneticfieldalignedvesicle_p2psi_kernel_theta(scalar x, sasfit_param * param)
{
	scalar res,p;
	_THETA = x;
	p = 1;
	GAMA = acos( sin(_THETA) * cos(PHI) * sin(PSI) + cos(_THETA) * cos(PSI) );
	res = sin(_THETA)*p*sasfit_ff_magneticfieldalignedvesicle_util_psi(Q,param);
	return res;
}

scalar sasfit_ff_magneticfieldalignedvesicle_p2psi_kernel_phi(scalar x, sasfit_param * param)
{
	scalar res;
	PHI = x;
	res = 2.0*sasfit_integrate(0.0, M_PI/2.0, &sasfit_ff_magneticfieldalignedvesicle_p2psi_kernel_theta, param);
	return res;
}
scalar sasfit_ff_magneticfieldalignedvesicle_p2psi(scalar q, sasfit_param * param)
{	
	scalar res,err;
	scalar xl[3], xu[3];
	int status;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0),    param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((RSH < 0.0),  param, "Rsh(%lg) < 0",RSH); 
	SASFIT_CHECK_COND1((T_SH < 0.0), param, "t_sh(%lg) < 0",T_SH); 
	SASFIT_CHECK_COND1((T_C < 0.0),	 param, "t_c(%lg) < 0",T_C); 

	Q=q;

	gsl_set_error_handler_off ();
	if (KAPPA == 0.0) {
		PSI = 0.0;
		PHI=0.0;
//		if (RSH == 0.0) {
//			res = sasfit_ff_magneticfieldalignedvesicle_util_Psi(Q,param);
//		} else {
			res = 2.0*sasfit_integrate(0.0, M_PI/2.0, &sasfit_ff_magneticfieldalignedvesicle_p2psi_kernel_theta, param);
//		}
	} else {
		PSI = PSI_DEG*M_PI/180.0;
		xl[0]=0.0;
		xl[1]=0.0;
		xl[2]=0.0;
		xu[0]=Pi;
		xu[1]=2.0*Pi;
		xu[2]=RSH0*exp(-SIGMA*SIGMA*(1.0-6.0)+sqrt(2.0*SIGMA*SIGMA*log(100.0/0.1)));

//		if (RSH0 == 0.0) {
//			res = sasfit_ff_magneticfieldalignedvesicle_util_Psi(Q,param);
//		} else {
			status = sasfit_integrate_plugin_P2Psi(&sasfit_ff_magneticfieldalignedvesicle_p2psi_kernel,
											   xl,xu,3,param,sasfit_eps_get_iter_4_mc(),&res,&err);
			sasfit_out("Q=%lf, Int=%lf, Err/Int=%lf Rmax=%lf\n",Q,res,err/res,xu[2]);
			return res;

//			res = sasfit_integrate(0.0, 2.0*M_PI, &sasfit_ff_magneticfieldalignedvesicleP2Psi_kernel_Phi, param);
//		}
	}
	return res;
}

scalar sasfit_ff_magneticfieldalignedvesicle_p2psi_f(scalar q, sasfit_param * param)
{
	// insert your code here
	return 0.0;
}

scalar sasfit_ff_magneticfieldalignedvesicle_p2psi_v(scalar q, sasfit_param * param, int dist)
{
	// insert your code here
	return 0.0;
}
