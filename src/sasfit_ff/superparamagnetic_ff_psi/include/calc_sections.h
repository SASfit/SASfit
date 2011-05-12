/*
 * src/sasfit_ff/superparamagnetic_ff_psi/include/calc_sections.h
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

#ifndef CALC_SECTIONS_H
#define CALC_SECTIONS_H

#include "calc_bt.h"
#include "calc_sq.h"
#include "calc_alpha.h"

/*
 * Sets variables which required prior values from the input parameter
 */
#define SET_VARS_PRIOR \
\
	V[KMAG] = 1.0/V[KNUC_KMAG]; \
	V[PHI0] = M_PI*V[PHI0DEG]/180.0

/*
 * Variables required by SOME_NAME_FCT
 */
#define SOME_NAME_FCT_VARS \
		scalar res2 = 0.0; \
		scalar ppsi, sin2, sin4, pm, ipp, imm, imp, ipm


/*
 * A core calculation which occurs in all superparamagnetic_ff_psi form factors.
 * Please, someone may provide a proper name for this thing.
 */
#define SOME_NAME_FCT \
	do { \
		ppsi = sasfit_param_override_get_psi( V[PSI]/180.0*M_PI ); \
\
		sin2 = pow(sin(ppsi),2.0); \
		sin4 = pow(sin(ppsi),4.0); \
\
		pm  = -1.0; \
		ipp = pow(V[KMAG]*(V[L]-V[GAMMA])*sin2+pm*V[KNUC],2.0)*V[SQ_TMP] \
			 +V[KMAG]*V[KMAG]*(V[LA]*sin2-(V[L]*V[L]-1.0+3.0*V[LA])*sin4); \
		pm  = +1.0; \
		imm = pow(V[KMAG]*(V[L]-V[GAMMA])*sin2+pm*V[KNUC],2.0)*V[SQ_TMP] \
			 +V[KMAG]*V[KMAG]*(V[LA]*sin2-(V[L]*V[L]-1.0+3.0*V[LA])*sin4); \
		imp = (sin2-sin4)*pow(V[L]-V[GAMMA],2.0)*V[KMAG]*V[KMAG]*V[SQ_TMP] \
			+ V[KMAG]*V[KMAG]*((sin4-sin2)*(V[L]*V[L]-1.0+3.0*V[LA])+(2.0-sin2)*V[LA]); \
		ipm = imp; \
		res2 = (1.-V[POL]*V[BT_SIGN])/2. * (imm+imp) + (1.+V[POL]*V[BT_SIGN])/2. * (ipp+ipm); \
	} while(0)


/*
 * Variables required by CALC_MAIN_BODY and nested macros.
 * see SOME_NAME_FCT
 */
#define MAIN_BODY_VARS \
		SOME_NAME_FCT_VARS; \
		sasfit_param subParam2


/*
 * The main calculation of all the superparamagnetic_ff_psi_strobo
 * form factors inside the loop.
 * SET_BT	A macro which computes the bt value in V[BT].
 * SET_ALPHA	A macro which computes the alpha value in V[ALPHA].
 * SET_SQ	A macro which computes the sq value (repeatedly in the loop) in V[SQ].
 * TRIANGULAR_PARAM	A macro which sets the parameter for calculation of the size distribution.
 */
#define CALC_MAIN_BODY \
	do { \
		SET_BT; \
		SET_ALPHA; \
		SET_L_LA; \
\
		SET_SQ; \
\
		SOME_NAME_FCT; \
\
		sasfit_init_param( &subParam2 ); \
		TRIANGULAR_PARAM; \
		res2 = res2*sasfit_sd_Triangular(V[LAMB], &subParam2); \
		SASFIT_CHECK_SUB_ERR(param, subParam2); \
\
	} while(0)


/*
 * Initialization of variables required by the inner loop of each 
 * superparamagnetic_ff_psi_strobo form factor.
 */
#define LOOP_VARS \
		scalar res = 0.0, res1 = 0.0; \
		scalar lamb_min; \
		int i, ndl; \
		MAIN_BODY_VARS


/*
 * Calculation of the inner loop of each superparamagnetic_ff_psi_strobo
 * form factor and assumes that all variables are already defined and set to reasonable
 * values.
 */
#define CALC_LOOP \
	do { \
		ndl 	= (int)V[NDL]; \
\
		lamb_min = V[LAMBDA] * (1.0 - V[D_LAMBDA] ); \
\
		for (i=0; i <= 2*ndl ;i++) \
		{ \
			res1 = res2; \
			V[LAMB] = lamb_min + i * V[LAMBDA] * V[D_LAMBDA] / V[NDL]; \
			CALC_MAIN_BODY; \
			if ( subParam.errStatus != FALSE ) return res; \
			res = res + (res1+res2) * 0.5 * V[LAMBDA] * V[D_LAMBDA] / V[NDL] * pow(V[R]/V[R_AV],6.0); \
\
		} \
	} while(0)


/*
 * Header which is written directly into the form factor source file (strobo versions).
 * Variable definitions have to be on top.
 */
#define HEADER_FF_PSI_STROBO \
\
	LOOP_VARS; \
	INIT_SUBPARAM; \
	SET_COMMON_VARS; \
	SASFIT_ASSERT_PTR( param )


/*
 * Header which is written directly into the form factor source file. (non-strobo versions)
 * Variable definitions have to be on top.
 */
#define HEADER_FF_PSI \
\
	scalar mag_sh, mag_matrix; \
	MAIN_BODY_VARS; \
	INIT_SUBPARAM; \
	SET_COMMON_VARS; \
	SASFIT_ASSERT_PTR( param )


/*
 * The main calculation body of all superparamagnetic_ff_psi form factors 
 * (not for *strobo)
 */
#define SUPER_PARAMAGNETIC_FF_PSI_BODY \
	do { \
		SASFIT_CHECK_COND1((V[T] < 0.0), param, "q(%lg) < 0",V[T]); \
		SASFIT_CHECK_COND1((V[R] < 0.0), param, "R(%lg) < 0",V[R]); \
		SASFIT_CHECK_COND1((V[R_AV] <= 0.0), param, "R0(%lg) <= 0",V[R_AV]); \
		SASFIT_CHECK_COND1((fabs(V[POL]) > 1.0), param, "fabs(pol)(%lg) > 1",fabs(V[POL])); \
\
		SET_ALPHA_PLAIN; \
		SET_L_LA; \
		mag_sh = 0.0; \
		mag_matrix = 0.0; \
		SET_SQ_ONE; \
\
		sasfit_init_param( &subParam2 ); \
		subParam2.p[0] = V[R] + V[D_R]; \
		subParam2.p[1] = V[R]; \
		subParam2.p[2] = V[NUC_SH] - V[NUC_MAT]; \
		subParam2.p[3] = V[KNUC] - V[NUC_MAT]; \
		V[KNUC] = sasfit_ff_Ksh(V[T], &subParam2); \
		SASFIT_CHECK_SUB_ERR(param, subParam2); \
\
		subParam2.p[2] = mag_sh - mag_matrix; \
		subParam2.p[3] = V[KMAG] - mag_matrix; \
		V[KMAG] = sasfit_ff_Ksh(V[T], &subParam2 ); \
		SASFIT_CHECK_SUB_ERR(param, subParam2); \
\
		SOME_NAME_FCT; \
\
		SASFIT_CHECK_SUB_ERR(param, subParam); \
	} while(0)


#endif

