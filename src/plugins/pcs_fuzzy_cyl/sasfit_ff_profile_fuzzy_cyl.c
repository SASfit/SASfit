/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	fabs(param->p[0])
#define DUMMY1	param->p[1]
#define SIGMA	fabs(param->p[2])
#define DUMMY4	param->p[4]
#define DUMMY5	param->p[5]
#define DUMMY6	param->p[6]
#define ETA_CYL	param->p[7]
#define ETA_SOL	param->p[8]
#define DETA ETA_CYL-ETA_SOL

#define X   param->p[MAXPAR-1]

scalar profile_fuzzy_cyl_OOURA(scalar Q, void *pam) {
    sasfit_param * param;
    param = (sasfit_param *) pam;
    return sasfit_ff_fuzzy_cyl_f(Q,param)*gsl_sf_bessel_J0(Q*X)*Q;
}

scalar sasfit_ff_profile_fuzzy_cyl(scalar r, sasfit_param * param)
{
    int lenaw = 4000;
    scalar *aw;
    scalar res, err;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs

	// insert your code here
	X = fabs(r);
	if (SIGMA <= R/10) {
	    if (SIGMA == 0) {
            if (X<=R) {
                return ETA_CYL;
            } else {
                return ETA_SOL;
            }
	    }
	    return 0.5*(1-gsl_sf_erf((X-R)/(M_SQRT2*SIGMA))) * DETA + ETA_SOL;
	} else {
        aw = (scalar *)malloc((lenaw)*sizeof(scalar));
        sasfit_intdeoini(lenaw, 0, sasfit_eps_get_nriq(), aw);
        sasfit_intdeo(&profile_fuzzy_cyl_OOURA, 0.0, X<R?R:X, aw, &res, &err,param);
        free(aw);
        return res/(2*M_PI)*DETA +ETA_SOL;
	}
}

scalar sasfit_ff_profile_fuzzy_cyl_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0;
}

scalar sasfit_ff_profile_fuzzy_cyl_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0;
}

