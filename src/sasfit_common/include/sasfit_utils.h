/*
 * src/sasfit_common/include/sasfit_utils.h
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
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *   Ingo Bressler (ingo@cs.tu-berlin.de)
 */

#ifndef SASFIT_UTILS_H
#define SASFIT_UTILS_H

#include "sasfit_common_shared_exports.h"
#include <gsl/gsl_integration.h>

/**
 * \file sasfit_utils.h
 * \ingroup sasfit_utils
 */

/**
 * \defgroup sasfit_utils Utilities
 * \ingroup sasfit_common
 * Utitily functions often used for implementation of model functions.
 */
/*@{*/

/**
 * \def intabseps
 * Absolute error for integration by \ref sasfit_integrate.
 * Left over from old sasfit code.
 */
#ifdef intabseps
#undef intabseps
#endif
#define intabseps 0.0

/**
 * \def sasfit_integrate
 * Shortcut for integration routine \ref sasfit_integrate_ctm.
 * Assumes 1000 intervals for integration, an absolute error of 0
 * (\ref intabseps) and a relative error as reported by
 * \ref sasfit_eps_get_aniso.
 */
#define sasfit_integrate(a,b,fct,p) sasfit_integrate_ctm(a, b, fct, p, 1000, intabseps, sasfit_eps_get_aniso())

/**
 * \def SASFIT_MAX_WS_PER_THREAD
 * Maximum number of simultaneously integration workspaces per thread.
 * Equals the maximum depth of nesting of integrations.
 */
#define SASFIT_MAX_WS_PER_THREAD 10

/**
 * \def SASFIT_MAX_THREAD
 * Maximum number of simultaneously integration workspaces.
 * Equals the maximum depth of nesting of integrations.
 */
#define SASFIT_MAX_THREAD 100

/**
 * Contains GSL integration workspace information.
 */
typedef struct
{
	int size;			//!< Number of integration intervals.
	gsl_integration_workspace * ptr;//!< Points to GSL integration workspace.
} sasfit_int_workspace_t;

/**
 * A set of several independent integration workspaces.
 */
typedef struct
{
	int last[SASFIT_MAX_THREAD];	//!< Last workspace currently occupied by an integration routine.
	sasfit_int_workspace_t ws[SASFIT_MAX_THREAD][SASFIT_MAX_WS_PER_THREAD]; //!< Set of integration workspaces available.
} sasfit_int_ws_all_t;


// hide direct functions from plugins
#ifndef MAKE_SASFIT_PLUGIN

sasfit_common_DLLEXP scalar sasfit_gammaln(scalar x);
sasfit_common_DLLEXP scalar sasfit_gamma(scalar x);
sasfit_common_DLLEXP scalar sasfit_dgammaln_dx(scalar x);
sasfit_common_DLLEXP scalar sasfit_dgamma_dx(scalar x);
sasfit_common_DLLEXP scalar sasfit_erf_pure(scalar x);
sasfit_common_DLLEXP scalar sasfit_si(scalar x);
sasfit_common_DLLEXP scalar sasfit_log1p(const scalar x);
sasfit_common_DLLEXP void sasfit_polint(scalar xa[], scalar ya[], int n, scalar x, scalar * y, scalar * dy);
sasfit_common_DLLEXP scalar sasfit_qromb(scalar a, scalar b, sasfit_func_one_t * trapzd_fct, sasfit_param * param);
sasfit_common_DLLEXP scalar sasfit_trapzd(scalar a, scalar b, scalar n, sasfit_func_one_t * trapzd_fct, sasfit_param * param);
sasfit_common_DLLEXP scalar sasfit_j1f(scalar x);
sasfit_common_DLLEXP scalar sasfit_bessj1(scalar x);
sasfit_common_DLLEXP scalar sasfit_rwbrush_w(scalar Q, scalar R);
sasfit_common_DLLEXP scalar sasfit_rod_fc(scalar Q, scalar R);
sasfit_common_DLLEXP scalar sasfit_sphere_fc(scalar Q, scalar R);
sasfit_common_DLLEXP scalar sasfit_gauss_fc(scalar Q, scalar R);
sasfit_common_DLLEXP scalar sasfit_g(scalar fp, scalar a);
sasfit_common_DLLEXP void sasfit_intcc(double (*f)(double, void *), double a, double b, double eps, int lenw, double *w, double *i, double *err, void *fparams);
sasfit_common_DLLEXP void sasfit_intccini(int lenw, double *w);
sasfit_common_DLLEXP void sasfit_intdeo(double (*f)(double, void *), double a, double omega, double *aw, double *i, double *err, void *fparams);
sasfit_common_DLLEXP void sasfit_intdeoini(int lenaw, double tiny, double eps, double *aw);
sasfit_common_DLLEXP void sasfit_intdei(double (*f)(double, void *), double a, double *aw, double *i, double *err, void *fparams);
sasfit_common_DLLEXP void sasfit_intdeiini(int lenaw, double tiny, double eps, double *aw);
sasfit_common_DLLEXP void sasfit_intde(double (*f)(double, void *), double a, double b, double *aw, double *i, double *err, void *fparams);
sasfit_common_DLLEXP void sasfit_intdeini(int lenaw, double tiny, double eps, double *aw);

/**
 * Returns the shared library prefix for the current platform.
 */
sasfit_common_DLLEXP const char * sasfit_get_lib_prefix(void);

/**
 * Returns the shared library suffix for the current platform.
 */
sasfit_common_DLLEXP const char * sasfit_get_lib_suffix(void);

/**
 * Occupies an integration workspace with a capacity of at least \e size intervals.
 * \param size Number of intervals.
 */
sasfit_common_DLLEXP void sasfit_int_occupy(int size, int thid);

/**
 * Releases an integration workspace no be used by the next integration routine.
 */
sasfit_common_DLLEXP void sasfit_int_release(int thid);

/**
 * Returns a pointer to the GSL integration workspace.
 */
sasfit_common_DLLEXP gsl_integration_workspace * sasfit_int_mem(int thid);

/**
 * Frees all GSL integration workspace.
 * \param idx Index of the integration workspace to free.
 * \sa \ref SASFIT_MAX_WS
 */
sasfit_common_DLLEXP void sasfit_int_free(int idx, int thid);

/**
 * Frees all GSL integration workspaces used by SASfit.
 * \note Currently not in use.
 */
sasfit_common_DLLEXP void sasfit_int_ws_all_free(int thid);

/**
 * Performs numerical integration.
 * Use this (or \ref sasfit_integrate) to do numerical integration in your
 * model function.
 * \param int_start Start of the integration interval. Use \e GSL_NEGINF to
 * specify an infinite interval. An appropriate integration routine is
 * selected automatically.
 * \param int_end End of the integration interval. Use \e GSL_POSINF to
 * specify an infinite interval. An appropriate integration routine is
 * selected automatically.
 * \param intKern_fct Function to integrate.
 * \param param Parameter structure which will be forwarded to \e intKern_fct.
 * \param limit Number of integration intervals ('grid size')
 * \param epsabs Absolute error boundary.
 * \param epsrel Relative error boundary.
 * \returns Result of the integration.
 * \sa \ref sasfit_integrate
 */
sasfit_common_DLLEXP scalar sasfit_integrate_ctm(scalar int_start, scalar int_end, sasfit_func_one_t intKern_fct, sasfit_param * param, int limit, scalar epsabs, scalar epsrel);


sasfit_common_DLLEXP void sasfit_int_ws_init(void);
/*@}*/
#endif // MAKE_SASFIT_PLUGIN
#endif // file

