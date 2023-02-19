/*
 * src/sasfit_common/include/sasfit_common_stubs.h
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

#ifndef SASFIT_COMMON_STUBS_H
#define SASFIT_COMMON_STUBS_H

#include <f2c.h>

/**
 * \file sasfit_common_stubs.h
 * \ingroup sasfit_stubs
 */

// some thoughts about the strategy:
// ---------------------------------
// Howto provide sasfit_common api functions to plugins without static or
// dynamic linking the sasfit library binary ?
// Here, I decided to use the Tcl-Stubs approach for simplicity and ease of
// use.
//
// Alternative would be a completely dynamic array containing the
// sasfit_common function pointers and search for required functions in it at
// runtime during initialization.
// Disadvantages of that approach are:
// -> complicated usage by the plugin developer, functions required in a plugin
// have to be requested by putting all their names into a plugin-local array
// which is used by the init function to determine the according pointers.
// -> The plugin writer has understand all macros used and determine the
// underlying C-functions: bad thing regarding principle of 'hiding
// implementation'
// -> The call of each sasfit-api function has to be a macro itself to allow
// it to be build as non-plugin (i.e. 'sasfit_msg()' -> 'SF(sasfit_msg)()')
// -> last but not least: no type checking at compilation time by the compiler

/**
 * \defgroup sasfit_stubs Stubs
 * \ingroup sasfit_common_int
 * Provides a lookup table of all exported \ref sasfit_common functions.
 */

// Function types used by functions in the stubs table
typedef int sasfit_func_zspow_t(real * xla, real * f, integer * n, real * par);
typedef double sasfit_func_int_t(double, void*);

/**
 * \ingroup sasfit_stubs
 * Contains pointer to all functions in sasfit_common available for usage in
 * plugins.
 * Each plugin gets a pointer to this structure at initialization time.
 */

/* !BEGIN!: Do not edit below this line, see sasfit_common.decls for modifications. */

