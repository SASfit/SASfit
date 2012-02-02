/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@pssi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

scalar find_LogNorm_int_range(scalar dim, scalar x0, scalar sigma, scalar *Xstart, scalar *Xend, sasfit_param *param) 
{
	scalar a1,a2,a3,a4, X_0, X_50, X_n, nperc;
	nperc = 0.1;
	a3 = 1.0;
	a4 = fabs(x0);
	a2 = fabs(sigma);
	X_0  = a4*exp(-a2*a2*(a3-2.0*dim));
	X_n  = a4*exp(-a2*a2*(a3-2.0*dim)+sqrt(2.0*a2*a2*log(100.0/nperc)));
	X_50 = a4*exp(-a2*a2*(a3-2.0*dim)+sqrt(2.0*a2*a2*log(100.0/50.0)));

	SASFIT_CHECK_COND((X_50 == 0.0), param, "Can't guess good integration interval");

	*Xstart = 0.0;
	*Xend = X_n;

	if ((X_n-X_0) < X_0)
	{
		*Xend = X_n;
		*Xstart = 2.0*X_0-X_n;
	}
	return 0;
}