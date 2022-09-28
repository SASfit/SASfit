# src/sasfit_common/sasfit_common.decls
#
# Copyright (c) 2008-2009, Paul Scherrer Institute (PSI)
#
# This file is part of SASfit.
#
# SASfit is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# SASfit is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with SASfit.  If not, see <http://www.gnu.org/licenses/>.

# Author(s) of this file:
#   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
#   Ingo Bressler (ingo@cs.tu-berlin.de)


# This file contains the declarations for all supported public
# functions that are exported by the sasfit_common library via
# the stubs table.
# This file is used to generate the sasfit_common_stubs.h
# and sasfit_common.c files. Use src/scripts/genStubs.tcl for this purpose:
# cd <sasfit-dir>
# tclsh src/scripts/genStubs.tcl src/sasfit_common src/sasfit_common/sasfit_common.decls
# <command>                      <output dir>      <declaration file>

library sasfit_common

interface sasfit_common

# Declare each of the functions in the public sasfit_common interface.  Note that
# the an index should never be reused for a different function in order
# to preserve backwards compatibility.

declare 4 generic {
	void sasfit_set_msg_handler(sasfit_msg_type_e type, int pass_std, sasfit_func_msg_t * msg_func)
}
declare 5 generic {
	void sasfit_msg(sasfit_msg_type_e type, const char * format, ...)
}
declare 6 generic {
	void sasfit_param_set_err(sasfit_param * param, const char * format, ...)
}
declare 7 generic {
	scalar sasfit_integrate_ctm(scalar int_start, scalar int_end, sasfit_func_one_t intKern_fct,
		sasfit_param * param, int limit, scalar epsabs, scalar epsrel)
}
declare 8 generic {
	const char * sasfit_get_lib_prefix(void)
}
declare 9 generic {
	const char * sasfit_get_lib_suffix(void)
}
declare 10 generic {
	int sasfit_get_maxpar(void)
}
declare 11 generic {
	void sasfit_init_fct(sasfit_function * f)
}
declare 12 generic {
	bool sasfit_valid_fct(sasfit_function * f)
}
declare 13 generic {
	scalar sasfit_part_diff_fct(scalar x, sasfit_function * f, int dparam)
}
declare 14 generic {
	void sasfit_init_param(sasfit_param * param)
}
declare 15 generic {
	void sasfit_get_param(sasfit_param * param, int num, ...)
}
declare 16 generic {
	void sasfit_print_param(sasfit_param * param)
}
declare 17 generic {
	void sasfit_copy_param(sasfit_param * param, scalar * a)
}
declare 18 generic {
	scalar sasfit_param_override_get_psi (scalar default_val)
}
declare 19 generic {
	scalar sasfit_orient_avg_ctm(sasfit_func_two_t intKern_fct,
		sasfit_param * param, int limit, scalar epsabs, scalar epsrel)
}



declare 22 generic {
	void sasfit_init_vector(scalar ** vec, int size)
}
declare 23 generic {
	void sasfit_free_vector(scalar ** vec)
}
declare 24 generic {
	sasfit_timer * sasfit_timer_create(void)
}
declare 25 generic {
	void sasfit_timer_destroy(sasfit_timer ** sf_timer)
}
declare 26 generic {
	void sasfit_timer_start(sasfit_timer * sf_timer)
}
declare 27 generic {
	double sasfit_timer_measure(sasfit_timer * sf_timer)
}
declare 28 generic {
	scalar sasfit_gammaln(scalar x)
}
declare 29 generic {
	scalar sasfit_gamma(scalar x)
}
declare 30 generic {
	scalar sasfit_dgammaln_dx(scalar x)
}
declare 31 generic {
	scalar sasfit_dgamma_dx(scalar x)
}
declare 32 generic {
	scalar sasfit_erf_pure(scalar x)
}
declare 33 generic {
	scalar sasfit_si(scalar x)
}
declare 34 generic {
	scalar sasfit_log1p(const scalar x)
}
declare 35 generic {
	scalar sasfit_j1f(scalar x)
}
declare 36 generic {
	scalar sasfit_bessj1(scalar x)
}
declare 37 generic {
	scalar sasfit_rwbrush_w(scalar Q, scalar R)
}
declare 38 generic {
	scalar sasfit_rod_fc(scalar Q, scalar R)
}
declare 39 generic {
	scalar sasfit_sphere_fc(scalar Q, scalar R)
}
declare 40 generic {
	scalar sasfit_gauss_fc(scalar Q, scalar R)
}
declare 41 generic {
	void sasfit_polint(scalar xa[], scalar ya[], int n, scalar x, scalar * y, scalar * dy)
}
declare 42 generic {
	scalar sasfit_qromb(scalar a, scalar b, sasfit_func_one_t * trapzd_fct, sasfit_param * param)
}
declare 43 generic {
	scalar sasfit_trapzd(scalar a, scalar b, scalar n, sasfit_func_one_t * trapzd_fct, sasfit_param * param)
}
declare 44 generic {
	scalar sasfit_g(scalar fp, scalar a)
}
declare 45 generic {
	scalar erfcx(scalar x)
}
declare 46 generic {
	scalar erfcxinv(scalar y)
}