typedef struct
{
	void *reserved0;
	void *reserved1;
	void *reserved2;
	void *reserved3;
	void (*sasfit_set_msg_handler) (sasfit_msg_type_e type, int pass_std, sasfit_func_msg_t * msg_func); /* 4 */
	void (*sasfit_msg) (sasfit_msg_type_e type, const char * format, ... ); /* 5 */
	void (*sasfit_param_set_err) (sasfit_param * param, const char * format, ... ); /* 6 */
	scalar (*sasfit_integrate_ctm) (scalar int_start, scalar int_end, sasfit_func_one_t intKern_fct, sasfit_param * param, int limit, scalar epsabs, scalar epsrel); /* 7 */
	const char * (*sasfit_get_lib_prefix) (void); /* 8 */
	const char * (*sasfit_get_lib_suffix) (void); /* 9 */
	int (*sasfit_get_maxpar) (void); /* 10 */
	void (*sasfit_init_fct) (sasfit_function * f); /* 11 */
	bool (*sasfit_valid_fct) (sasfit_function * f); /* 12 */
	scalar (*sasfit_part_diff_fct) (scalar x, sasfit_function * f, int dparam); /* 13 */
	void (*sasfit_init_param) (sasfit_param * param); /* 14 */
	void (*sasfit_get_param) (sasfit_param * param, int num, ... ); /* 15 */
	void (*sasfit_print_param) (sasfit_param * param); /* 16 */
	void (*sasfit_copy_param) (sasfit_param * param, scalar * a); /* 17 */
	scalar (*sasfit_param_override_get_psi) (scalar default_val); /* 18 */
	scalar (*sasfit_orient_avg_ctm) (sasfit_func_two_t intKern_fct, sasfit_param * param, int limit, scalar epsabs, scalar epsrel); /* 7 */
	void *reserved20;
	void *reserved21;
	void (*sasfit_init_vector) (scalar ** vec, int size); /* 22 */
	void (*sasfit_free_vector) (scalar ** vec); /* 23 */
	sasfit_timer * (*sasfit_timer_create) (void); /* 24 */
	void (*sasfit_timer_destroy) (sasfit_timer ** sf_timer); /* 25 */
	void (*sasfit_timer_start) (sasfit_timer * sf_timer); /* 26 */
	double (*sasfit_timer_measure) (sasfit_timer * sf_timer); /* 27 */
	scalar (*sasfit_gammaln) (scalar x); /* 28 */
	scalar (*sasfit_gamma) (scalar x); /* 29 */
	scalar (*sasfit_dgammaln_dx) (scalar x); /* 30 */
	scalar (*sasfit_dgamma_dx) (scalar x); /* 31 */
	scalar (*sasfit_erf_pure) (scalar x); /* 32 */
	scalar (*sasfit_si) (scalar x); /* 33 */
	scalar (*sasfit_log1p) (const scalar x); /* 34 */
	scalar (*sasfit_j1f) (scalar x); /* 35 */
	scalar (*sasfit_bessj1) (scalar x); /* 36 */
	scalar (*sasfit_rwbrush_w) (scalar Q, scalar R); /* 37 */
	scalar (*sasfit_rod_fc) (scalar Q, scalar R); /* 38 */
	scalar (*sasfit_sphere_fc) (scalar Q, scalar R); /* 39 */
	scalar (*sasfit_gauss_fc) (scalar Q, scalar R); /* 40 */
	void (*sasfit_polint) (scalar xa[], scalar ya[], int n, scalar x, scalar * y, scalar * dy); /* 41 */
	scalar (*sasfit_qromb) (scalar a, scalar b, sasfit_func_one_t * trapzd_fct, sasfit_param * param); /* 42 */
	scalar (*sasfit_trapzd) (scalar a, scalar b, scalar n, sasfit_func_one_t * trapzd_fct, sasfit_param * param); /* 43 */
	scalar (*sasfit_g) (scalar fp, scalar a); /* 44 */
	scalar (*erfcx) (scalar x); /* 45 */
	scalar (*erfcxinv) (scalar y); /* 46 */
	void *reserved47;
	void *reserved48;
	void *reserved49;
	void (*sasfit_eps_print) (void); /* 50 */
	scalar (*sasfit_eps_get_aniso) (void); /* 51 */
	int (*sasfit_eps_get_jmax_aniso) (void); /* 52 */
	scalar (*sasfit_eps_get_res) (void); /* 53 */
	int (*sasfit_eps_get_jmax_res) (void); /* 54 */
	scalar (*sasfit_eps_get_nriq) (void); /* 55 */
	int (*sasfit_eps_get_jmax_nriq) (void); /* 56 */
	int (*sasfit_eps_get_sq_or_iq) (void); /* 57 */
	int (*sasfit_eps_get_robertus_p) (void); /* 58 */
	int (*sasfit_eps_get_iter_4_mc) (void); /* 59 */
	int (*sasfit_eps_get_fitorsim) (void); /* 60 */
	scalar (*sasfit_eps_get_h) (void); /* 61 */
	scalar (*sasfit_eps_get_comp) (void); /* 62 */
	void (*sasfit_eps_set_aniso) (scalar value); /* 63 */
	void (*sasfit_eps_set_jmax_aniso) (int value); /* 64 */
	void (*sasfit_eps_set_res) (scalar value); /* 65 */
	void (*sasfit_eps_set_jmax_res) (int value); /* 66 */
	void (*sasfit_eps_set_nriq) (scalar value); /* 67 */
	void (*sasfit_eps_set_jmax_nriq) (int value); /* 68 */
	void (*sasfit_eps_set_sq_or_iq) (int value); /* 69 */
	void (*sasfit_eps_set_robertus_p) (int value); /* 70 */
	void (*sasfit_eps_set_iter_4_mc) (int value); /* 71 */
	void (*sasfit_eps_set_fitorsim) (int value); /* 72 */
	void (*sasfit_eps_set_h) (scalar value); /* 73 */
	void (*sasfit_eps_set_comp) (scalar value); /* 74 */
	void (*sasfit_set_sphavg_strategy) (int value); /* 75 */
	int (*sasfit_get_sphavg_strategy) (void); /* 76 */
	void (*sasfit_set_int_strategy) (int value); /* 77 */
	int (*sasfit_get_int_strategy) (void); /* 78 */
	scalar (*sasfit_sd_LogNorm) (scalar value, sasfit_param * param); /* 79 */
	int (*sasfit_miev0) (real * xx, complex * crefin, logical * perfct, real * mimcut, logical * anyang, integer * numang, real * xmu, integer * nmom, integer * ipolzn, integer * momdim, logical * prnt, real * qext, real * qsca, real * gqsc, real * pmom, complex * sforw, complex * sback, complex * s1, complex * s2, complex * tforw, complex * tback, real * spike); /* 80 */
	int (*sasfit_dmilay) (real * rcore, real * rshell, real * wvno, complex * rindsh, complex * rindco, real * mu, integer * numang, real * qext, real * qsca, real * qbs, real * gqsc, real * m1, real * m2, real * s21, real * d21, integer * maxang); /* 81 */
	doublereal (*sasfit_th36) (real * x, sasfit_param * param, char * thnam, char * parnam, integer * npar, integer * ini, ftnlen thnam_len, ftnlen parnam_len); /* 82 */
	int (*sasfit_f2c_errmsg_) (char * messag, logical * fatal, ftnlen messag_len); /* 83 */
	logical (*sasfit_f2c_tstbad_) (char * varnam, real * relerr, ftnlen varnam_len); /* 84 */
	logical (*sasfit_f2c_wrtbad_) (char * varnam, ftnlen varnam_len); /* 85 */
	logical (*sasfit_f2c_wrtdim_) (char * dimnam, integer * minval, ftnlen dimnam_len); /* 86 */
	doublereal (*sasfit_struve_h0) (doublereal * xvalue); /* 87 */
	doublereal (*sasfit_struve_h1) (doublereal * xvalue); /* 88 */
	int (*zspow_) (sasfit_func_zspow_t * fcn, integer * nsig, integer * n, integer * itmax, real * par, real * x, real * fnorm, real * wk, integer * ier); /* 89 */
	int (*sasfit_robertus_deal) (doublereal * rm, doublereal * sig, doublereal * phi, logical * labdas); /* 90 */
	int (*sasfit_robertus_calc) (sasfit_param * param, doublereal * rm, doublereal * sig, doublereal * phi); /* 91 */
	int (*sasfit_robertus_subint) (sasfit_param * param, doublereal * q, doublereal * ai, doublereal * aint0); /* 92 */
	double (*sasfit_3f2) (double a1, double a2, double a3, double b1, double b2, double x, sasfit_param * param); /* 93 */
	double (*sasfit_2f1) (double a1, double a2, double b1, double x, sasfit_param * param); /* 94 */
	double (*sasfit_pfq) (double * p_r, double * p_i, double * q_r, double * q_i, int ip, int iq, double z_r, double z_i, int ln_pFq, int ix, double * pFq_r, double * pFq_i, int nsigfig, sasfit_param * param); /* 95 */
	int (*sasfit_wofz) (double * xi, double * yi, double * u, double * v, long * flag__); /* 96 */
	double (*re_cerf_z) (double x, double y); /* 97 */
	double (*im_cerf_z) (double x, double y); /* 98 */
	double (*re_cerfi_z) (double x, double y); /* 99 */
	double (*im_cerfi_z) (double x, double y); /* 100 */
	double (*re_w_of_z) (double x, double y); /* 101 */
	double (*im_w_of_z) (double x, double y); /* 102 */
	void (*w_of_z_e) (double x, double y, double * rew, double * imw); /* 103 */
	scalar (*sasfit_jinc) (scalar x); /* 104 */
	scalar (*find_LogNorm_int_range) (scalar dim, scalar x0, scalar sigma, scalar * Xstart, scalar * Xend, sasfit_param * param); /* 105 */
	void (*hcubature) (unsigned fdim, integrand f, void * fdata, unsigned dim, const double * xmin, const double * xmax, size_t maxEval, double reqAbsError, double reqRelError, error_norm norm, double * val, double * err); /* 106 */
	void (*pcubature) (unsigned fdim, integrand f, void * fdata, unsigned dim, const double * xmin, const double * xmax, size_t maxEval, double reqAbsError, double reqRelError, error_norm norm, double * val, double * err); /* 107 */
	void (*sasfit_intccini) (int lenw, double * w); /* 108 */
	void (*sasfit_intcc) (sasfit_func_int_t * f, double a, double b, double eps, int lenw, double * w, double * i, double * err, void * fparams); /* 109 */
	void (*sasfit_intdeiini) (int lenaw, double tiny, double eps, double * aw); /* 110 */
	void (*sasfit_intdei) (sasfit_func_int_t * f, double a, double * aw, double * i, double * err, void * fparams); /* 111 */
	void (*sasfit_intdeoini) (int lenaw, double tiny, double eps, double * aw); /* 112 */
	void (*sasfit_intdeo) (sasfit_func_int_t * f, double a, double omega, double * aw, double * i, double * err, void * fparams); /* 113 */
	void (*sasfit_intdeini) (int lenaw, double tiny, double eps, double * aw); /* 114 */
	void (*sasfit_intde) (sasfit_func_int_t * f, double a, double b, double * aw, double * i, double * err, void * fparams); /* 115 */
	int (*sasfit_cubature) (size_t ndim, scalar * int_start, scalar * int_end, sasfit_func_ndim_t * intKern_fct, void * param, scalar epsrel, scalar * result, scalar * error); /* 116 */
	scalar (*sasfit_erfinv) (scalar y); /* 117 */
	int (*sasfit_available_table) ( int rule ); /* 118 */
	int (*sasfit_order_table) ( int rule ); /* 119 */
	void (*sasfit_ld_by_order) ( int order, double *x, double *y, double *z, double *w ); /* 120 */
	int (*sasfit_eps_get_gausslegendre) (void); /*121 */
	int (*sasfit_eps_get_chebyshev1) (void); /*122 */
	int (*sasfit_eps_get_chebyshev2) (void); /*123 */
	int (*sasfit_eps_get_gegenbauer) (void); /*124 */
	int (*sasfit_eps_get_exponential) (void); /*125 */
	int (*sasfit_eps_get_laguerre) (void); /*126 */
	int (*sasfit_eps_get_jacobi) (void); /*127 */
	int (*sasfit_eps_get_lebedev) (void); /*128 */
	int (*sasfit_eps_get_fibonacci) (void); /*129 */
	scalar (*sasfit_eps_get_alpha) (void); /*130 */
	scalar (*sasfit_eps_get_beta) (void); /*131 */
	void (*sasfit_eps_set_alpha) (scalar value); /* 132 */
	void (*sasfit_eps_set_beta) (scalar value); /* 133 */
	int (*sasfit_eps_get_spherical_t_design) (void); /*134 */
	int (*sasfit_available_sph_t_table) ( int rule ); /*135 */
    int (*sasfit_sph_t_order_table) ( int rule ); /*136 */
    void (*sasfit_sph_t_by_order) ( int order, double *x, double *y, double *z); /*137 */
    scalar (*sasfit_hankel) (double nu, double (*f)(double q, void *par), double x, void *fparams); /* 138 */
	scalar (*sasfit_get_h_Ogata) (void); /* 139 */
	scalar (*sasfit_get_N_Ogata) (void); /* 140 */
	void (*sasfit_set_h_Ogata) (scalar value); /* 141 */
	void (*sasfit_set_N_Ogata) (scalar value); /* 142 */
	void (*sasfit_set_hankel_strategy) (int value); /* 143 */
	int (*sasfit_get_hankel_strategy) (void); /* 144 */
	scalar (*sasfit_get_MSASthickness) (void); /* 145 */
	scalar (*sasfit_get_MSASlambda) (void); /* 146 */
	void (*sasfit_set_MSASthickness) (scalar value); /* 147 */
	void (*sasfit_set_MSASlambda) (scalar value); /* 148 */
	void (*sasfit_param_set_polar_phi) (scalar new_value); /* 149 */
	scalar (*sasfit_param_get_polar_phi) (void); /* 150 */
	void (*sasfit_param_set_polar_theta) (scalar new_value); /* 151 */
	scalar (*sasfit_param_get_polar_theta) (void); /* 152 */
	void (*sasfit_set_sg_level) (int value); /* 153 */
	int (*sasfit_get_sg_level) (void); /* 154 */
} sasfit_common_stubs_t;

