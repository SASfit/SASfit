/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define A	param->p[0]
#define CENTER	param->p[1]
#define W1	fabs(param->p[2])
#define W2	fabs(param->p[3])
#define SIGMA	fabs(param->p[4])
#define C0	param->p[5]
#define PW1 param->p[MAXPAR-1]

scalar smoothed_box(scalar u, sasfit_param *param) {
    return (gsl_sf_erf((PW1 - 2*u)/(2.*sqrt(2)*SIGMA)) + gsl_sf_erf((PW1 + 2*u)/(2.*sqrt(2)*SIGMA)))/(2.*PW1);
}

scalar smoothed_trapez(scalar u, sasfit_param *param) {
    if (fabs(W1-W2)/fabs(W1+W2)<sasfit_eps_get_comp()) {
        return (((exp(-gsl_pow_2(u + W2)/(2.*gsl_pow_2(SIGMA))) + exp((2*u*W2)/gsl_pow_2(SIGMA)-gsl_pow_2(u + W2)/(2.*gsl_pow_2(SIGMA))) - 2*exp((W2*(2*u + W2))/(2.*gsl_pow_2(SIGMA))-gsl_pow_2(u + W2)/(2.*gsl_pow_2(SIGMA))))*gsl_pow_2(SIGMA)) +
              sqrt(M_PI/2.)*SIGMA*(-2*u*gsl_sf_erf(u/(sqrt(2)*SIGMA)) + (-u + W2)*gsl_sf_erf((-u + W2)/(sqrt(2)*SIGMA)) + (u + W2)*gsl_sf_erf((u + W2)/(sqrt(2)*SIGMA))))/gsl_pow_2(W2*sqrt(2*M_PI)*SIGMA);
    } else if (W1>W2) {
        return (SIGMA*(4*(-exp(-gsl_pow_2(2*u + W1 - W2)/(8.*gsl_pow_2(SIGMA))) - exp(-gsl_pow_2(2*u - W1 + W2)/(8.*gsl_pow_2(SIGMA))) + exp(-gsl_pow_2(-2*u + W1 + W2)/(8.*gsl_pow_2(SIGMA))) +
              exp(-gsl_pow_2(2*u + W1 + W2)/(8.*gsl_pow_2(SIGMA))))*SIGMA - sqrt(2*M_PI)*(-2*u + W1 + W2)*gsl_sf_erf((-2*u + W1 - W2)/(2.*sqrt(2)*SIGMA)) + sqrt(2*M_PI)*(-2*u + W1 + W2)*gsl_sf_erf((-2*u + W1 + W2)/(2.*sqrt(2)*SIGMA)) +
              sqrt(2*M_PI)*((-2*u - W1 + W2)*gsl_sf_erf((2*u + W1 - W2)/(2.*sqrt(2)*SIGMA)) - 2*W2*gsl_sf_erf((2*u - W1 + W2)/(2.*sqrt(2)*SIGMA)) + (2*u + W1 + W2)*gsl_sf_erf((2*u + W1 + W2)/(2.*sqrt(2)*SIGMA)))))/(4.*W1*W2*sqrt(2*M_PI)*SIGMA);
    } else {
        return (SIGMA*(4*(-exp(-gsl_pow_2(2*u + W1 - W2)/(8.*gsl_pow_2(SIGMA))) - exp(-gsl_pow_2(2*u - W1 + W2)/(8.*gsl_pow_2(SIGMA))) + exp(-gsl_pow_2(-2*u + W1 + W2)/(8.*gsl_pow_2(SIGMA))) +
              exp(-gsl_pow_2(2*u + W1 + W2)/(8.*gsl_pow_2(SIGMA))))*SIGMA + sqrt(2*M_PI)*(2*u + W1 + W2)*gsl_sf_erf((-2*u + W1 - W2)/(2.*sqrt(2)*SIGMA)) + sqrt(2*M_PI)*(-2*u + W1 + W2)*gsl_sf_erf((-2*u + W1 + W2)/(2.*sqrt(2)*SIGMA)) +
              sqrt(2*M_PI)*((-2*u - W1 + W2)*gsl_sf_erf((2*u + W1 - W2)/(2.*sqrt(2)*SIGMA)) + 2*W1*gsl_sf_erf((2*u - W1 + W2)/(2.*sqrt(2)*SIGMA)) + (2*u + W1 + W2)*gsl_sf_erf((2*u + W1 + W2)/(2.*sqrt(2)*SIGMA)))))/(4.*W1*W2*sqrt(2*M_PI)*SIGMA);
    }
}

scalar sasfit_ff_smoothed_symm_trapez_area(scalar x, sasfit_param * param)
{
    scalar u,p;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	u=x-CENTER;

	p=0;
	if (fabs(fabs(W1-W2)/fabs(W1+W2)-1)<=sasfit_eps_get_comp()) {
        PW1 = GSL_MAX(W1,W2);
        if (fabs(SIGMA)/fabs(W1+W2)<=sasfit_eps_get_comp()) {
            if (fabs(u)<=fabs(W1+W2)/2.) {
                p = 1./fabs(W1+W2);
            } else {
                p = 0;
            }
        } else {
            p = smoothed_box(u,param);
        }
	} else {
        if (fabs(SIGMA)/fabs(W1+W2)<=sasfit_eps_get_comp()) {
            if (W1<W2+2*u&&W1+2*u<W2) {
                p= W1;
            } else if (W1>=W2+2*u&&W1+2*u>=W2) {
                p= W2;
            } else if (fabs(W1+W2)/2. > fabs(u)) {
                p = fabs(W1+W2)/2. - GSL_SIGN(u)*u;
            } else {
                p = 0;
            }
            if (fabs(fabs(W1-W2)/fabs(W1+W2))<=sasfit_eps_get_comp()) {
                p=p/(W1*W1);
            } else {
                p=p/(W1*W2);
            }
        } else {
            p = smoothed_trapez(u,param);
        }
	}

	// insert your code here
	return A*p+C0;
}

scalar sasfit_ff_smoothed_symm_trapez_area_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_smoothed_symm_trapez_area_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

