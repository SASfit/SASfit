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
	sasfit_param_override_get_psi2, /* 19 */
	sasfit_param_override_get_2theta, /* 20 */
	sasfit_param_override_get_lambda, /* 21 */
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
	NULL, /* 45 */
	NULL, /* 46 */
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
	NULL, /* 75 */
	NULL, /* 76 */
	NULL, /* 77 */
	NULL, /* 78 */
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
};

/* !END!: Do not edit above this line, see sasfit_common.decls for modifications. */