#if defined(MAKE_SASFIT_PLUGIN)

/*
 * Inline function declarations:
 */

/* Slot 0 is reserved */
/* Slot 1 is reserved */
/* Slot 2 is reserved */
/* Slot 3 is reserved */
#ifndef sasfit_set_msg_handler
#define sasfit_set_msg_handler \
	(SASFIT_COMMON_STUBS()->sasfit_set_msg_handler) /* 4 */
#endif
#ifndef sasfit_msg
#define sasfit_msg \
	(SASFIT_COMMON_STUBS()->sasfit_msg) /* 5 */
#endif
#ifndef sasfit_param_set_err
#define sasfit_param_set_err \
	(SASFIT_COMMON_STUBS()->sasfit_param_set_err) /* 6 */
#endif
#ifndef sasfit_integrate_ctm
#define sasfit_integrate_ctm \
	(SASFIT_COMMON_STUBS()->sasfit_integrate_ctm) /* 7 */
#endif
#ifndef sasfit_get_lib_prefix
#define sasfit_get_lib_prefix \
	(SASFIT_COMMON_STUBS()->sasfit_get_lib_prefix) /* 8 */
#endif
#ifndef sasfit_get_lib_suffix
#define sasfit_get_lib_suffix \
	(SASFIT_COMMON_STUBS()->sasfit_get_lib_suffix) /* 9 */
