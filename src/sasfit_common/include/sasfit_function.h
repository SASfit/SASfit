/*
 * src/sasfit_common/include/sasfit_function.h
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
 *   Ingo Bressler (ingo@cs.tu-berlin.de)
 */

#ifndef SASFIT_FUNCTION_H
#define SASFIT_FUNCTION_H

#include <stdarg.h>
#include "sasfit_common_shared_exports.h"

/**
 * \file sasfit_function.h
 * \ingroup sasfit_function
 */

/**
 * \defgroup sasfit_function Model Function Definitions and Structures
 * \ingroup sasfit_common
 * Utilities and data structures for handling model functions (\ref form_fac,
 * \ref struct_fac and \ref size_distrib) and their parameters.
 */
/*@{*/

/**
 * \def EMPTY
 * Dummy parameter variable.
 * \sa sasfit_get_param
 */
#define EMPTY	(scalar*)0

/**
 * Used to select the appropriate flavor of a form factor which does
 * internal calculations.
 */
typedef enum
{
	CYLSHELL1,
	CYLSHELL2,
	CYL_RWBRUSH,	CYL_RWBRUSH_RC, 	CYL_RWBRUSH_NAGG,
	DISC_RWBRUSH,	DISC_RWBRUSH_LC, 	DISC_RWBRUSH_NAGG,
	DOZIER_STAR,
	DOZIER_STAR2,
	ELLIP_CORE_SH,
	ELLIP_CORE_SH_F,
	ELLIP_RWBRUSH,	ELLIP_RWBRUSH_RC, 	ELLIP_RWBRUSH_NAGG,
	ELLIP1,
	ELLIP2,
	GAUSS1,
	GAUSS2,
	GAUSS3,
	GAUSSPOLY,
	IMC_CROSS,
	IMC_ANISO,
	IMC_PSI,
	A_FCT,
	B_FCT,
	JCS_INT_V,
	JCS_INT_FF,
	KSHLIN1,
	KSHLIN2,
	LANGEVIN1,
	LANGEVIN2,
	ROD_EXP,	ROD_EXP_RC, 		ROD_EXP_NAGG,
	ROD_GAUSS,	ROD_GAUSS_RC, 		ROD_GAUSS_NAGG,
	ROD_RMA,	ROD_RMA_RC, 		ROD_RMA_NAGG,
	ROD_RWBRUSH,	ROD_RWBRUSH_RC,		ROD_RWBRUSH_NAGG,
	CYL_SHRD,
	CYL_SHRD_TH,
	CYL_SHRD_GAUSS,
	CYL_SHRD_GAUSS_TH,
	SPH_SHELL1,
	SPH_SHELL2,
	SPH_SHELL3,
	SPHERE_RMA,	SPHERE_RMA_RC,		SPHERE_RMA_NAGG,
	SPHERE_RMA_RC_SMOOTH,	SPHERE_RMA_NAGG_SMOOTH,
	SPHERE_RMA_MANU1,
	SPHERE_RMA_MANU2,
	SPHERE_RWBRUSH,	SPHERE_RWBRUSH_RC,	SPHERE_RWBRUSH_NAGG,
	SPHERE_SAW,	SPHERE_SAW_RC,		SPHERE_SAW_NAGG,
	SPMAG_ANISO,
	SPMAG_CROSS,
	SPMAG_ISO,
	SPM_STROBO_SQ_PPSI,
	SPM_STROBO_SQ_Lx,
	SPM_STROBO_SQ_L2x,
	SPM_STROBO_POL_SQ_L2x,
	SPM_STROBO_POL_SQ_L2modx,
	SPM_STROBO_SQ_BT1,
	SPM_STROBO_SQ,
	SPM_STROBO,
	SPM_STROBO2,
	WORM_RMA,	WORM_RMA_RC,		WORM_RMA_NAGG,
	WORM_RWBRUSH,	WORM_RWBRUSH_RC,	WORM_RWBRUSH_NAGG,
	DEFAULT
} sasfit_kernel_type;

