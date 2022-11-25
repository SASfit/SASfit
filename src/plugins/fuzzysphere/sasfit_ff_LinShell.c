/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables

#define R1 param->p[0]

scalar sasfit_ff_linshell(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param);

	return gsl_pow_2(sasfit_ff_linshell_f(q,param));
}

scalar sasfit_ff_linshell_f(scalar q, sasfit_param * param)
{
	// insert your code here

	scalar A, t1, t2, shlin, m, b, eta_sh_in, eta_sh_out;
	scalar DR, eta_core, eta_shell, x_in_solvent, x_out_solvent, eta_solvent;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 7, EMPTY, &DR, &eta_core, &eta_shell, &x_in_solvent, &x_out_solvent, &eta_solvent);

	eta_sh_in  = (1.0- x_in_solvent)*eta_shell +  x_in_solvent*eta_solvent-eta_solvent;
	eta_sh_out = (1.0-x_out_solvent)*eta_shell + x_out_solvent*eta_solvent-eta_solvent;

	A = q*R1;
	shlin = 4.0*M_PI*(eta_core-eta_solvent)*(sin(A)-A*cos(A))/pow(q,3.0);

	if (DR > 0)
	{
		m = (eta_sh_out-eta_sh_in)/DR;
		b = -m*R1+eta_sh_in;

		t1 = 4.0*M_PI*m*(2.0*cos(A)+2.0*A*sin(A)-A*A*cos(A))/pow(q,4);
		t2 = 4.0*M_PI*b*(sin(A)-A*cos(A))/pow(q,3.0);
		shlin = shlin-t1-t2;

		A = q*(R1+DR);
		t1 = 4.0*M_PI*m*(2.0*cos(A)+2.0*A*sin(A)-A*A*cos(A))/pow(q,4);
		t2 = 4.0*M_PI*b*(sin(A)-A*cos(A))/pow(q,3.0);
		shlin = shlin+t1+t2;
	}

	return shlin;
}

scalar sasfit_ff_linshell_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}
