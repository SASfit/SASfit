/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"
#include "include/2Y_cpoly.h"
#include "include/2Y_OneYukawa.h"
#include "include/2Y_PairCorrelation.h"
#include "include/2Y_TwoYukawa.h"
#include "include/2Y_utility.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(2)
SASFIT_PLUGIN_EXP_ADD( sq_one_yukawa )
SASFIT_PLUGIN_EXP_ADD( sq_two_yukawa )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_NONE

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