/**
 * Describes the type of function.
 */
typedef enum
{
	SDfct = 0,
	SQfct = 1,
	FFfct = 2
} sasfit_function_type;

/**
 * A set of parameters used by every model function in sasfit.
 */
typedef struct
{
	scalar             p[MAXPAR];      //!< Parameter of a function.
	sasfit_kernel_type kernelSelector; //!< Selects the kernel function to use, when used with gsl functions.
	sasfit_kernel_type kernelSubSelector; //!< ??
	char               errStr[STRLEN]; //!< An error message.
	int                errLen;         //!< Length of the error message.
	bool               errStatus;      //!< True, if an error occured. False otherwise.
	double             *xarr;            //!< hack for OZ solver
	double             *yarr;            //!< hack for OZ solver
} sasfit_param;

/**
 * Data structure to override single parameter values.
 * This is a temporary solution, because probably it gets obsolete when
 * fitting algo is reviewed ...
 */
typedef struct
{
	int    override; //!< Flag to enable overriding.
	scalar value;    //!< New value.
	scalar value2;   //!< New value.
	scalar value3;   //!< New value.
	scalar value4;    //!< New value.
} sasfit_param_override_t;

/**
 * Prototype of a function without single-value arguments.
 */
typedef scalar  sasfit_func_zero_t (sasfit_param *);
/**
 * Prototype of a function with one single-value argument.
 */
typedef scalar  sasfit_func_one_t (scalar, sasfit_param *);
/**
 * Prototype of a monte carlo function with one array-value argument.
 */
typedef scalar  sasfit_func_monte_t (scalar *, size_t, sasfit_param *);
/**
 * Prototype of a volume function with one single-value argument.
 */
typedef scalar  sasfit_func_vol_t (scalar, sasfit_param *, int);
/**
 * Prototype of a function with two single-value arguments.
 */
typedef scalar  sasfit_func_two_t (scalar, scalar, sasfit_param *);

/**
 * Prototype of an alternate output (printf) function.
 */
typedef void    sasfit_func_msg_t (const char * format, va_list arglist);

/**
 * Represents a model function of any type.
 */
typedef struct
{
	char    typestr[STRLEN];
	int     typeLen;
	sasfit_func_one_t * fct;   //!< Default function
	sasfit_func_one_t * fct_f; //!< not-squared function
	sasfit_func_vol_t * fct_v; //!< Volume function
	bool    compute_f;
	int     F_identifier;      //!< ??
	int     V_identifier;      //!< ??
	sasfit_param params;       //!< Function parameters
} sasfit_function;

// hide direct functions from plugins
#ifndef MAKE_SASFIT_PLUGIN

/** Initialize all parameter override data structures. */
sasfit_common_DLLEXP void sasfit_param_override_init (void);

/**
 * Sets the new wavelength \f$ \lambda\f$ of an anisotropic model function.
 * Enables override of previous parameter value.
 * \param new_value New wavelength \f$ \lambda \f$.
 */
sasfit_common_DLLEXP void sasfit_param_override_set_lambda (scalar new_value);

/**
 * Overrides the wavelength \f$ \lambda \f$ of an anisotropic model function, if
 * enabled.
 * \param default_val Default parameter value.
 * \returns Desired wavelength.
 */

sasfit_common_DLLEXP scalar sasfit_param_override_get_lambda (scalar default_val);
/**
 * Sets the new angle \f$ 2\theta\f$ of an anisotropic model function.
 * Enables override of previous parameter value.
 * \param new_value New angle \f$ 2\theta \f$.
 */
sasfit_common_DLLEXP void sasfit_param_override_set_2theta (scalar new_value);

/**
 * Overrides the angle \f$ 2\theta \f$ of an anisotropic model function, if
 * enabled.
 * \param default_val Default parameter value.
 * \returns Desired angle value.
 */

sasfit_common_DLLEXP scalar sasfit_param_override_get_2theta (scalar default_val);

