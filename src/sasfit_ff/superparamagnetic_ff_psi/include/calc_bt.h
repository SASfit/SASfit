/*
 * src/sasfit_ff/superparamagnetic_ff_psi/include/calc_bt.h
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

#ifndef CALC_BT_H
#define CALC_BT_H

/*
 * functions to calculate bt
 */

#define T_ACT		(V[T] - V[SD] * V[LAMB]/3956.034613)
#define COS_TERM	(2.*M_PI* V[FREQ]) + V[PHI0]

#define SET_BT_PLAIN \
\
	V[BT] = - V[B0] * cos( T_ACT * COS_TERM )

#define SET_BT_DEFAULT \
\
	SET_BT_PLAIN; \
	V[BT] = V[B1] + V[BT]

#define SET_BT_PPSI \
\
	SET_BT_DEFAULT; \
	V[BEFF] = GSL_SIGN(V[BT]) * (1.0 - V[BS] * cos( T_ACT * COS_TERM - V[PHI0SQ] )) / (1.0+fabs(V[BS])); \
	V[BT] = V[BT] + ( V[B_SQ] * V[BEFF] )

#define SET_BT_ALBR \
\
	SET_BT_DEFAULT; \
	V[BT_SIGN] = GSL_SIGN( V[BT] ); \
	V[BT] = V[B1] - V[B0] * cos( T_ACT * COS_TERM + V[D_PHI] )

#define SET_BT_TISANE \
\
	V[BT] = V[B1] - V[B0] * cos( V[LAMB] * COS_TERM )

#endif

