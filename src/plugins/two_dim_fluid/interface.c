/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(4)
SASFIT_PLUGIN_EXP_ADD( sq__d_hard_disks__rosenfeld_random )
SASFIT_PLUGIN_EXP_ADD( sq__d_hard_disks__guo_random )
SASFIT_PLUGIN_EXP_ADD( sq__d_hard_disks__rosenfeld_aligned )
SASFIT_PLUGIN_EXP_ADD( sq__d_hard_disks__guo_aligned )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_NONE

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