declare 50 generic {
	void sasfit_eps_print(void)
}
# getter
declare 51 generic {
	scalar sasfit_eps_get_aniso(void)
}
declare 52 generic {
	int sasfit_eps_get_jmax_aniso(void)
}
declare 53 generic {
	scalar sasfit_eps_get_res(void)
}
declare 54 generic {
	int sasfit_eps_get_jmax_res(void)
}
declare 55 generic {
	scalar sasfit_eps_get_nriq(void)
}
declare 56 generic {
	int sasfit_eps_get_jmax_nriq(void)
}
declare 57 generic {
	int sasfit_eps_get_sq_or_iq(void)
}
declare 58 generic {
	int sasfit_eps_get_robertus_p(void)
}
declare 59 generic {
	int sasfit_eps_get_iter_4_mc(void)
}
declare 60 generic {
	int sasfit_eps_get_fitorsim(void)
}
declare 61 generic {
	scalar sasfit_eps_get_h(void)
}
declare 62 generic {
	scalar sasfit_eps_get_comp(void)
}
declare 63 generic {
	void sasfit_eps_set_aniso(scalar value)
}
declare 64 generic {
	void sasfit_eps_set_jmax_aniso(int value)
}
declare 65 generic {
	void sasfit_eps_set_res(scalar value)
}
declare 66 generic {
	void sasfit_eps_set_jmax_res(int value)
}
declare 67 generic {
	void sasfit_eps_set_nriq(scalar value)
}
declare 68 generic {
	void sasfit_eps_set_jmax_nriq(int value)
}
declare 69 generic {
	void sasfit_eps_set_sq_or_iq(int value)
}
declare 70 generic {
	void sasfit_eps_set_robertus_p(int value)
}
declare 71 generic {
	void sasfit_eps_set_iter_4_mc(int value)
}
declare 72 generic {
	void sasfit_eps_set_fitorsim(int value)
}
declare 73 generic {
	void sasfit_eps_set_h(scalar value)
}
declare 74 generic {
	void sasfit_eps_set_comp(scalar value)
}
declare 75 generic {
	void sasfit_set_sphavg_strategy(int value)
}
declare 76 generic {
	int sasfit_get_sphavg_strategy(void)
}
declare 77 generic {
	void sasfit_set_int_strategy(int value)
}
declare 78 generic {
	int sasfit_get_int_strategy(void)
}
declare 79 generic {
	scalar sasfit_sd_LogNorm(scalar value, sasfit_param * param)
}
declare 80 generic {
	int sasfit_miev0(real * xx, complex * crefin, logical * perfct, real * mimcut, logical * anyang, integer * numang, real * xmu, integer * nmom, integer * ipolzn, integer * momdim, logical * prnt, real * qext, real * qsca, real * gqsc, real * pmom, complex * sforw, complex * sback, complex * s1, complex * s2, complex * tforw, complex * tback, real * spike)
}
declare 81 generic {
	int sasfit_dmilay(real *rcore, real *rshell, real *wvno, complex *rindsh, complex *rindco, real *mu, integer *numang, real *qext, real *qsca, real *qbs, real *gqsc, real *m1, real *m2, real *s21, real *d21, integer *maxang)
}
declare 82 generic {
	doublereal sasfit_th36(real * x, sasfit_param * param, char * thnam, char * parnam, integer * npar, integer * ini, ftnlen thnam_len, ftnlen parnam_len)
}
declare 83 generic {
	int sasfit_f2c_errmsg_(char *messag, logical *fatal, ftnlen messag_len)
}
declare 84 generic {
	logical sasfit_f2c_tstbad_(char *varnam, real *relerr, ftnlen varnam_len)
}
declare 85 generic {
	logical sasfit_f2c_wrtbad_(char *varnam, ftnlen varnam_len)
}
declare 86 generic {
	logical sasfit_f2c_wrtdim_(char *dimnam, integer *minval, ftnlen dimnam_len)
}
declare 87 generic {
	doublereal sasfit_struve_h0(doublereal *xvalue)
}
declare 88 generic {
	doublereal sasfit_struve_h1(doublereal *xvalue)
}
declare 89 generic {
	int zspow_(sasfit_func_zspow_t * fcn, integer * nsig, integer * n,	integer * itmax, real * par, real * x, real * fnorm, real * wk, integer * ier)
}
declare 90 generic {
	int sasfit_robertus_deal(doublereal * rm, doublereal * sig, doublereal * phi, logical * labdas)
}
declare 91 generic {
	int sasfit_robertus_calc(sasfit_param * param, doublereal * rm, doublereal * sig, doublereal * phi)
}
declare 92 generic {
	int sasfit_robertus_subint(sasfit_param * param, doublereal * q, doublereal * ai, doublereal * aint0)
}
declare 93 generic {
	double sasfit_3f2(double a1, double a2, double a3, double b1, double b2, double x, sasfit_param * param)
}
declare 94 generic {
	double sasfit_2f1(double a1, double a2, double b1, double x, sasfit_param * param)
}
declare 95 generic {
	double sasfit_pfq(double *p_r, double *p_i,  double *q_r, double *q_i, int ip, int iq,
                  double z_r, double z_i, int ln_pFq, int ix,
                  double *pFq_r, double *pFq_i, int nsigfig, sasfit_param * param)
}
declare 96 generic {
	int sasfit_wofz(double * xi, double * yi, double * u, double * v, long * flag__)
}
declare 97 generic {
	double re_cerf_z(double x, double y)
}
declare 98 generic {
	double im_cerf_z(double x, double y)
}
declare 99 generic {
	double re_cerfi_z(double x, double y)
}
declare 100 generic {
	double im_cerfi_z(double x, double y)
}
declare 101 generic {
	double re_w_of_z(double x, double y)
}
declare 102 generic {
	double im_w_of_z(double x, double y)
}
declare 103 generic {
	void w_of_z_e(double x, double y, double * rew, double * imw)
}

