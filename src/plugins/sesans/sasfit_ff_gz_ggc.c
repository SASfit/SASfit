/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define RG	param->p[0]
#define NU param->p[1]
#define DUMMY2 param->p[2]
#define I0	param->p[3]
scalar sasfit_ff_gz_ggc(scalar z, sasfit_param * param)
{
	scalar w,Gz,G0;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((z < 0.0), param, "z(%lg) < 0",z);
	SASFIT_CHECK_COND1((RG < 0.0), param, "R(%lg) < 0",RG); // modify condition to your needs
    SASFIT_CHECK_COND1((NU <= 0.0), param, "nu(%lg) <= 0",NU); // modify condition to your needs
    SASFIT_CHECK_COND1((NU >= 0.5), param, "nu(%lg) >= 0.5",NU); // modify condition to your needs
	// insert your code here
    gsl_set_error_handler_off();

    if (z==0) return 0;
    w = 3./4.*z*z/((1 + 3*NU + 2*NU*NU)*RG*RG);
    G0 =  I0*(3*M_PI)/((1 - 5*gsl_pow_2(NU) + 4*gsl_pow_4(NU))*gsl_pow_2(RG));

	Gz = I0*(4*M_PI)/(NU*z*z)*(
         pow(w,0.5/NU)*gsl_sf_gamma_inc(1.0-0.5/NU,w)
        -pow(w,1.0/NU)*gsl_sf_gamma_inc(1.0-1.0/NU,w)
    );
    return Gz-G0;

// old implementation:    
    
	w = z*z/((1 + 3*NU + 2*NU*NU)*RG*RG);
    G0 =  I0*(3*M_PI)/((1 - 5*gsl_pow_2(NU) + 4*gsl_pow_4(NU))*gsl_pow_2(RG));
    if (z==0) return 0;
	Gz = -((pow(3,1/NU)*pow(4,1 - 1/NU)*M_PI*pow(w,-1 + 1/NU)*gsl_sf_gamma_inc(1 - 1/NU,(3*w)/4.))/
      (NU*(1 + NU)*(1 + 2*NU)*pow(RG,2))) + 
   (pow(3,1/(2.*NU))*pow(4,1 - 1/(2.*NU))*M_PI*pow(w,-1 + 1/(2.*NU))*
      gsl_sf_gamma_inc(1 - 1/(2.*NU),(3*w)/4.))/(NU*(1 + NU)*(1 + 2*NU)*pow(RG,2));
	return Gz-G0;
}

scalar sasfit_ff_gz_ggc_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_gz_ggc_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

