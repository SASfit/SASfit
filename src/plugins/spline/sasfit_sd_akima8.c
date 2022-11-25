/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables
#define XMIN	param->p[0]
#define XMAX	param->p[1]
#define Y1	param->p[2]
#define Y2	param->p[3]
#define Y3	param->p[4]
#define Y4	param->p[5]
#define Y5	param->p[6]
#define Y6	param->p[7]
#define Y7	param->p[8]
#define Y8	param->p[9]

scalar sasfit_sd_akima8(scalar x, sasfit_param * param)
{
	scalar tmp, xcs[10], ycs[10];
	int i;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	if (x < XMIN) return 0;
	if (x > XMAX) return 0;

	if (XMIN > XMAX) {
		tmp = XMAX;
		XMAX = XMIN;
		XMIN = tmp;
	}
	xcs[0]=XMIN;
	ycs[0]=0;
	xcs[9]=XMAX;
	ycs[9]=0;
	for (i=1; i<=8; i++) {
		xcs[i] = XMIN+i*(XMAX-XMIN)/(8.0+1.0);
		ycs[i] = param->p[1+i];
	}
	gsl_spline_init(sdakima8_T, xcs, ycs, 10);

	return gsl_spline_eval (sdakima8_T, x, acc_cspline);
}

scalar sasfit_sd_akima8_f(scalar x, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sd_akima8_v(scalar x, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