/**
 * Sets the new angle \f$ \psi_2 \f$ of an anisotropic model function.
 * Enables override of previous parameter value.
 * \param new_value New angle \f$ \psi_2 \f$.
 */
sasfit_common_DLLEXP void sasfit_param_override_set_psi2 (scalar new_value);

/**
 * Overrides the angle \f$ \psi_2 \f$ of an anisotropic model function, if
 * enabled.
 * \param default_val Default parameter value.
 * \returns Desired angle value.
 */

sasfit_common_DLLEXP scalar sasfit_param_override_get_psi2 (scalar default_val);
/**
 * Sets the new angle \f$ \psi \f$ of an anisotropic model function.
 * Enables override of previous parameter value.
 * \param new_value New angle \f$ \psi \f$.
 */
sasfit_common_DLLEXP void sasfit_param_override_set_psi (scalar new_value);

/**
 * Overrides the angle \f$ \psi \f$ of an anisotropic model function, if
 * enabled.
 * \param default_val Default parameter value.
 * \returns Desired angle value.
 */
sasfit_common_DLLEXP scalar sasfit_param_override_get_psi (scalar default_val);

/**
 * Initializes a given sasfit_function.
 * \param f Pointer to a sasfit_function.
 * \sa sasfit_function
 */
sasfit_common_DLLEXP void sasfit_init_fct(sasfit_function * f);

/**
 * Initializes a given sasfit_param for use with a sasfit_function.
 * \param param Pointer to a sasfit_param.
 * \sa sasfit_param
 */
sasfit_common_DLLEXP void sasfit_init_param(sasfit_param * param);

/**
 * Copies a parameter values from a sasfit_param structure to a supplied
 * list of variables (pointers to scalar).
 *
 * Example:
 * \code
 * scalar a, b, c;
 * sasfit_param param;
 * param.p[0] = 5;
 * param.p[1] = 7;
 * param.p[2] = 11;
 * param.p[3] = 9;
 * sasfit_get_param(3, &param, EMPTY, &a, &b, &c);
 * // a == 7 && b == 11 && c == 9
 * \endcode
 *
 * EMPTY is defined as (scalar*)NULL
 *
 * \param param Pointer to a sasfit_param containing parameter values.
 * \param num Number of values (i.e. variables) to read in.
 */
sasfit_common_DLLEXP void sasfit_get_param(sasfit_param * param, int num, ...);

/**
 * Prints the supplied parameter array to stderr.
 * \param param Parameter array to print.
 */
sasfit_common_DLLEXP void sasfit_print_param(sasfit_param * param);

/**
 * Copies function parameters from an arbitrary data array to a given
 * sasfit_param structure.
 * WARNING: Ensure that array a has em{at least} MAXPAR entries !
 * \param param Pointer to a sasfit_param.
 * \param a Pointer to data array.
 * \sa sasfit_param
 */
sasfit_common_DLLEXP void sasfit_copy_param(sasfit_param * param, scalar * a);

/**
 * Returns the maximum number of parameter values.
 */
sasfit_common_DLLEXP int sasfit_get_maxpar(void);

/**
 * Tests if a function is valid for evaluation.
 * \param f Pointer to a sasfit_function.
 * \returns 1, if at least the fct pointer, its parameters and its typestr is not null.
 * 	    0, otherwise.
 * \sa sasfit_function
 */
sasfit_common_DLLEXP int sasfit_valid_fct(sasfit_function * f);

/**
 * Partially differentiates (first order) the supplied function using a numerical approach.
 * \param x Position where the function will be evaluated.
 * \param f Pointer to a (well defined) sasfit_function structure.
 * \param dparam Index of the variable to differentiate minus one.
 *               (dparam == 0 means no differentiation at all)
 * \sa sasfit_function
 */
sasfit_common_DLLEXP scalar sasfit_part_diff_fct(scalar x, sasfit_function * f, int dparam);

/*@}*/
#endif // MAKE_SASFIT_PLUGIN
#endif // file

