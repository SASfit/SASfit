/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */


#include "include/private.h"
#include <sasfit_error_ff.h>
#include <sasfit_utils_f2c.h>

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
#define Csc 1./sin

scalar sasfit_ff_magneticfieldalignedvesicle_psi(scalar q, sasfit_param * param)
{	
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((RSH < 0.0), param, "Rsh(%lg) < 0",RSH); 
	SASFIT_CHECK_COND1((T_SH < 0.0), param, "t_sh(%lg) < 0",T_SH); 
	SASFIT_CHECK_COND1((T_C < 0.0), param, "t_c(%lg) < 0",T_C); 

	PSI = PSI_DEG*M_PI/180.0;
	GAMA = PSI;
	return sasfit_ff_magneticfieldalignedvesicle_util_psi(q,param);
}

scalar sasfit_ff_magneticfieldalignedvesicle_psi_f(scalar q, sasfit_param * param)
{
	// insert your code here
	return 0.0;
}

scalar sasfit_ff_magneticfieldalignedvesicle_psi_v(scalar q, sasfit_param * param, int dist)
{
	// insert your code here
	return 0.0;
}

