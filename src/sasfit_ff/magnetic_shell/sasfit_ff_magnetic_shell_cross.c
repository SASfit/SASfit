/*
 * src/sasfit_ff/magnetic_shell/sasfit_ff_magnetic_shell_cross.c
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

#include "include/private.h"

scalar sasfit_ff_magnetic_shell_cross(scalar q, sasfit_param * param)
{

    PSI=M_PI_2;
	MAGNETIC_SHELL_HEADER;

	subParam.p[0] = R + D_R;
	subParam.p[1] = R;
	subParam.p[2] = NUC_SH - NUC_MATRIX;
	subParam.p[3] = NUC_C  - NUC_MATRIX;
	knuc = sasfit_ff_Ksh(Q, &subParam );
	SASFIT_CHECK_SUB_ERR(param, subParam);
	subParam.p[2] = MAG_SH - MAG_MATRIX;
	subParam.p[3] = MAG_C  - MAG_MATRIX;
	kmag = sasfit_ff_Ksh(Q, &subParam );
	SASFIT_CHECK_SUB_ERR(param, subParam);


    PSI2D = sasfit_param_override_get_psi ( PSI/180.0*M_PI );
    sin2 = pow(sin(PSI2D),2.0);
	res = POL*4.0*knuc*kmag*sin2;

	return res;
}

