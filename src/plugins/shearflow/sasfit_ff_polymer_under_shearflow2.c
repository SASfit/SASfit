/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define RG	param->p[0]
#define FP	param->p[1]
#define ALPHA	param->p[2]
#define BETA	param->p[3]
#define GAMMA	param->p[4]
#define DELTA	param->p[5]
#define I0  param->p[6]
#define THETA0_DEG	param->p[7]
#define THETA0	(param->p[7]*M_PI/180)
#define PSI_DEG	param->p[8]
#define DELTA_THETA0DEG  param->p[9]
#define DELTA_THETA0    (param->p[9]*M_PI/180.)
#define Q   param->p[MAXPAR-1]
#define PSI	param->p[MAXPAR-2]
#define NN  param->p[MAXPAR-3]
#define MM  param->p[MAXPAR-4]

scalar sasfit_ff_polymer_under_shearflow2_core(scalar theta, sasfit_param * param) {
    scalar b,c,expbnu;
    b= gsl_pow_2(RG*ALPHA*Q*cos(theta+THETA0-PSI))+gsl_pow_2(RG*BETA *Q*sin(theta+THETA0-PSI));
    c= gsl_pow_2(RG*GAMMA*Q*cos(theta+THETA0-PSI))+gsl_pow_2(RG*DELTA*Q*sin(theta+THETA0-PSI));
    expbnu = exp(-b*FP);


    return 2*(expbnu/c+(1-expbnu)*(1/b-1/(b*b))+expbnu/(c*c)*(exp(-c*(1-FP))-1)+expbnu*(1/b-1/c)*FP);
	return 2*(exp(-c - b*FP)*(gsl_pow_2(b)*exp(c*FP) +
       exp(c)*((-1 + b)*gsl_pow_2(c)*exp(b*FP) +
          (-b + c)*(b + c + b*c*(-1 + FP))))) / (gsl_pow_2(b*c));

	return 2/(b*b)*(b-1+exp(-FP*b)
	                   *(1+(gsl_pow_2(b/c)-1)*exp((FP-1)*c)+(1-FP)*b/c*(b-c)));
}

scalar sasfit_ff_polymer_under_shearflow2(scalar q, sasfit_param * param)
{

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0) , param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((RG < 0.0), param, "Rg(%lg) < 0",RG); // modify condition to your needs
	SASFIT_CHECK_COND1((FP < 0.0), param, "fp(%lg) < 0",FP); // modify condition to your
	SASFIT_CHECK_COND1((FP > 1.0), param, "fp(%lg) > 1",FP); // modify condition to your needs

	// insert your code here

	Q = q;
	PSI = sasfit_param_override_get_psi(PSI_DEG * M_PI/180.);

	if (DELTA_THETA0 == 0) return I0*sasfit_ff_polymer_under_shearflow2_core(0,param);
	return I0*sasfit_integrate(-DELTA_THETA0/2.,DELTA_THETA0/2.,&sasfit_ff_polymer_under_shearflow2_core,param)/DELTA_THETA0;
}

scalar sasfit_ff_polymer_under_shearflow2_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_polymer_under_shearflow2_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

