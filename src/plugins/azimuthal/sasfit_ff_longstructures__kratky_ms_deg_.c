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

scalar K_MS_anal(scalar x, sasfit_param * param) {
    scalar u, norm,w;
	if (KAPPA < 0) {
		u = sqrt(-KAPPA);
		norm = sqrt(M_PI)*gsl_sf_erf(u)/u;
	} else if (KAPPA == 0.0) {
		norm = 2;
	} else {
		u = sqrt(KAPPA);
		norm = 2.0*exp(KAPPA)*gsl_sf_dawson(u)/u; // maple: `assuming`([convert(int(exp(ksppa*cos(psi)^2)*sin(psi), psi = 0 .. Pi), dawson)], [kappa > 0])
	}
	w = KAPPA*gsl_pow_2(cos(x))/2.;
	return exp(w)*gsl_sf_bessel_I0(w)/(8.0*norm);
}

scalar K_MS_deg_Kernel(scalar x, sasfit_param * param) {
    return MaierSaupeODF(x,KAPPA)*KratkyKernel(x,DPSI);
}

scalar K_MS_deg_intpsi(scalar x,sasfit_param *param) {
    DPSI = x;
    return sasfit_integrate(DPSI,M_PI_2,&K_MS_deg_Kernel,param);
}
scalar sasfit_ff_longstructures__kratky_ms_deg_(scalar psi, sasfit_param * param)
{
    static scalar kappa_old=-99991999999.99;
    static scalar NormIdpsi=1.0;
    scalar Idpsi;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

/*
	Idpsi = K_MS_anal((psi-DELTA)*M_PI/180.,param)
	NormIdpsi=sasfit_integrate(0,M_PI_2,&K_MS_anal,param)/M_PI_2;
	return A+B*Idpsi/NormIdpsi;
*/
    DPSI=psi_quarter_deg(psi-90-DELTA)*M_PI/180.;
	// insert your code here
	if (fabs(M_PI_2-DPSI) < 1e-6*M_PI_2) DPSI=(1.0-1e-6)*M_PI_2;
	Idpsi= sasfit_integrate(DPSI,M_PI_2,&K_MS_deg_Kernel,param);
	if (KAPPA!=kappa_old) {
        NormIdpsi=sasfit_integrate(0,M_PI_2,&K_MS_deg_intpsi,param)/M_PI_2;
        kappa_old = KAPPA;
	}

	return A+B*Idpsi/NormIdpsi;
}

scalar sasfit_ff_longstructures__kratky_ms_deg__f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_longstructures__kratky_ms_deg__v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

