/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define DR	param->p[1]
#define L	param->p[2]
#define ETA_C	param->p[3]
#define ETA_SH	param->p[4]

scalar sasfit_ff_dumbbellshell(scalar q, sasfit_param * param)
{
	scalar A, R12, ftmp, Kval1, Kval2;

	SASFIT_ASSERT_PTR(param);

	Kval1 = sphere_f(q,R+DR,ETA_SH);
	Kval2 = sphere_f(q,R, ETA_SH-ETA_C);

	R12 = 2.0*(R+DR)+L;
	//A = K(interp,q,(R+DR),eta2,error) - K(interp,q,R,eta2-eta1,error);
	A = Kval1 - Kval2;

	if (R12 == 0)
	{
		ftmp = 2.0;
	} else {
		ftmp = (1.0+sin(q*R12)/(q*R12));
	}
	return A*A*2.0*ftmp;
}

scalar sasfit_ff_dumbbellshell_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_dumbbellshell_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

