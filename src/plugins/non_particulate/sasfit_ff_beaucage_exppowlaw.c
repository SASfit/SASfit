/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define G	param->p[0]
#define B	param->p[1]
#define GS	param->p[2]
#define BS	param->p[3]
#define RG	param->p[4]
#define RSUB	param->p[5]
#define RS	param->p[6]
#define P	param->p[7]
#define PS	param->p[8]

scalar sasfit_ff_beaucage_exppowlaw(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((G < 0.0), param, "G(%lg) < 0",G); // modify condition to your needs
	SASFIT_CHECK_COND1((B < 0.0), param, "B(%lg) < 0",B); // modify condition to your needs
	SASFIT_CHECK_COND1((GS < 0.0), param, "Gs(%lg) < 0",GS); // modify condition to your needs
	SASFIT_CHECK_COND1((BS < 0.0), param, "Bs(%lg) < 0",BS); // modify condition to your needs
	SASFIT_CHECK_COND1((RG < 0.0), param, "Rg(%lg) < 0",RG); // modify condition to your needs
	SASFIT_CHECK_COND1((RSUB < 0.0), param, "Rsub(%lg) < 0",RSUB); // modify condition to your needs
	SASFIT_CHECK_COND1((RS < 0.0), param, "Rs(%lg) < 0",RS); // modify condition to your needs
	SASFIT_CHECK_COND1((P < 0.0), param, "P(%lg) < 0",P); // modify condition to your needs
	SASFIT_CHECK_COND1((PS < 0.0), param, "Ps(%lg) < 0",PS); // modify condition to your needs

	// insert your code here
    
    
	scalar x, xs, xsub, k, ks, tmp, qsub; 

	x 	= q*RG;
	xs 	= q*RS;
	k 	= 1;
	ks 	= 1.;
	xsub 	= q*RSUB;

	tmp  = gsl_pow_3( gsl_sf_erf(x *k /sqrt(6.))) / q;
	qsub = gsl_pow_3( gsl_sf_erf(xs*ks/sqrt(6.))) / q;

	return   G* exp(-x *x /3.0)   + B *exp(-xsub*xsub/3.)*pow(tmp,P) + 
                 GS*exp(-xs*xs/3.0) + BS*  pow(qsub,PS);
}

scalar sasfit_ff_beaucage_exppowlaw_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_beaucage_exppowlaw_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

