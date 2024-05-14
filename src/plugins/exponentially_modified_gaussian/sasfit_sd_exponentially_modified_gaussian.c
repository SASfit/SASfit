/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables
#define N	param->p[0]
#define SIGMA	fabs(param->p[1])
#define MU	param->p[2]
#define TAU	fabs(param->p[3])

scalar CDFnormal(scalar x, scalar mu, scalar sigma) {
    scalar z;
    z = (x-mu)/sigma;
	return 0.5*(1+gsl_sf_erf(z/sqrt(2.)));
}

scalar fpdf1(scalar x, sasfit_param *param) {
    return SIGMA/TAU*sqrt(M_PI/2.0)*exp(0.5*gsl_pow_2(SIGMA/TAU)-(x-MU)/TAU)*gsl_sf_erfc(1./sqrt(2)*(SIGMA/TAU-(x-MU)/SIGMA));
}
scalar fpdf2(scalar x, sasfit_param *param) {
    scalar z;
    z=(x-MU)/SIGMA;
    return exp(-0.5*z*z)*SIGMA/TAU*sqrt(M_PI/2.0)*erfcx(1./sqrt(2.)*(SIGMA/TAU-z));
}
scalar fpdf3(scalar x, sasfit_param *param) {
    scalar z;
    z=(x-MU)/SIGMA;
    return exp(-0.5*z*z)/(1+z*TAU/SIGMA);
}

scalar sasfit_sd_exponentially_modified_gaussian(scalar x, sasfit_param * param)
{
    scalar z, fpdf;
    int choice;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((SIGMA == 0.0), param, "sigma(%lg) == 0",SIGMA); // modify condition to your needs

	// insert your code here
	z=1/sqrt(2.)*(SIGMA/TAU-(x-MU)/SIGMA);
	if (z<0) {
        choice=1;
	} else if (z<=6.71e7) {
	    choice=2;
	} else {
	    choice=3;
	}
	switch (choice) {
	    case 1: fpdf = fpdf1(x,param);
                break;
	    case 2: fpdf = fpdf2(x,param);
                break;
	    default: fpdf = fpdf3(x,param) ;
	}
	return N*fpdf/(SIGMA*sqrt(M_PI/2.)*2.*(1-sasfit_sd_exponentially_modified_gaussian_f(0,param)));
}

scalar sasfit_sd_exponentially_modified_gaussian_f(scalar x, sasfit_param * param)
{
    scalar u,v;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	if (TAU == 0) {
        return CDFnormal(x,MU,SIGMA);
	} else {
	    u = (x-MU)/TAU;
	    v = SIGMA/TAU;
	    return CDFnormal(u,0,v)-exp(-(u+v*v)/(2+log(CDFnormal(u,v*v,v))));
	}
}

scalar sasfit_sd_exponentially_modified_gaussian_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

