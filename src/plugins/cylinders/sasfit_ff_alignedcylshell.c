/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define DR	param->p[1]
#define DUMMY	param->p[2]
#define L	param->p[3]
#define ETA_CORE	param->p[4]
#define ETA_SHELL	param->p[5]
#define ETA_SOLV	param->p[6]
#define PSI	param->p[7]
#define THETA	param->p[8]
#define PHI	param->p[9]

scalar sasfit_ff_alignedcylshell(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((DR < 0.0), param, "DR(%lg) < 0",DR); // modify condition to your needs
	SASFIT_CHECK_COND1((DUMMY < 0.0), param, "dummy(%lg) < 0",DUMMY); // modify condition to your needs
	SASFIT_CHECK_COND1((L < 0.0), param, "L(%lg) < 0",L); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA_CORE < 0.0), param, "eta_core(%lg) < 0",ETA_CORE); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA_SHELL < 0.0), param, "eta_shell(%lg) < 0",ETA_SHELL); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA_SOLV < 0.0), param, "eta_solv(%lg) < 0",ETA_SOLV); // modify condition to your needs
	SASFIT_CHECK_COND1((PSI < 0.0), param, "psi(%lg) < 0",PSI); // modify condition to your needs
	SASFIT_CHECK_COND1((THETA < 0.0), param, "theta(%lg) < 0",THETA); // modify condition to your needs
	SASFIT_CHECK_COND1((PHI < 0.0), param, "phi(%lg) < 0",PHI); // modify condition to your needs

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_alignedcylshell_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_alignedcylshell_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

