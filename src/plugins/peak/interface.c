/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *   2th of August 2016
 */

#ifdef MAKE_SASFIT_PLUGIN
#include "include/private.h"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN(74)
SASFIT_PLUGIN_EXP_ADD( peak_beta_amplitude )
SASFIT_PLUGIN_EXP_ADD( peak_beta_area )
SASFIT_PLUGIN_EXP_ADD( peak_chi_squared_amplitude )
SASFIT_PLUGIN_EXP_ADD( peak_chi_squared_area )
SASFIT_PLUGIN_EXP_ADD( peak_erfc_amplitude )
SASFIT_PLUGIN_EXP_ADD( peak_erfc_area )
SASFIT_PLUGIN_EXP_ADD( peak_error_amplitude )
SASFIT_PLUGIN_EXP_ADD( peak_error_area )
SASFIT_PLUGIN_EXP_ADD( peak_exponentially_modified_gaussian_amplitude )
SASFIT_PLUGIN_EXP_ADD( peak_exponentially_modified_gaussian_area )
SASFIT_PLUGIN_EXP_ADD( peak_extreme_value_amplitude )
SASFIT_PLUGIN_EXP_ADD( peak_extreme_value_area )
SASFIT_PLUGIN_EXP_ADD( peak_f_variance_amplitude )
SASFIT_PLUGIN_EXP_ADD( peak_f_variance_area )
SASFIT_PLUGIN_EXP_ADD( peak_fatique_life_amplitude )
SASFIT_PLUGIN_EXP_ADD( peak_fatique_life_area )
SASFIT_PLUGIN_EXP_ADD( peak_gamma_amplitude )
SASFIT_PLUGIN_EXP_ADD( peak_gamma_area )
SASFIT_PLUGIN_EXP_ADD( peak_gaussian_amplitude )
SASFIT_PLUGIN_EXP_ADD( peak_gaussian_area )
SASFIT_PLUGIN_EXP_ADD( peak_gaussian_lorentzian_cross_product_amplitude )
SASFIT_PLUGIN_EXP_ADD( peak_gaussian_lorentzian_cross_product_area )
SASFIT_PLUGIN_EXP_ADD( peak_gaussian_lorentzian_sum_amplitude )
SASFIT_PLUGIN_EXP_ADD( peak_gaussian_lorentzian_sum_area )
SASFIT_PLUGIN_EXP_ADD( peak_generalized_gaussian1_amplitude )
SASFIT_PLUGIN_EXP_ADD( peak_generalized_gaussian1_area )
SASFIT_PLUGIN_EXP_ADD( peak_generalized_gaussian2_amplitude )
SASFIT_PLUGIN_EXP_ADD( peak_generalized_gaussian2_area )
SASFIT_PLUGIN_EXP_ADD( peak_generalized_maxwell_amplitude )
SASFIT_PLUGIN_EXP_ADD( peak_generalized_maxwell_area )
SASFIT_PLUGIN_EXP_ADD( peak_gex_amplitude )
SASFIT_PLUGIN_EXP_ADD( peak_gex_area )
SASFIT_PLUGIN_EXP_ADD( peak_giddings_amplitude )
SASFIT_PLUGIN_EXP_ADD( peak_giddings_area )
SASFIT_PLUGIN_EXP_ADD( peak_haarhoff_vander_linde_area )
SASFIT_PLUGIN_EXP_ADD( peak_half_gaussian_modified_gaussian_area )
SASFIT_PLUGIN_EXP_ADD( peak_inverted_gamma_amplitude )
SASFIT_PLUGIN_EXP_ADD( peak_inverted_gamma_area )
SASFIT_PLUGIN_EXP_ADD( peak_kumaraswamy_amplitude )
SASFIT_PLUGIN_EXP_ADD( peak_kumaraswamy_area )
SASFIT_PLUGIN_EXP_ADD( peak_laplace_amplitude )
SASFIT_PLUGIN_EXP_ADD( peak_laplace_area )
SASFIT_PLUGIN_EXP_ADD( peak_logistic_amplitude )
SASFIT_PLUGIN_EXP_ADD( peak_logistic_area )
SASFIT_PLUGIN_EXP_ADD( peak_logit_amplitude )
SASFIT_PLUGIN_EXP_ADD( peak_logit_area )
SASFIT_PLUGIN_EXP_ADD( peak_loglogistic_amplitude )
SASFIT_PLUGIN_EXP_ADD( peak_loglogistic_area )
SASFIT_PLUGIN_EXP_ADD( peak_lognormal4parameter_amplitude )
SASFIT_PLUGIN_EXP_ADD( peak_lognormal4parameter_area )
SASFIT_PLUGIN_EXP_ADD( peak_lognormal_amplitude )
SASFIT_PLUGIN_EXP_ADD( peak_lognormal_area )
SASFIT_PLUGIN_EXP_ADD( peak_lorentzian_amplitude )
SASFIT_PLUGIN_EXP_ADD( peak_lorentzian_area )
SASFIT_PLUGIN_EXP_ADD( peak_maxwell_amplitude )
SASFIT_PLUGIN_EXP_ADD( peak_maxwell_area )
SASFIT_PLUGIN_EXP_ADD( peak_pearsonIV_amplitude )
SASFIT_PLUGIN_EXP_ADD( peak_pearsonIV_area )
SASFIT_PLUGIN_EXP_ADD( peak_pearsonVII_amplitude )
SASFIT_PLUGIN_EXP_ADD( peak_pearsonVII_area )
SASFIT_PLUGIN_EXP_ADD( peak_powerlognormal_area )
SASFIT_PLUGIN_EXP_ADD( peak_powernormal_area )
SASFIT_PLUGIN_EXP_ADD( peak_pulse_amplitude )
SASFIT_PLUGIN_EXP_ADD( peak_pulse_area )
SASFIT_PLUGIN_EXP_ADD( peak_pulse_with_2nd_width_amplitude )
SASFIT_PLUGIN_EXP_ADD( peak_pulse_with_2nd_width_area )
SASFIT_PLUGIN_EXP_ADD( peak_pulse_with_power_term_amplitude )
SASFIT_PLUGIN_EXP_ADD( peak_pulse_with_power_term_area )
SASFIT_PLUGIN_EXP_ADD( peak_student_t_amplitude )
SASFIT_PLUGIN_EXP_ADD( peak_student_t_area )
SASFIT_PLUGIN_EXP_ADD( peak_voigt_amplitude )
SASFIT_PLUGIN_EXP_ADD( peak_voigt_area )
SASFIT_PLUGIN_EXP_ADD( peak_weibull_amplitude )
SASFIT_PLUGIN_EXP_ADD( peak_weibull_area )

SASFIT_PLUGIN_EXP_END

// import functions from other plugins
SASFIT_PLUGIN_IMP_NONE

SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif

