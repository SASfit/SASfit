/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define L	param->p[1]
#define DR	param->p[2]
#define ETAC	param->p[3]
#define ETASHI	param->p[4]
#define ETASHO	param->p[5]
#define ETASOL	param->p[6]

scalar sasfit_ff_profile_linsh_cyl(scalar r, sasfit_param * param)
{
    scalar m, b;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((DR < 0.0), param, "DR(%lg) < 0",DR); // modify condition to your needs
	SASFIT_CHECK_COND1((L < 0.0), param, "L(%lg) < 0",L); // modify condition to your needs

	// insert your code here
	if (fabs(r)<R) {
        return ETAC;
    }
	if (DR==0) {
        return ETASOL;
	}
    m=(ETASHO-ETASHI)/DR;
    b=-m*R+ETASHI;
    if (fabs(r)<R+DR) {
        return m*fabs(r)+b;
    }
	return ETASOL;
}

scalar sasfit_ff_profile_linsh_cyl_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0;
}

scalar sasfit_ff_profile_linsh_cyl_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0;
}

