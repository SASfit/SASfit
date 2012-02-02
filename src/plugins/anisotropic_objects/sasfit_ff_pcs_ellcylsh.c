/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define EPSILON	param->p[1]
#define T	param->p[2]
#define ETA_CORE	param->p[3]
#define ETA_SHELL	param->p[4]
#define STA_SOLV	param->p[5]

scalar sasfit_ff_pcs_ellcylsh(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((EPSILON < 0.0), param, "epsilon(%lg) < 0",EPSILON); // modify condition to your needs
	SASFIT_CHECK_COND1((T < 0.0), param, "t(%lg) < 0",T); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA_CORE < 0.0), param, "eta_core(%lg) < 0",ETA_CORE); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA_SHELL < 0.0), param, "eta_shell(%lg) < 0",ETA_SHELL); // modify condition to your needs
	SASFIT_CHECK_COND1((STA_SOLV < 0.0), param, "sta_solv(%lg) < 0",STA_SOLV); // modify condition to your needs

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_pcs_ellcylsh_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_pcs_ellcylsh_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

