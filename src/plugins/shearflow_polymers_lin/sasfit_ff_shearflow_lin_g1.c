/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
scalar sasfit_ff_shearflow_lin_g1_phi(scalar phi, sasfit_param * param) {
    PHI=phi;
    calc_QFVG(param);
    return sasfit_ff_shearflow_lin_core(param);
}

scalar sasfit_ff_shearflow_lin_g1(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((I0 < 0.0), param, "I0(%lg) < 0",I0); // modify condition to your needs

	// insert your code here
	QMOD = q;
	VARPHI = sasfit_param_override_get_psi(VARPHI_DEG*M_PI/180.);
    THETA = 0;
    PSI = 0;
	return I0*sasfit_integrate((PHI0_DEG-DELTA_PHI_DEG/2.)*M_PI/180.,
                            (PHI0_DEG+DELTA_PHI_DEG/2.)*M_PI/180.,
                            &sasfit_ff_shearflow_lin_g1_phi,
                            param)/ (DELTA_PHI_DEG*M_PI/180.);
}

scalar sasfit_ff_shearflow_lin_g1_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_shearflow_lin_g1_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

