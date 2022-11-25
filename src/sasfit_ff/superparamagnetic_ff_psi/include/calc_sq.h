/*
 * src/sasfit_ff/superparamagnetic_ff_psi/include/calc_sq.h
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

#ifndef CALC_SQ_H
#define CALC_SQ_H

/*
 * functions for calculation of sq for use in the loop
 */

#define SET_SQ_ONE \
\
	V[SQ_TMP] = 1.0

#define SET_SQ_B1 \
\
	V[SQ_TMP] = ( 1.0+(V[SQ]-1.0)*fabs( V[BT]/(fabs(V[B1])+2./M_PI*fabs(V[B0])) ))

#define SET_SQ_PPSI \
\
	V[SQ_TMP] = ( 1.0 + (V[SQ]-1.0) * fabs(V[BEFF]) )

#define SET_SQ_LX \
\
	V[SQ_TMP] = ( 1.0 + (V[SQ]-1.0) * fabs(V[L]) )

#define SET_SQ_L2X \
\
	V[SQ_TMP] = ( 1.0 + (V[SQ]-1.0) * V[L]*V[L] )

#define SET_SQ_IDENT \
\
	V[SQ_TMP] = V[SQ]

#endif

