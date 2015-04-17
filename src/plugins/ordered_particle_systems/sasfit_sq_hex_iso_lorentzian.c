/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define A	param->p[0]

scalar sasfit_sq_hex_iso_lorentzian(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((A < 0.0), param, "a(%lg) < 0",A); // modify condition to your needs

	// insert your code here
	PEAKSHAPE = LORENTZIANPEAK;
	ORDERTYPE = HEX;

	QMODVAL     = q;
	ADVAL       = PAD;
	DELTAVAL    = PDELTA;
	CLVAL       = PCL;

	return Lattice_Factor_iso(&ospParameter,param)*G(&ospParameter,param)+(1.0-G(&ospParameter,param));
}

scalar sasfit_sq_hex_iso_lorentzian_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_hex_iso_lorentzian_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here

	return 0.0;
}

