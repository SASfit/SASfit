/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(48)
SASFIT_PLUGIN_EXP_ADD( sq_sc_iso_lorentzian )
SASFIT_PLUGIN_EXP_ADD( sq_sc_iso_gaussian )
SASFIT_PLUGIN_EXP_ADD( sq_sc_iso_pearson )

SASFIT_PLUGIN_EXP_ADD( sq_sc_aniso_lorentzian )
SASFIT_PLUGIN_EXP_ADD( sq_sc_aniso_gaussian )
SASFIT_PLUGIN_EXP_ADD( sq_sc_aniso_pearson )


SASFIT_PLUGIN_EXP_ADD( sq_bcc_iso_lorentzian )
SASFIT_PLUGIN_EXP_ADD( sq_bcc_iso_gaussian )
SASFIT_PLUGIN_EXP_ADD( sq_bcc_iso_pearson )

SASFIT_PLUGIN_EXP_ADD( sq_bcc_aniso_lorentzian )
SASFIT_PLUGIN_EXP_ADD( sq_bcc_aniso_gaussian )
SASFIT_PLUGIN_EXP_ADD( sq_bcc_aniso_pearson )


SASFIT_PLUGIN_EXP_ADD( sq_bct_iso_lorentzian )
SASFIT_PLUGIN_EXP_ADD( sq_bct_iso_gaussian )
SASFIT_PLUGIN_EXP_ADD( sq_bct_iso_pearson )

SASFIT_PLUGIN_EXP_ADD( sq_bct_aniso_lorentzian )
SASFIT_PLUGIN_EXP_ADD( sq_bct_aniso_gaussian )
SASFIT_PLUGIN_EXP_ADD( sq_bct_aniso_pearson )


SASFIT_PLUGIN_EXP_ADD( sq_fcc_iso_lorentzian )
SASFIT_PLUGIN_EXP_ADD( sq_fcc_iso_gaussian )
SASFIT_PLUGIN_EXP_ADD( sq_fcc_iso_pearson )

SASFIT_PLUGIN_EXP_ADD( sq_fcc_aniso_lorentzian )
SASFIT_PLUGIN_EXP_ADD( sq_fcc_aniso_gaussian )
SASFIT_PLUGIN_EXP_ADD( sq_fcc_aniso_pearson )


SASFIT_PLUGIN_EXP_ADD( sq_hcp_iso_lorentzian )
SASFIT_PLUGIN_EXP_ADD( sq_hcp_iso_gaussian )
SASFIT_PLUGIN_EXP_ADD( sq_hcp_iso_pearson )

SASFIT_PLUGIN_EXP_ADD( sq_hcp_aniso_lorentzian )
SASFIT_PLUGIN_EXP_ADD( sq_hcp_aniso_gaussian )
SASFIT_PLUGIN_EXP_ADD( sq_hcp_aniso_pearson )


SASFIT_PLUGIN_EXP_ADD( sq_hex_iso_lorentzian )
SASFIT_PLUGIN_EXP_ADD( sq_hex_iso_gaussian )
SASFIT_PLUGIN_EXP_ADD( sq_hex_iso_pearson )

SASFIT_PLUGIN_EXP_ADD( sq_hex_aniso_lorentzian )
SASFIT_PLUGIN_EXP_ADD( sq_hex_aniso_gaussian )
SASFIT_PLUGIN_EXP_ADD( sq_hex_aniso_pearson )


SASFIT_PLUGIN_EXP_ADD( sq_lam_iso_lorentzian )
SASFIT_PLUGIN_EXP_ADD( sq_lam_iso_gaussian )
SASFIT_PLUGIN_EXP_ADD( sq_lam_iso_pearson )

SASFIT_PLUGIN_EXP_ADD( sq_lam_aniso_lorentzian )
SASFIT_PLUGIN_EXP_ADD( sq_lam_aniso_gaussian )
SASFIT_PLUGIN_EXP_ADD( sq_lam_aniso_pearson )


SASFIT_PLUGIN_EXP_ADD( sq_crec_iso_lorentzian )
SASFIT_PLUGIN_EXP_ADD( sq_crec_iso_gaussian )
SASFIT_PLUGIN_EXP_ADD( sq_crec_iso_pearson )

SASFIT_PLUGIN_EXP_ADD( sq_crec_aniso_lorentzian )
SASFIT_PLUGIN_EXP_ADD( sq_crec_aniso_gaussian )
SASFIT_PLUGIN_EXP_ADD( sq_crec_aniso_pearson )


SASFIT_PLUGIN_EXP_ADD( sq_sq_iso_lorentzian )
SASFIT_PLUGIN_EXP_ADD( sq_sq_iso_gaussian )
SASFIT_PLUGIN_EXP_ADD( sq_sq_iso_pearson )

SASFIT_PLUGIN_EXP_ADD( sq_sq_aniso_lorentzian )
SASFIT_PLUGIN_EXP_ADD( sq_sq_aniso_gaussian )
SASFIT_PLUGIN_EXP_ADD( sq_sq_aniso_pearson )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_NONE

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