#endif
#ifndef sasfit_get_maxpar
#define sasfit_get_maxpar \
	(SASFIT_COMMON_STUBS()->sasfit_get_maxpar) /* 10 */
#endif
#ifndef sasfit_init_fct
#define sasfit_init_fct \
	(SASFIT_COMMON_STUBS()->sasfit_init_fct) /* 11 */
#endif
#ifndef sasfit_valid_fct
#define sasfit_valid_fct \
	(SASFIT_COMMON_STUBS()->sasfit_valid_fct) /* 12 */
#endif
#ifndef sasfit_part_diff_fct
#define sasfit_part_diff_fct \
	(SASFIT_COMMON_STUBS()->sasfit_part_diff_fct) /* 13 */
#endif
#ifndef sasfit_init_param
#define sasfit_init_param \
	(SASFIT_COMMON_STUBS()->sasfit_init_param) /* 14 */
#endif
#ifndef sasfit_get_param
#define sasfit_get_param \
	(SASFIT_COMMON_STUBS()->sasfit_get_param) /* 15 */
#endif
#ifndef sasfit_print_param
#define sasfit_print_param \
	(SASFIT_COMMON_STUBS()->sasfit_print_param) /* 16 */
#endif
#ifndef sasfit_copy_param
#define sasfit_copy_param \
	(SASFIT_COMMON_STUBS()->sasfit_copy_param) /* 17 */
#endif
#ifndef sasfit_param_override_get_psi
#define sasfit_param_override_get_psi \
	(SASFIT_COMMON_STUBS()->sasfit_param_override_get_psi) /* 18 */
#endif
#ifndef sasfit_orient_avg_ctm
#define sasfit_orient_avg_ctm \
	(SASFIT_COMMON_STUBS()->sasfit_orient_avg_ctm) /* 19 */
#endif
/* Slot 20 is reserved */
/* Slot 21 is reserved */
#ifndef sasfit_init_vector
#define sasfit_init_vector \
	(SASFIT_COMMON_STUBS()->sasfit_init_vector) /* 22 */
#endif
#ifndef sasfit_free_vector
#define sasfit_free_vector \
	(SASFIT_COMMON_STUBS()->sasfit_free_vector) /* 23 */
#endif
#ifndef sasfit_timer_create
#define sasfit_timer_create \
	(SASFIT_COMMON_STUBS()->sasfit_timer_create) /* 24 */
#endif
#ifndef sasfit_timer_destroy
#define sasfit_timer_destroy \
	(SASFIT_COMMON_STUBS()->sasfit_timer_destroy) /* 25 */
#endif
#ifndef sasfit_timer_start
#define sasfit_timer_start \
	(SASFIT_COMMON_STUBS()->sasfit_timer_start) /* 26 */
#endif
#ifndef sasfit_timer_measure
#define sasfit_timer_measure \
	(SASFIT_COMMON_STUBS()->sasfit_timer_measure) /* 27 */
#endif
#ifndef sasfit_gammaln
#define sasfit_gammaln \
	(SASFIT_COMMON_STUBS()->sasfit_gammaln) /* 28 */
#endif
#ifndef sasfit_gamma
#define sasfit_gamma \
	(SASFIT_COMMON_STUBS()->sasfit_gamma) /* 29 */
#endif
#ifndef sasfit_dgammaln_dx
#define sasfit_dgammaln_dx \
	(SASFIT_COMMON_STUBS()->sasfit_dgammaln_dx) /* 30 */
#endif
#ifndef sasfit_dgamma_dx
#define sasfit_dgamma_dx \
	(SASFIT_COMMON_STUBS()->sasfit_dgamma_dx) /* 31 */
#endif
#ifndef sasfit_erf_pure
#define sasfit_erf_pure \
	(SASFIT_COMMON_STUBS()->sasfit_erf_pure) /* 32 */
#endif
#ifndef sasfit_si
#define sasfit_si \
	(SASFIT_COMMON_STUBS()->sasfit_si) /* 33 */
#endif
#ifndef sasfit_log1p
#define sasfit_log1p \
	(SASFIT_COMMON_STUBS()->sasfit_log1p) /* 34 */
#endif
#ifndef sasfit_j1f
#define sasfit_j1f \
	(SASFIT_COMMON_STUBS()->sasfit_j1f) /* 35 */
