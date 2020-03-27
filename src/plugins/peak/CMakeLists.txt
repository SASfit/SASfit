cmake_minimum_required(VERSION 3.0)

# the project name
set(PRJ_NAME sasfit_peak)

# required external libs which are installed on the system
set(LIBS_EXT
	GSL
)

# source files
set(SOURCE_${PRJ_NAME}
	sasfit_peak_beta_amplitude.c
	sasfit_peak_beta_area.c
	sasfit_peak_chi_squared_amplitude.c
	sasfit_peak_chi_squared_area.c
	sasfit_peak_erfc_amplitude.c
	sasfit_peak_erfc_area.c
	sasfit_peak_error_amplitude.c
	sasfit_peak_error_area.c
	sasfit_peak_exponentially_modified_gaussian_amplitude.c
	sasfit_peak_exponentially_modified_gaussian_area.c
	sasfit_peak_extreme_value_amplitude.c
	sasfit_peak_extreme_value_area.c
	sasfit_peak_f_variance_amplitude.c
	sasfit_peak_f_variance_area.c
	sasfit_peak_fatique_life_amplitude.c
	sasfit_peak_fatique_life_area.c
	sasfit_peak_gamma_amplitude.c
	sasfit_peak_gamma_area.c
	sasfit_peak_gaussian_amplitude.c
	sasfit_peak_gaussian_area.c
	sasfit_peak_gaussian_lorentzian_cross_product_amplitude.c
	sasfit_peak_gaussian_lorentzian_cross_product_area.c
	sasfit_peak_gaussian_lorentzian_sum_amplitude.c
	sasfit_peak_gaussian_lorentzian_sum_area.c
	sasfit_peak_generalized_gaussian1_amplitude.c
	sasfit_peak_generalized_gaussian1_area.c
	sasfit_peak_generalized_gaussian2_amplitude.c
	sasfit_peak_generalized_gaussian2_area.c
	sasfit_peak_generalized_maxwell_amplitude.c
	sasfit_peak_generalized_maxwell_area.c
	sasfit_peak_gex_amplitude.c
	sasfit_peak_gex_area.c
	sasfit_peak_giddings_amplitude.c
	sasfit_peak_giddings_area.c
	sasfit_peak_haarhoff_vander_linde_area.c
	sasfit_peak_half_gaussian_modified_gaussian_area.c
	sasfit_peak_inverted_gamma_amplitude.c
	sasfit_peak_inverted_gamma_area.c
	sasfit_peak_kumaraswamy_amplitude.c
	sasfit_peak_kumaraswamy_area.c
	sasfit_peak_laplace_amplitude.c
	sasfit_peak_laplace_area.c
	sasfit_peak_logistic_amplitude.c
	sasfit_peak_logistic_area.c
	sasfit_peak_loglogistic_amplitude.c
	sasfit_peak_loglogistic_area.c
	sasfit_peak_lognormal4parameter_amplitude.c
	sasfit_peak_lognormal4parameter_area.c
	sasfit_peak_lognormal_amplitude.c
	sasfit_peak_lognormal_area.c
	sasfit_peak_lorentzian_amplitude.c
	sasfit_peak_lorentzian_area.c
	sasfit_peak_maxwell_amplitude.c
	sasfit_peak_maxwell_area.c
	sasfit_peak_pearsonIV_amplitude.c
	sasfit_peak_pearsonIV_area.c
	sasfit_peak_pearsonVII_amplitude.c
	sasfit_peak_pearsonVII_area.c
	sasfit_peak_powerlognormal_area.c
	sasfit_peak_powernormal_area.c
	sasfit_peak_pulse_amplitude.c
	sasfit_peak_pulse_area.c
	sasfit_peak_pulse_with_2nd_width_amplitude.c
	sasfit_peak_pulse_with_2nd_width_area.c
	sasfit_peak_pulse_with_power_term_amplitude.c
	sasfit_peak_pulse_with_power_term_area.c
	sasfit_peak_student_t_amplitude.c
	sasfit_peak_student_t_area.c
	sasfit_peak_voigt_amplitude.c
	sasfit_peak_voigt_area.c
	sasfit_peak_weibull_amplitude.c
	sasfit_peak_weibull_area.c
	interface.c
)


# add plugin path for finding 'Find*.cmake' files for new <LIBS_EXT>
set(REL_CMAKE_MODULE_PATH .)

#                                                       #
### no adjustments required below this line (usually) ###
#                                                       #

project(${PRJ_NAME} C)

set(LIB_IS_PLUGIN true)

set(SRC_DIR ${${PRJ_NAME}_SOURCE_DIR})
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${SRC_DIR}/${MODULE_PATH}")

include(SasfitCmakeUtils)
sasfit_cmake_plugin()
