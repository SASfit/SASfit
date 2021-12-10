/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(9)
SASFIT_PLUGIN_EXP_ADD( sd_g_n_pdf )
SASFIT_PLUGIN_EXP_ADD( sd_g_l_pdf )
SASFIT_PLUGIN_EXP_ADD( sd_g_c_pdf )
SASFIT_PLUGIN_EXP_ADD( sd_g_n_pdf_s )
SASFIT_PLUGIN_EXP_ADD( sd_g_l_pdf_s )
SASFIT_PLUGIN_EXP_ADD( sd_g_c_pdf_s )
SASFIT_PLUGIN_EXP_ADD( sd_g_n_pdf_b )
SASFIT_PLUGIN_EXP_ADD( sd_g_l_pdf_b )
SASFIT_PLUGIN_EXP_ADD( sd_g_c_pdf_b )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_NONE

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