#endif
#ifndef sasfit_bessj1
#define sasfit_bessj1 \
	(SASFIT_COMMON_STUBS()->sasfit_bessj1) /* 36 */
#endif
#ifndef sasfit_rwbrush_w
#define sasfit_rwbrush_w \
	(SASFIT_COMMON_STUBS()->sasfit_rwbrush_w) /* 37 */
#endif
#ifndef sasfit_rod_fc
#define sasfit_rod_fc \
	(SASFIT_COMMON_STUBS()->sasfit_rod_fc) /* 38 */
#endif
#ifndef sasfit_sphere_fc
#define sasfit_sphere_fc \
	(SASFIT_COMMON_STUBS()->sasfit_sphere_fc) /* 39 */
#endif
#ifndef sasfit_gauss_fc
#define sasfit_gauss_fc \
	(SASFIT_COMMON_STUBS()->sasfit_gauss_fc) /* 40 */
#endif
#ifndef sasfit_polint
#define sasfit_polint \
	(SASFIT_COMMON_STUBS()->sasfit_polint) /* 41 */
#endif
#ifndef sasfit_qromb
#define sasfit_qromb \
	(SASFIT_COMMON_STUBS()->sasfit_qromb) /* 42 */
#endif
#ifndef sasfit_trapzd
#define sasfit_trapzd \
	(SASFIT_COMMON_STUBS()->sasfit_trapzd) /* 43 */
#endif
#ifndef sasfit_g
#define sasfit_g \
	(SASFIT_COMMON_STUBS()->sasfit_g) /* 44 */
#endif
#ifndef erfcx
#define erfcx \
	(SASFIT_COMMON_STUBS()->erfcx) /* 44 */
#endif
#ifndef erfcxinv
#define erfcxinv \
	(SASFIT_COMMON_STUBS()->erfcxinv) /* 46 */
#endif
/* Slot 47 is reserved */
/* Slot 48 is reserved */
/* Slot 49 is reserved */
#ifndef sasfit_eps_print
#define sasfit_eps_print \
	(SASFIT_COMMON_STUBS()->sasfit_eps_print) /* 50 */
#endif
#ifndef sasfit_eps_get_aniso
#define sasfit_eps_get_aniso \
	(SASFIT_COMMON_STUBS()->sasfit_eps_get_aniso) /* 51 */
#endif
#ifndef sasfit_eps_get_jmax_aniso
#define sasfit_eps_get_jmax_aniso \
	(SASFIT_COMMON_STUBS()->sasfit_eps_get_jmax_aniso) /* 52 */
#endif
#ifndef sasfit_eps_get_res
#define sasfit_eps_get_res \
	(SASFIT_COMMON_STUBS()->sasfit_eps_get_res) /* 53 */
#endif
#ifndef sasfit_eps_get_jmax_res
#define sasfit_eps_get_jmax_res \
	(SASFIT_COMMON_STUBS()->sasfit_eps_get_jmax_res) /* 54 */
#endif
#ifndef sasfit_eps_get_nriq
#define sasfit_eps_get_nriq \
	(SASFIT_COMMON_STUBS()->sasfit_eps_get_nriq) /* 55 */
#endif
#ifndef sasfit_eps_get_jmax_nriq
#define sasfit_eps_get_jmax_nriq \
	(SASFIT_COMMON_STUBS()->sasfit_eps_get_jmax_nriq) /* 56 */
#endif
#ifndef sasfit_eps_get_sq_or_iq
#define sasfit_eps_get_sq_or_iq \
	(SASFIT_COMMON_STUBS()->sasfit_eps_get_sq_or_iq) /* 57 */
#endif
#ifndef sasfit_eps_get_robertus_p
#define sasfit_eps_get_robertus_p \
	(SASFIT_COMMON_STUBS()->sasfit_eps_get_robertus_p) /* 58 */
#endif
#ifndef sasfit_eps_get_iter_4_mc
#define sasfit_eps_get_iter_4_mc \
	(SASFIT_COMMON_STUBS()->sasfit_eps_get_iter_4_mc) /* 59 */
#endif
#ifndef sasfit_eps_get_fitorsim
#define sasfit_eps_get_fitorsim \
	(SASFIT_COMMON_STUBS()->sasfit_eps_get_fitorsim) /* 60 */
#endif
#ifndef sasfit_eps_get_h
#define sasfit_eps_get_h \
	(SASFIT_COMMON_STUBS()->sasfit_eps_get_h) /* 61 */
#endif
#ifndef sasfit_eps_get_comp
#define sasfit_eps_get_comp \
	(SASFIT_COMMON_STUBS()->sasfit_eps_get_comp) /* 62 */
#endif
#ifndef sasfit_eps_set_aniso
#define sasfit_eps_set_aniso \
	(SASFIT_COMMON_STUBS()->sasfit_eps_set_aniso) /* 63 */
#endif
#ifndef sasfit_eps_set_jmax_aniso
#define sasfit_eps_set_jmax_aniso \
	(SASFIT_COMMON_STUBS()->sasfit_eps_set_jmax_aniso) /* 64 */
#endif
#ifndef sasfit_eps_set_res
#define sasfit_eps_set_res \
	(SASFIT_COMMON_STUBS()->sasfit_eps_set_res) /* 65 */
#endif
#ifndef sasfit_eps_set_jmax_res
#define sasfit_eps_set_jmax_res \
	(SASFIT_COMMON_STUBS()->sasfit_eps_set_jmax_res) /* 66 */
#endif
#ifndef sasfit_eps_set_nriq
#define sasfit_eps_set_nriq \
	(SASFIT_COMMON_STUBS()->sasfit_eps_set_nriq) /* 67 */
