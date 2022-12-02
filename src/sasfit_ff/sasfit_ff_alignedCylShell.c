/*
 * src/sasfit_ff/sasfit_ff_alignedCylShell.c
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
 *   Ingo Bressler (ingo.bressler@bam.de)
 */

#include <gsl/gsl_math.h>
#include <gsl/gsl_sf.h>
#include "include/sasfit_ff_utils.h"

#define R			param->p[0]
#define DR			param->p[1]
#define L			param->p[2]
#define ETA_C		param->p[3]
#define ETA_SH		param->p[4]
#define ETA_SOL		param->p[5]
#define PSI_DEG		param->p[6]
#define THETA_DEG	param->p[7]
#define PHI_DEG		param->p[8]

scalar sasfit_ff_alignedCylShell(scalar q, sasfit_param * param)
{
	scalar gama, psi, theta, phi;

	SASFIT_ASSERT_PTR(param);

//	sasfit_get_param(param, 9, &R, &DR, &L, &eta_core, &eta_shell, &eta_solv, &psi_deg, &theta_deg, &phi_deg);

	psi   = sasfit_param_override_get_psi(PSI_DEG * M_PI/180.);
	theta = THETA_DEG * M_PI/180.;
	phi   = PHI_DEG * M_PI/180.;

	gama = acos( sin(theta) * cos(phi) * cos(psi) + cos(theta) * sin(psi) );

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((L < 0.0), param, "L(%lg) < 0",L);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R);
	SASFIT_CHECK_COND1((DR < 0.0), param, "DR(%lg) < 0",DR);

	if ( q == 0.0 )
	{
		return pow( (ETA_C - ETA_SH)*R*R*L*M_PI + (ETA_SH - ETA_SOL)*(R+DR)*(R+DR)*L*M_PI, 2);
	}
	if (L == 0.0)
	{
		return 0.0;
	}
	if (R+DR == 0.0)
	{
		return 0.0;
	}

	if (gama == M_PI/2.0) 
	{
		return pow(2.*gsl_sf_bessel_J1(q*R     )/q *L*   R  *M_PI*(ETA_C-ETA_SH) +
		  	       2.*gsl_sf_bessel_J1(q*(R+DR))/q *L*(R+DR)*M_PI*(ETA_SH-ETA_SOL)  ,2);
	} 
	else if (gama == 0.0) 
	{
		return pow(2./q*R     *R     *sin(q*L*0.5)*(ETA_C-ETA_SH)*M_PI+
			       2./q*(R+DR)*(R+DR)*sin(q*L*0.5)*(ETA_SH-ETA_SOL)*M_PI  ,2);
	} 
	else 
	{
		return pow(4.*(ETA_C-ETA_SH)  *M_PI* R    *gsl_sf_bessel_J1(q* R    *sin(gama))*sin(q*L*cos(gama)/2.)*pow(q,-2.)/sin(gama)/cos(gama)+
			       4.*(ETA_SH-ETA_SOL)*M_PI*(R+DR)*gsl_sf_bessel_J1(q*(R+DR)*sin(gama))*sin(q*L*cos(gama)/2.)*pow(q,-2.)/sin(gama)/cos(gama) ,2);
	}
}

