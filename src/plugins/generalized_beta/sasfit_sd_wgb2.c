/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables
#define N	param->p[0]
#define A	fabs(param->p[1])
#define B	fabs(param->p[2])
#define K	param->p[3]
#define P	fabs(param->p[4])
#define Q	fabs(param->p[5])

scalar sasfit_sd_wgb2_mode(sasfit_param * param)
{
    return B*pow((A*P+K-1)/(A*Q-K+1),1/A);
}
scalar sasfit_sd_wgb2_mean(sasfit_param * param)
{
    gsl_sf_result Bpq1,Bpq2;
	gsl_set_error_handler_off();
	gsl_sf_beta_e(P+(K+1)/A,Q-(K+1)/A,&Bpq1);
	gsl_sf_beta_e(P+K/A,Q-K/A,&Bpq2);
    return B*Bpq1.val/Bpq2.val;
}

scalar sasfit_sd_wgb2_variance(sasfit_param * param)
{
    gsl_sf_result Bpq1,Bpq2;
	gsl_set_error_handler_off();
	gsl_sf_beta_e(P+(K+2)/A,Q-(K+2)/A,&Bpq1);
	gsl_sf_beta_e(P+K/A,Q-K/A,&Bpq2);
    return B*B*Bpq1.val/Bpq2.val
             - gsl_pow_2(sasfit_sd_wgb2_mean(param));
}

scalar sasfit_sd_wgb2(scalar x, sasfit_param * param)
{
    /*
     *   THIS IS THE PROBABILITY DISTRIBUTION FUNCTION
     */
    gsl_sf_result Bpq;
    int ierr;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((A == 0.0), param, "a(%lg) == 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((B == 0.0), param, "b(%lg) == 0",B); // modify condition to your needs
	SASFIT_CHECK_COND1((K <= -A*P), param, "k <= -ap(%lg)",-A*P); // modify condition to your needs
	SASFIT_CHECK_COND1((K >= A*Q), param, "k >= aq(%lg)",A*Q); // modify condition to your needs
	SASFIT_CHECK_COND1((P == 0.0), param, "p(%lg) == 0",P); // modify condition to your needs
	SASFIT_CHECK_COND1((Q == 0.0), param, "q(%lg) == 0",Q); // modify condition to your needs

	// insert your code here
	if (x<=0) return 0.0;
	gsl_set_error_handler_off();
	ierr = gsl_sf_beta_e(P+K/A,Q-K/A,&Bpq);
	if (ierr) {
        sasfit_err("%s B(%lf,%lf)+-%lf\n", gsl_strerror (ierr),P,Q,Bpq.err);
        return 0;
	}
	return N*fabs(A)*pow(x,A*P+K-1)/(pow(B,A*P+K)*Bpq.val*pow(1+pow(x/B,A),P+Q));
}

scalar sasfit_sd_wgb2_f(scalar x, sasfit_param * param)
{
    /*
     *   THIS IS THE CUMMULITATIVE DISTRIBUTION FUNCTION
     */
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	gsl_set_error_handler_off();
	if (x<0) return 0;
	return 1.0-gsl_sf_beta_inc(P+K/A,Q-K/A,1./(1+pow(x/B,A)));
}

scalar sasfit_sd_wgb2_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