#endif
#ifndef sasfit_eps_set_jmax_nriq
#define sasfit_eps_set_jmax_nriq \
	(SASFIT_COMMON_STUBS()->sasfit_eps_set_jmax_nriq) /* 68 */
#endif
#ifndef sasfit_eps_set_sq_or_iq
#define sasfit_eps_set_sq_or_iq \
	(SASFIT_COMMON_STUBS()->sasfit_eps_set_sq_or_iq) /* 69 */
#endif
#ifndef sasfit_eps_set_robertus_p
#define sasfit_eps_set_robertus_p \
	(SASFIT_COMMON_STUBS()->sasfit_eps_set_robertus_p) /* 70 */
#endif
#ifndef sasfit_eps_set_iter_4_mc
#define sasfit_eps_set_iter_4_mc \
	(SASFIT_COMMON_STUBS()->sasfit_eps_set_iter_4_mc) /* 71 */
#endif
#ifndef sasfit_eps_set_fitorsim
#define sasfit_eps_set_fitorsim \
	(SASFIT_COMMON_STUBS()->sasfit_eps_set_fitorsim) /* 72 */
#endif
#ifndef sasfit_eps_set_h
#define sasfit_eps_set_h \
	(SASFIT_COMMON_STUBS()->sasfit_eps_set_h) /* 73 */
#endif
#ifndef sasfit_eps_set_comp
#define sasfit_eps_set_comp \
	(SASFIT_COMMON_STUBS()->sasfit_eps_set_comp) /* 74 */
#endif
#ifndef sasfit_set_sphavg_strategy
#define sasfit_set_sphavg_strategy \
	(SASFIT_COMMON_STUBS()->sasfit_set_sphavg_strategy) /* 75 */
#endif
#ifndef sasfit_get_sphavg_strategy
#define sasfit_get_sphavg_strategy \
	(SASFIT_COMMON_STUBS()->sasfit_get_sphavg_strategy) /* 76 */
#endif
#ifndef sasfit_set_int_strategy
#define sasfit_set_int_strategy \
	(SASFIT_COMMON_STUBS()->sasfit_set_int_strategy) /* 77 */
#endif
#ifndef sasfit_get_int_strategy
#define sasfit_get_int_strategy \
	(SASFIT_COMMON_STUBS()->sasfit_get_int_strategy) /* 78 */
#endif
#ifndef sasfit_sd_LogNorm
#define sasfit_sd_LogNorm \
	(SASFIT_COMMON_STUBS()->sasfit_sd_LogNorm) /* 79 */
#endif
#ifndef sasfit_miev0
#define sasfit_miev0 \
	(SASFIT_COMMON_STUBS()->sasfit_miev0) /* 80 */
#endif
#ifndef sasfit_dmilay
#define sasfit_dmilay \
	(SASFIT_COMMON_STUBS()->sasfit_dmilay) /* 81 */
#endif
#ifndef sasfit_th36
#define sasfit_th36 \
	(SASFIT_COMMON_STUBS()->sasfit_th36) /* 82 */
#endif
#ifndef sasfit_f2c_errmsg_
#define sasfit_f2c_errmsg_ \
	(SASFIT_COMMON_STUBS()->sasfit_f2c_errmsg_) /* 83 */
#endif
#ifndef sasfit_f2c_tstbad_
#define sasfit_f2c_tstbad_ \
	(SASFIT_COMMON_STUBS()->sasfit_f2c_tstbad_) /* 84 */
#endif
#ifndef sasfit_f2c_wrtbad_
#define sasfit_f2c_wrtbad_ \
	(SASFIT_COMMON_STUBS()->sasfit_f2c_wrtbad_) /* 85 */
#endif
#ifndef sasfit_f2c_wrtdim_
#define sasfit_f2c_wrtdim_ \
	(SASFIT_COMMON_STUBS()->sasfit_f2c_wrtdim_) /* 86 */
#endif
#ifndef sasfit_struve_h0
#define sasfit_struve_h0 \
	(SASFIT_COMMON_STUBS()->sasfit_struve_h0) /* 87 */
#endif
#ifndef sasfit_struve_h1
#define sasfit_struve_h1 \
	(SASFIT_COMMON_STUBS()->sasfit_struve_h1) /* 88 */
#endif
#ifndef zspow_
#define zspow_ \
	(SASFIT_COMMON_STUBS()->zspow_) /* 89 */
#endif
#ifndef sasfit_robertus_deal
#define sasfit_robertus_deal \
	(SASFIT_COMMON_STUBS()->sasfit_robertus_deal) /* 90 */
#endif
#ifndef sasfit_robertus_calc
#define sasfit_robertus_calc \
	(SASFIT_COMMON_STUBS()->sasfit_robertus_calc) /* 91 */
#endif
#ifndef sasfit_robertus_subint
#define sasfit_robertus_subint \
	(SASFIT_COMMON_STUBS()->sasfit_robertus_subint) /* 92 */
#endif
#ifndef sasfit_3f2
#define sasfit_3f2 \
	(SASFIT_COMMON_STUBS()->sasfit_3f2) /* 93 */
#endif
#ifndef sasfit_2f1
#define sasfit_2f1 \
	(SASFIT_COMMON_STUBS()->sasfit_2f1) /* 94 */
#endif
#ifndef sasfit_pfq
#define sasfit_pfq \
	(SASFIT_COMMON_STUBS()->sasfit_pfq) /* 95 */
