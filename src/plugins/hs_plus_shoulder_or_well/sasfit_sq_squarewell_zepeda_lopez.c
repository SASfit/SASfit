
/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/sasfit_sq_squarewell_zepeda_lopez.h"
#include "include/squarewell_common.h"
#include <math.h>
#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define RHS	param->p[0]
#define FP	param->p[1]
#define EPSI_KT	param->p[2]
#define LAMBDA	param->p[3]

double sw_zepeda_lopez_S(double Q, double sigma, double eta, double eps_kT, double lam)
{
    /* NOTE ON UNITS: the paper's formulas are written entirely in reduced
     * units (k*=Q*sigma dimensionless), so the density factor multiplying
     * c0_hat and the perturbation term must be the REDUCED density
     * rho*=rho*sigma^3=6*eta/pi, not the dimensional number density. */
    double rho_star = 6.0 * eta / M_PI;

    double phi_vw = eta * (1.0 - eta / 16.0);
    double sigma_vw = sigma * cbrt(phi_vw / eta);

    double kstar = Q * sigma_vw;
    double c0 = sw_c0_hat(kstar, phi_vw);

    double x = exp(eps_kT) - 1.0;
    double kp = Q * sigma;
    double pert = sw_fhat_mayer(kp, x, lam);

    /* NOTE (bug fix): c_total = c0 + pert, so S^-1 = 1 - rho*c0 - rho*pert.
     * An earlier version used "+rho_star*pert", which flips the sign of
     * attraction's effect on S(Q) -- caught via an independent low-density
     * virial check (see squarewell_zepeda_lopez.py's Python twin for details). */
    return 1.0 / (1.0 - rho_star * c0 - rho_star * pert);
}


scalar sasfit_sq_square_well_zepeda_lopez(scalar q, sasfit_param * param)
{

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

    SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((RHS < 0.0), param, "RHS(%lg) < 0",RHS); // modify condition to your needs
	SASFIT_CHECK_COND1((FP < 0.0), param, "fp(%lg) < 0",FP); // modify condition to your needs
	SASFIT_CHECK_COND1((FP >= 1.0), param, "fp(%lg) >= 1",FP); // modify condition to your needs
	SASFIT_CHECK_COND1((LAMBDA <= 1.0), param, "lambda(%lg) <= 1",LAMBDA); // modify condition to your needs

	return sw_zepeda_lopez_S(q, 2*RHS, FP, EPSI_KT, LAMBDA);
}

scalar sasfit_sq_square_well_zepeda_lopez_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_square_well_zepeda_lopez_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}
