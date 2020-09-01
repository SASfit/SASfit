/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define A	param->p[0]
#define CENTER	param->p[1]
#define WP	fabs(param->p[2])
#define WE	fabs(param->p[3])
#define DUMMY	param->p[4]
#define C0	param->p[5]

#define ALPHA	(CENTER-WP/2-WE/2)
#define BETA	(CENTER-WP/2)
#define GAMMA	(CENTER+WP/2)
#define DELTA	(CENTER+WP/2+WE/2)

scalar sasfit_ff_symm__trapez__area(scalar x, sasfit_param * param)
{
    scalar u,p;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	u=2/(DELTA+GAMMA-BETA-ALPHA);

	p=0;
    if (ALPHA<=x && x<BETA) {
        p=(x-ALPHA)/(BETA-ALPHA)*u;
	} else if (BETA<=x && x<GAMMA) {
	    p=u;
	} else if (GAMMA<=x && x<DELTA) {
	    p=u*(DELTA-x)/(DELTA-GAMMA);
	}
	// insert your code here
	return A*p+C0;
}

scalar sasfit_ff_symm__trapez__area_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_symm__trapez__area_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