#endif
#ifndef sasfit_wofz
#define sasfit_wofz \
	(SASFIT_COMMON_STUBS()->sasfit_wofz) /* 96 */
#endif
#ifndef re_cerf_z
#define re_cerf_z \
	(SASFIT_COMMON_STUBS()->re_cerf_z) /* 97 */
#endif
#ifndef im_cerf_z
#define im_cerf_z \
	(SASFIT_COMMON_STUBS()->im_cerf_z) /* 98 */
#endif
#ifndef re_cerfi_z
#define re_cerfi_z \
	(SASFIT_COMMON_STUBS()->re_cerfi_z) /* 99 */
#endif
#ifndef im_cerfi_z
#define im_cerfi_z \
	(SASFIT_COMMON_STUBS()->im_cerfi_z) /* 100 */
#endif
#ifndef re_w_of_z
#define re_w_of_z \
	(SASFIT_COMMON_STUBS()->re_w_of_z) /* 101 */
#endif
#ifndef im_w_of_z
#define im_w_of_z \
	(SASFIT_COMMON_STUBS()->im_w_of_z) /* 102 */
#endif
#ifndef w_of_z_e
#define w_of_z_e \
	(SASFIT_COMMON_STUBS()->w_of_z_e) /* 103 */
#endif
#ifndef sasfit_jinc
#define sasfit_jinc \
	(SASFIT_COMMON_STUBS()->sasfit_jinc) /* 104 */
#endif
#ifndef find_LogNorm_int_range
#define find_LogNorm_int_range \
	(SASFIT_COMMON_STUBS()->find_LogNorm_int_range) /* 105 */
#endif
#ifndef hcubature
#define hcubature \
	(SASFIT_COMMON_STUBS()->hcubature) /* 106 */
#endif
#ifndef pcubature
#define pcubature \
	(SASFIT_COMMON_STUBS()->pcubature) /* 107 */
#endif
#ifndef sasfit_intccini
#define sasfit_intccini \
	(SASFIT_COMMON_STUBS()->sasfit_intccini) /* 108 */
#endif
#ifndef sasfit_intcc
#define sasfit_intcc \
	(SASFIT_COMMON_STUBS()->sasfit_intcc) /* 109 */
#endif
#ifndef sasfit_intdeiini
#define sasfit_intdeiini \
	(SASFIT_COMMON_STUBS()->sasfit_intdeiini) /* 110 */
#endif
#ifndef sasfit_intdei
#define sasfit_intdei \
	(SASFIT_COMMON_STUBS()->sasfit_intdei) /* 111 */
#endif
#ifndef sasfit_intdeoini
#define sasfit_intdeoini \
	(SASFIT_COMMON_STUBS()->sasfit_intdeoini) /* 112 */
#endif
#ifndef sasfit_intdeo
#define sasfit_intdeo \
	(SASFIT_COMMON_STUBS()->sasfit_intdeo) /* 113 */
#endif
#ifndef sasfit_intdeini
#define sasfit_intdeini \
	(SASFIT_COMMON_STUBS()->sasfit_intdeini) /* 114 */
#endif
#ifndef sasfit_intde
#define sasfit_intde \
	(SASFIT_COMMON_STUBS()->sasfit_intde) /* 115 */
#endif
#ifndef sasfit_cubature
#define sasfit_cubature \
	(SASFIT_COMMON_STUBS()->sasfit_cubature) /* 116 */
#endif
#ifndef sasfit_erfinv
#define sasfit_erfinv \
	(SASFIT_COMMON_STUBS()->sasfit_erfinv) /* 117 */
#endif
#ifndef sasfit_available_table
#define sasfit_available_table \
	(SASFIT_COMMON_STUBS()->sasfit_available_table) /* 118 */
#endif
#ifndef sasfit_order_table
#define sasfit_order_table \
	(SASFIT_COMMON_STUBS()->sasfit_order_table) /* 119 */
#endif
#ifndef sasfit_ld_by_order
#define sasfit_ld_by_order \
	(SASFIT_COMMON_STUBS()->sasfit_ld_by_order) /* 120 */
#endif
#ifndef sasfit_eps_get_gausslegendre
#define sasfit_eps_get_gausslegendre \
	(SASFIT_COMMON_STUBS()->sasfit_eps_get_gausslegendre) /* 121 */
#endif
#ifndef sasfit_eps_get_chebyshev1
#define sasfit_eps_get_chebyshev1 \
	(SASFIT_COMMON_STUBS()->sasfit_eps_get_chebyshev1) /* 122 */
#endif
#ifndef sasfit_eps_get_chebyshev2
#define sasfit_eps_get_chebyshev2 \
	(SASFIT_COMMON_STUBS()->sasfit_eps_get_chebyshev2) /* 123 */
#endif
#ifndef sasfit_eps_get_gegenbauer
#define sasfit_eps_get_gegenbauer \
	(SASFIT_COMMON_STUBS()->sasfit_eps_get_gegenbauer) /* 124 */
#endif
#ifndef sasfit_eps_get_exponential
#define sasfit_eps_get_exponential \
	(SASFIT_COMMON_STUBS()->sasfit_eps_get_exponential) /* 125 */
#endif
#ifndef sasfit_eps_get_laguerre
#define sasfit_eps_get_laguerre \
	(SASFIT_COMMON_STUBS()->sasfit_eps_get_laguerre) /* 126 */
#endif
#ifndef sasfit_eps_get_jacobi
#define sasfit_eps_get_jacobi \
	(SASFIT_COMMON_STUBS()->sasfit_eps_get_jacobi) /* 127 */
