/*
 * src/sasfit_common/sasfit_common.c
 *
 * Copyright (c) 2008-2009, Paul Scherrer Institute (PSI)
 *
 * This file is part of SASfit.
 *
 * SASfit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SASfit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SASfit.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Author(s) of this file:
 *   Ingo Bressler (ingo.bressler@bam.de)
 */

#include "include/sasfit_common.h"
#include "include/sasfit_utils_f2c.h"
#include "sasfit_sd.h"


sasfit_eps_t		sasfit_eps;

// initialise global storage of output channel information
sasfit_msg_chan_t	sasfit_msg_chans[] =	{
						{ 1, NULL }, // ERROR_MSG
						{ 1, NULL }  // INFO_MSG
						};


/* !BEGIN!: Do not edit below this line, see sasfit_common.decls for modifications. */

sasfit_common_stubs_t sasfit_common_stubs = {
	NULL, /* 0 */
	NULL, /* 1 */
	NULL, /* 2 */
	NULL, /* 3 */
	sasfit_set_msg_handler, /* 4 */
	sasfit_msg, /* 5 */
	sasfit_param_set_err, /* 6 */
	sasfit_integrate_ctm, /* 7 */
	sasfit_get_lib_prefix, /* 8 */
	sasfit_get_lib_suffix, /* 9 */
	sasfit_get_maxpar, /* 10 */
	sasfit_init_fct, /* 11 */
	sasfit_valid_fct, /* 12 */
	sasfit_part_diff_fct, /* 13 */
	sasfit_init_param, /* 14 */
	sasfit_get_param, /* 15 */
	sasfit_print_param, /* 16 */
	sasfit_copy_param, /* 17 */
	sasfit_param_override_get_psi, /* 18 */
	sasfit_orient_avg_ctm, /* 19 */
	NULL, /* 20 */
	NULL, /* 21 */
	sasfit_init_vector, /* 22 */
	sasfit_free_vector, /* 23 */
	sasfit_timer_create, /* 24 */
	sasfit_timer_destroy, /* 25 */
	sasfit_timer_start, /* 26 */
	sasfit_timer_measure, /* 27 */
	sasfit_gammaln, /* 28 */
	sasfit_gamma, /* 29 */
	sasfit_dgammaln_dx, /* 30 */
	sasfit_dgamma_dx, /* 31 */
	sasfit_erf_pure, /* 32 */
	sasfit_si, /* 33 */
	sasfit_log1p, /* 34 */
	sasfit_j1f, /* 35 */
	sasfit_bessj1, /* 36 */
	sasfit_rwbrush_w, /* 37 */
	sasfit_rod_fc, /* 38 */
	sasfit_sphere_fc, /* 39 */
	sasfit_gauss_fc, /* 40 */
	sasfit_polint, /* 41 */
	sasfit_qromb, /* 42 */
	sasfit_trapzd, /* 43 */
	sasfit_g, /* 44 */
	erfcx, /* 45 */
	erfcxinv, /* 46 */
	NULL, /* 47 */
	NULL, /* 48 */
	NULL, /* 49 */
	sasfit_eps_print, /* 50 */
	sasfit_eps_get_aniso, /* 51 */
	sasfit_eps_get_jmax_aniso, /* 52 */
	sasfit_eps_get_res, /* 53 */
	sasfit_eps_get_jmax_res, /* 54 */
	sasfit_eps_get_nriq, /* 55 */
	sasfit_eps_get_jmax_nriq, /* 56 */
	sasfit_eps_get_sq_or_iq, /* 57 */
	sasfit_eps_get_robertus_p, /* 58 */
	sasfit_eps_get_iter_4_mc, /* 59 */
	sasfit_eps_get_fitorsim, /* 60 */
	sasfit_eps_get_h, /* 61 */
	sasfit_eps_get_comp, /* 62 */
	sasfit_eps_set_aniso, /* 63 */
	sasfit_eps_set_jmax_aniso, /* 64 */
	sasfit_eps_set_res, /* 65 */
	sasfit_eps_set_jmax_res, /* 66 */
	sasfit_eps_set_nriq, /* 67 */
	sasfit_eps_set_jmax_nriq, /* 68 */
	sasfit_eps_set_sq_or_iq, /* 69 */
	sasfit_eps_set_robertus_p, /* 70 */
	sasfit_eps_set_iter_4_mc, /* 71 */
	sasfit_eps_set_fitorsim, /* 72 */
	sasfit_eps_set_h, /* 73 */
	sasfit_eps_set_comp, /* 74 */
	sasfit_set_sphavg_strategy, /* 75 */
	sasfit_get_sphavg_strategy, /* 76 */
	sasfit_set_int_strategy, /* 77 */
	sasfit_get_int_strategy, /* 78 */
	sasfit_sd_LogNorm, /* 79 */
	sasfit_miev0, /* 80 */
	sasfit_dmilay, /* 81 */
	sasfit_th36, /* 82 */
	sasfit_f2c_errmsg_, /* 83 */
	sasfit_f2c_tstbad_, /* 84 */
	sasfit_f2c_wrtbad_, /* 85 */
	sasfit_f2c_wrtdim_, /* 86 */
	sasfit_struve_h0, /* 87 */
	sasfit_struve_h1, /* 88 */
	zspow_, /* 89 */
	sasfit_robertus_deal, /* 90 */
	sasfit_robertus_calc, /* 91 */
	sasfit_robertus_subint, /* 92 */
	sasfit_3f2, /* 93 */
	sasfit_2f1, /* 94 */
	sasfit_pfq, /* 95 */
	sasfit_wofz, /* 96 */
	re_cerf_z, /* 97 */
	im_cerf_z, /* 98 */
	re_cerfi_z, /* 99 */
	im_cerfi_z, /* 100 */
	re_w_of_z, /* 101 */
	im_w_of_z, /* 102 */
	w_of_z_e, /* 103 */
	sasfit_jinc, /* 104 */
	find_LogNorm_int_range, /* 105 */
	hcubature, /* 106 */
	pcubature, /* 107 */
	sasfit_intccini, /* 108 */
	sasfit_intcc, /* 109 */
	sasfit_intdeiini, /* 110 */
	sasfit_intdei, /* 111 */
	sasfit_intdeoini, /* 112 */
	sasfit_intdeo, /* 113 */
	sasfit_intdeini, /* 114 */
	sasfit_intde, /* 115 */
	sasfit_cubature, /* 116 */
	sasfit_erfinv, /* 117 */
	sasfit_available_table, /* 118 */
	sasfit_order_table, /* 119 */
	sasfit_ld_by_order, /* 120 */
	sasfit_eps_get_gausslegendre, /* 121 */
	sasfit_eps_get_chebyshev1, /* 122 */
	sasfit_eps_get_chebyshev2, /* 123 */
	sasfit_eps_get_gegenbauer, /* 124 */
	sasfit_eps_get_exponential, /* 125 */
	sasfit_eps_get_laguerre, /* 126 */
	sasfit_eps_get_jacobi, /* 127 */
	sasfit_eps_get_lebedev, /* 128 */
	sasfit_eps_get_fibonacci, /* 129 */
	sasfit_eps_get_alpha, /* 130 */
	sasfit_eps_get_beta, /* 131 */
	sasfit_eps_set_alpha, /* 132 */
	sasfit_eps_set_beta, /* 133 */
	sasfit_eps_get_spherical_t_design, /* 134 */
	sasfit_available_sph_t_table, /* 135 */
	sasfit_sph_t_order_table, /* 136*/
	sasfit_sph_t_by_order, /* 137 */
	sasfit_hankel, /* 138 */
	sasfit_get_h_Ogata, /* 139 */
	sasfit_get_N_Ogata, /* 140 */
	sasfit_set_h_Ogata, /* 141 */
	sasfit_set_N_Ogata, /* 142 */
	sasfit_set_hankel_strategy, /* 143 */
	sasfit_get_hankel_strategy, /* 144 */
	sasfit_get_MSASthickness, /* 145 */
	sasfit_get_MSASlambda, /* 146 */
	sasfit_set_MSASthickness, /* 147 */
	sasfit_set_MSASlambda, /* 148 */
	sasfit_param_set_polar_phi, /* 149 */
	sasfit_param_get_polar_phi, /* 150 */
	sasfit_param_set_polar_theta, /* 151 */
	sasfit_param_get_polar_theta, /* 152 */
};
/* !END!: Do not edit above this line, see sasfit_common.decls for modifications. */

const sasfit_common_stubs_t * sasfit_common_stubs_ptr()
{
	return &sasfit_common_stubs;
}

