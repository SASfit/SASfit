/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define N	param->p[0]
#define D	param->p[1]
#define ETA	param->p[2]
#define NU	param->p[3]
#define Q   param->p[MAXPAR-1]
#define SIGMA_SD    param->p[MAXPAR-2]
#define NMEAN    param->p[MAXPAR-3]
#define NSIGN    param->p[MAXPAR-4]

scalar modifiedcaille_core(scalar x, void * pam)
{
    sasfit_param * param;
	param = (sasfit_param *) pam;

    N = NSIGN*x;
    return 1./(SIGMA_SD*sqrt(2*M_PI))*exp(-gsl_pow_2((x-NMEAN)/SIGMA_SD)/2.)*(sasfit_sq_modifiedcaille(Q,param)-NU);
}

scalar sasfit_sq_modifiedcaille_sd(scalar q, sasfit_param * param)
{
    scalar *aw, res,err,eps_nriq;
    int lenaw=4000;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((N < 0.0), param, "N(%lg) < 0",N); // modify condition to your needs
	SASFIT_CHECK_COND1((D < 0.0), param, "d(%lg) < 0",D); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA < 0.0), param, "eta(%lg) < 0",ETA); // modify condition to your needs
	SASFIT_CHECK_COND1((NU < 0.0), param, "Nu(%lg) < 0",NU); // modify condition to your needs

	// insert your code here
    if (fabs(N)>=5) {
        SIGMA_SD = sqrt(fabs(N));
    } else {
        SIGMA_SD = 0.5*(fabs(N)-1);
    }
    NMEAN = fabs(N);
    Q = q;
    NSIGN = GSL_SIGN(N);

	aw = (scalar *)malloc((lenaw)*sizeof(scalar));
	eps_nriq=sasfit_eps_get_nriq();
    sasfit_intdeoini(lenaw, GSL_DBL_MIN, eps_nriq, aw);
    sasfit_intdeo(&modifiedcaille_core,NMEAN-2*SIGMA_SD,Q*D, aw, &res, &err, param);
    free(aw);
    return res+NU;
//    return sasfit_integrate(NMEAN-2*SIGMA_SD,NMEAN+2*SIGMA_SD,&modifiedcaille_core,param)+NU;
}

scalar sasfit_sq_modifiedcaille_sd_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_modifiedcaille_sd_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