#endif
#ifndef sasfit_eps_get_lebedev
#define sasfit_eps_get_lebedev \
	(SASFIT_COMMON_STUBS()->sasfit_eps_get_lebedev) /* 128 */
#endif
#ifndef sasfit_eps_get_fibonacci
#define sasfit_eps_get_fibonacci \
	(SASFIT_COMMON_STUBS()->sasfit_eps_get_fibonacci) /* 129 */
#endif
#ifndef sasfit_eps_get_alpha
#define sasfit_eps_get_alpha \
	(SASFIT_COMMON_STUBS()->sasfit_eps_get_alpha) /* 130 */
#endif
#ifndef sasfit_eps_get_beta
#define sasfit_eps_get_beta \
	(SASFIT_COMMON_STUBS()->sasfit_eps_get_beta) /* 131 */
#endif
#ifndef sasfit_eps_set_alpha
#define sasfit_eps_set_alpha \
	(SASFIT_COMMON_STUBS()->sasfit_eps_set_alpha) /* 132 */
#endif
#ifndef sasfit_eps_set_beta
#define sasfit_eps_set_beta \
	(SASFIT_COMMON_STUBS()->sasfit_eps_set_beta) /* 133 */
#endif
#ifndef sasfit_eps_get_spherical_t_design
#define sasfit_eps_get_spherical_t_design \
	(SASFIT_COMMON_STUBS()->sasfit_eps_get_spherical_t_design) /* 134 */
#endif
#ifndef sasfit_available_sph_t_table
#define sasfit_available_sph_t_table \
	(SASFIT_COMMON_STUBS()->sasfit_available_sph_t_table) /* 135 */
#endif
#ifndef sasfit_sph_t_order_table
#define sasfit_sph_t_order_table \
	(SASFIT_COMMON_STUBS()->sasfit_sph_t_order_table) /* 136 */
#endif
#ifndef sasfit_sph_t_by_order
#define sasfit_sph_t_by_order \
	(SASFIT_COMMON_STUBS()->sasfit_sph_t_by_order) /* 137 */
#endif
#ifndef sasfit_hankel
#define sasfit_hankel \
	(SASFIT_COMMON_STUBS()->sasfit_hankel) /* 138 */
#endif
#ifndef sasfit_get_h_Ogata
#define sasfit_get_h_Ogata \
	(SASFIT_COMMON_STUBS()->sasfit_get_h_Ogata) /* 139 */
#endif
#ifndef sasfit_get_N_Ogata
#define sasfit_get_N_Ogata \
	(SASFIT_COMMON_STUBS()->sasfit_get_N_Ogata) /* 140 */
#endif
#ifndef sasfit_set_h_Ogata
#define sasfit_set_h_Ogata \
	(SASFIT_COMMON_STUBS()->sasfit_set_h_Ogata) /* 141 */
#endif
#ifndef sasfit_set_N_Ogata
#define sasfit_set_N_Ogata \
	(SASFIT_COMMON_STUBS()->sasfit_set_N_Ogata) /* 142 */
#endif
#ifndef sasfit_set_hankel_strategy
#define sasfit_set_hankel_strategy \
	(SASFIT_COMMON_STUBS()->sasfit_set_hankel_strategy) /* 143 */
#endif
#ifndef sasfit_get_hankel_strategy
#define sasfit_get_hankel_strategy \
	(SASFIT_COMMON_STUBS()->sasfit_get_hankel_strategy) /* 144 */
#endif
#ifndef sasfit_get_MSASthickness
#define sasfit_get_MSASthickness \
	(SASFIT_COMMON_STUBS()->sasfit_get_MSASthickness) /* 145 */
#endif
#ifndef sasfit_get_MSASlambda
#define sasfit_get_MSASlambda \
	(SASFIT_COMMON_STUBS()->sasfit_get_MSASlambda) /* 146 */
#endif
#ifndef sasfit_set_MSASthickness
#define sasfit_set_MSASthickness \
	(SASFIT_COMMON_STUBS()->sasfit_set_MSASthickness) /* 147 */
#endif
#ifndef sasfit_set_MSASlambda
#define sasfit_set_MSASlambda \
	(SASFIT_COMMON_STUBS()->sasfit_set_MSASlambda) /* 148 */
#endif
#ifndef sasfit_param_set_polar_phi
#define sasfit_param_set_polar_phi \
	(SASFIT_COMMON_STUBS()->sasfit_param_set_polar_phi) /* 149 */
#endif
#ifndef sasfit_param_get_polar_phi
#define sasfit_param_get_polar_phi \
	(SASFIT_COMMON_STUBS()->sasfit_param_get_polar_phi) /* 150 */
#endif
#ifndef sasfit_param_set_polar_theta
#define sasfit_param_set_polar_theta \
	(SASFIT_COMMON_STUBS()->sasfit_param_set_polar_theta) /* 151 */
#endif
#ifndef sasfit_param_get_polar_theta
#define sasfit_param_get_polar_theta \
	(SASFIT_COMMON_STUBS()->sasfit_param_get_polar_theta) /* 152 */
#endif
#ifndef sasfit_set_sg_level
#define sasfit_set_sg_level \
	(SASFIT_COMMON_STUBS()->sasfit_set_sg_level) /* 153 */
#endif
#ifndef sasfit_get_sg_level
#define sasfit_get_sg_level \
	(SASFIT_COMMON_STUBS()->sasfit_get_sg_level) /* 154 */
#endif
#endif /* defined(MAKE_SASFIT_PLUGIN) */

/* !END!: Do not edit above this line, see sasfit_common.decls for modifications. */

#endif // file
