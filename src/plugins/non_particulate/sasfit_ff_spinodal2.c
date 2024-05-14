/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define IMAX	param->p[0]
#define QMAX	param->p[1]
#define GAMA	param->p[2]
#define DDD  	param->p[3]
#define Model  	lround(param->p[4])

scalar sasfit_ff_spinodal2(scalar q, sasfit_param * param)
{
    scalar x,a,b,c,d,g,tmp;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((QMAX < 0.0), param, "Qmax(%lg) < 0",QMAX); // modify condition to your needs
	SASFIT_CHECK_COND1((GAMA <= 0.0), param, "GAMA(%lg) <= 0",GAMA); // modify condition to your needs

	// insert your code here

	switch (Model) {
	    case 0: g = GAMA;
                break;
	    case 1: g = 1.0/(4.0*M_PI*GAMA*(1-GAMA));
	            break;
	    case 2: g = 1.0/(4.0*sqrt(M_PI*GAMA)*(1-GAMA));
	            break;
	    case 3: g = 1.0/(2.0*pow(4.0*M_PI/3.,2./3.)*pow(GAMA,1./3.)*(1.0-GAMA));
	            break;
	    default: g = GAMA;
	}
    x = q/QMAX;
    d=DDD;
    b = gsl_pow_2(2*g*(1-d)/(1-gsl_pow_2(g)));
	c = d/(b-d*(1-d));
	a = (1+c)*(1+gsl_pow_2(d)/b);
    sasfit_out("a:%lf, b:%lf, c:%lf\n",a,b,c);
    if (g==1) {
        return IMAX;
    } else {
        tmp = a*gsl_pow_4(x)/(gsl_pow_4(x)+c) * b/(b+gsl_pow_2(x*x-1+d));
    }

	SASFIT_CHECK_COND1((tmp < 0.0), param, "I(Q)(%lg) < 0, choose a value for gamma closer do 1",tmp); // modify condition to your needs
	return tmp*IMAX;
}

scalar sasfit_ff_spinodal2_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_spinodal2_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