declare 104 generic {
	scalar sasfit_jinc(scalar x)
}

declare 105 generic {
	scalar find_LogNorm_int_range(scalar dim, scalar x0, scalar sigma, scalar *Xstart, scalar *Xend, sasfit_param *param)
}
declare 106 generic {
	void hcubature(unsigned fdim, integrand f, void *fdata,
	      unsigned dim, const double *xmin, const double *xmax,
	      size_t maxEval, double reqAbsError, double reqRelError,
	      error_norm norm,
	      double *val, double *err)
}
declare 107 generic {
	void pcubature (unsigned fdim, integrand f, void *fdata,
	      unsigned dim, const double *xmin, const double *xmax,
	      size_t maxEval, double reqAbsError, double reqRelError,
	      error_norm norm,
	      double *val, double *err)
}
declare 108 generic {
	void sasfit_intccini(int lenw, double *w)
}
declare 109 generic {
	void sasfit_intcc(sasfit_func_int_t * f, double a, double b, double eps, int lenw, double *w, double *i, double *err, void *fparams)
}
declare 110 generic {
	void sasfit_intdeiini(int lenaw, double tiny, double eps, double *aw)
}
declare 111 generic {
	void sasfit_intdei(sasfit_func_int_t * f, double a, double *aw, double *i, double *err, void *fparams)
}
declare 112 generic {
	void sasfit_intdeoini(int lenaw, double tiny, double eps, double *aw)
}
declare 113 generic {
	void sasfit_intdeo(sasfit_func_int_t * f, double a, double omega, double *aw, double *i, double *err, void *fparams)
}
declare 114 generic {
	void sasfit_intdeini(int lenaw, double tiny, double eps, double *aw)
}
declare 115 generic {
	void sasfit_intde(sasfit_func_int_t * f, double a, double b, double *aw, double *i, double *err, void *fparams)
}
declare 116 generic {
    int sasfit_cubature(size_t ndim,
			scalar *int_start,
			scalar *int_end,
			sasfit_func_ndim_t *intKern_fct,
			void * param,
			scalar epsrel,
			scalar *result,
			scalar *error)
}
declare 117 generic {
	scalar sasfit_erfinv(scalar y)
}

declare 118 generic {
	int sasfit_available_table ( int rule )
}

declare 119 generic {
	int sasfit_order_table ( int rule )
}

declare 120 generic {
	void sasfit_ld_by_order ( int order, double *x, double *y, double *z, double *w )
}

declare 121 generic {
	int sasfit_eps_get_gausslegendre(void)
}
declare 122 generic {
	int sasfit_eps_get_chebyshev1(void)
}
declare 123 generic {
	int sasfit_eps_get_chebyshev2(void)
}
declare 124 generic {
	int sasfit_eps_get_gegenbauer(void)
}
declare 125 generic {
	int sasfit_eps_get_exponential(void)
}
declare 126 generic {
	int sasfit_eps_get_laguerre(void)
}
declare 127 generic {
	int sasfit_eps_get_jacobi(void)
}
declare 128 generic {
	int sasfit_eps_get_lebedev(void)
}
declare 129 generic {
	int sasfit_eps_get_fibonacci(void)
}
declare 130 generic {
	scalar sasfit_eps_get_alpha(void)
}
declare 131 generic {
	scalar sasfit_eps_get_beta(void)
}
declare 132 generic {
	void sasfit_eps_set_alpha(scalar value)
}
declare 133 generic {
	void sasfit_eps_set_beta(scalar value)
}
declare 134 generic {
	int sasfit_eps_get_spherical_t_design(void)
}
declare 135 generic {
	int sasfit_available_sph_t_table(int rule)
}
declare 136 generic {
	int sasfit_sph_t_order_table(int rule)
}
declare 137 generic {
	void sasfit_sph_t_by_order( int order, double *x, double *y, double *z)
}
declare 138 generic {
	scalar sasfit_hankel(int algorithm, double nu, double (*f)(double q, void *par), double x, void *fparams)
}
