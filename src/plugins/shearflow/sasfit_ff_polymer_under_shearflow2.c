/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define RG	param->p[0]
#define DUMMY	param->p[1]
#define PSI_DEG	param->p[2]
#define NU	param->p[3]
#define A	param->p[4]
#define B	param->p[5]
#define G	param->p[6]
#define D	param->p[7]
#define Q   param->p[MAXPAR-1]
#define PSI	param->p[MAXPAR-2]
#define NN  param->p[MAXPAR-3]
#define MM  param->p[MAXPAR-4]

scalar sasfit_ff_polymer_under_shearflow2(scalar q, sasfit_param * param)
{	scalar b,c,expbnu;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((RG < 0.0), param, "Rg(%lg) < 0",RG); // modify condition to your needs

	// insert your code here

	PSI = sasfit_param_override_get_psi(PSI_DEG * M_PI/180.);

	b= gsl_pow_2(RG*A*q*cos(PSI))+gsl_pow_2(RG*B*q*sin(PSI));
	c= gsl_pow_2(RG*G*q*cos(PSI))+gsl_pow_2(RG*D*q*sin(PSI));

	expbnu = exp(-b*NU);
	return expbnu/c+(1-expbnu)*(1/b-1/(b*b))+expbnu/(c*c)*(exp(-c*(1-NU))-1)+expbnu*(1/b-1/c)*NU;
	return 2*(exp(-c - b*NU)*(gsl_pow_2(b)*exp(c*NU) +
       exp(c)*((-1 + b)*gsl_pow_2(c)*exp(b*NU) +
          (-b + c)*(b + c + b*c*(-1 + NU)))))/
   (gsl_pow_2(b*c)) ;

	return 2/(b*b)*(b-1+exp(-NU*b)
	                   *(1+(gsl_pow_2(b/c)-1)*exp((NU-1)*c)+(1-NU)*b/c*(b-c)));
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

