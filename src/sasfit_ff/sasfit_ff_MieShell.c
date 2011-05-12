/*
 * src/sasfit_ff/sasfit_ff_MieShell.c
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

#include <gsl/gsl_math.h>
#include "include/sasfit_ff_utils.h"
#include <sasfit_utils_f2c.h>


/*
double MieShell(Tcl_Interp *interp,
		double Q, 
		  double R,
		double dR,
		double lambda_solvent,
		  double mc_re,
		double mc_im,
		  double ms_re,
		double ms_im,
		double pol,
		  bool  *error)
 * form factor of a sphere using Mie scattering formula
 */
scalar sasfit_ff_MieShell(scalar q, sasfit_param * param)
{
	real res, Theta,xx[1];
	complex crefin_core;
	complex crefin_shell;
	real mimcut;
	logical anyang;
	integer numang;
	real Rc,Rs;
	real qext, qsca, gqsc;
	real m1[2],m2[2],s21[2],d21[2];
	real i1,i2;
	int  indx;
	real waveno,qbs;
	real xmu[1];
	scalar Qmax,pol_ang;
	scalar R, dR, lambda_solvent, mc_re, mc_im, ms_re, ms_im, pol;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 8, &R, &dR, &lambda_solvent, &mc_re, &mc_im, &ms_re, &ms_im, &pol);

	res = 0.0;
	Rc = (real)R;
	Rs = (real)(R+dR);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R);
	SASFIT_CHECK_COND1((lambda_solvent < 0.0), param, "lambda_solvent(%lg) < 0",lambda_solvent);

	waveno = (real)(2.0*M_PI/lambda_solvent);
	Qmax = (4.0*M_PI/lambda_solvent);

	SASFIT_CHECK_COND2((q == Qmax), param, "q(%lg) == Qmax(%lg)",q,Qmax);

	Theta = (real)(2.0*asin(q*lambda_solvent/(4.0*M_PI)));
	xx[0] = (real) (2.0*M_PI*R/lambda_solvent);
	xmu[0] = (real) cos(Theta);

	SASFIT_CHECK_COND1((xmu[0] > 1.0f) || (xmu[0] < -1.0f), param, "xmu[0](%lg) is out of range (-1,1)",xmu[0]);
	SASFIT_CHECK_COND1((xx[0] > 2e4f), param, "xx[0](%lg) > 2e4f",xx[0]);
	SASFIT_CHECK_COND1((mc_re <= 0.0), param, "mc_re(%lg) <= 0",mc_re);
	SASFIT_CHECK_COND1((fabs(mc_re) >= 10), param, "fabs(mc_re)(%lg) >= 10",fabs(mc_re));
	SASFIT_CHECK_COND1((fabs(mc_im) >= 10), param, "fabs(mc_im)(%lg) >= 10",fabs(mc_im));

	if (Theta < M_PI/2.0) 
	{
		indx = 0;
	} else 
	{
		indx = 1;   
		xmu[0] = (real) cos(M_PI-Theta);
	}

	crefin_core.r = (real) mc_re;
	crefin_core.i = (real) mc_im;

	SASFIT_CHECK_COND1((ms_re <= 0.0), param, "ms_re(%lg) <= 0",ms_re);
	SASFIT_CHECK_COND1((fabs(ms_re) >= 10), param, "fabs(ms_re)(%lg) >= 10",fabs(ms_re));
	SASFIT_CHECK_COND1((fabs(ms_im) >= 10), param, "fabs(ms_im)(%lg) >= 10",fabs(ms_im));

	crefin_shell.r = (real) ms_re;
	crefin_shell.i = (real) ms_im;

	mimcut = 1e-6f;
	anyang = TRUE_;
	numang = 1;

	sasfit_dmilay(&Rc,&Rs,&waveno, &crefin_shell, &crefin_core, 
		&xmu[0], &numang, &qext, &qsca, &qbs, &gqsc, 
		m1, m2, s21, d21, &numang);

	i1 = m1[indx];
	i2 = m2[indx];
	pol_ang = (1.0+pol)*M_PI_4;
	res = (real)(cos(pol_ang)*i2+sin(pol_ang)*i1);
	res = (real)(res/(cos(pol_ang)+sin(pol_ang)));
	/*
	if (pol == 0) {
	res = (i1+i2)/2.0;
	} else if (pol > 0) {
	res = i1;
	} else {
	res = i2;
	}
	*/
	return res;
}
