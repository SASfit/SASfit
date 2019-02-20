/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define A	param->p[0]
#define B	param->p[1]
#define KAPPA	param->p[2]
#define DELTA	param->p[3]
#define DPSI    param->p[MAXPAR-1]

scalar LN_MS_deg_Kernel(scalar x, sasfit_param * param) {
    return MaierSaupeODF(x,KAPPA)*LNKernel(x,DPSI);
}

scalar LN_MS_deg_intpsi(scalar x,sasfit_param *param) {
    DPSI = x;
    return sasfit_integrate(DPSI,M_PI_2,&LN_MS_deg_Kernel,param);
}
scalar sasfit_ff_longstructures__ln_ms_deg_(scalar psi, sasfit_param * param)
{
    static scalar kappa_old=-99991999999.99;
    static scalar NormIdpsi=1.0;
    scalar Idpsi;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid
    DPSI=psi_quarter_deg(psi-90-DELTA)*M_PI/180.;
	// insert your code here
	if (fabs(M_PI_2-DPSI) < 1e-6*M_PI_2) DPSI=(1.0-1e-6)*M_PI_2;
	Idpsi= sasfit_integrate(DPSI,M_PI_2,&LN_MS_deg_Kernel,param);
	if (KAPPA!=kappa_old) {
        NormIdpsi=sasfit_integrate(0,M_PI_2,&LN_MS_deg_intpsi,param)/M_PI_2;
        kappa_old = KAPPA;
	}

	return A+B*Idpsi/NormIdpsi;
}

scalar sasfit_ff_longstructures__ln_ms_deg__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_longstructures__ln_ms_deg__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

