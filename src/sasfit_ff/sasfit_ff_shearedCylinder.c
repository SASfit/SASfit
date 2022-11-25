/*
 * src/sasfit_ff/sasfit_ff_shearedCylinder.c
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
#include <gsl/gsl_sf.h>
#include "include/sasfit_ff_utils.h"

scalar sasfit_ff_shearedCylinder_i_core(scalar x, sasfit_param * param)
{
	scalar R, L, Gamma;

        SASFIT_ASSERT_PTR(param);

	R	= param->p[0];
	L	= param->p[1];
	Gamma	= param->p[2];

	if ((R == 0.0) || (L == 0.0))
	{
		return 0.0;
	} 
	else if (x == 0.0) 
	{
		return pow(M_PI*R*R*L, 2.0);
	} 
	else if (Gamma == M_PI/2.0) 
	{
		return 4.0*pow(gsl_sf_bessel_J1(x*R)*L*M_PI*R/x,2.0);
	} 
	else if (Gamma == 0.0) 
	{
		return 4.0*pow(sin(0.5*x*L)*M_PI*R*R/x,2.0);
	} else 
	{
		return 16.0 * pow(M_PI*R*R*L,2.0)*pow(gsl_sf_bessel_J1(x*R*sin(Gamma))
			* sin(x*L*cos(Gamma)/2.0)
			/ (x*x*R*sin(Gamma)*L*cos(Gamma))
			,2.0);
	}
}

scalar sasfit_ff_shearedCylinder_p_core(scalar x, sasfit_param * param)
/*
                                 float theta,
                 float phi,
                                 float Gama)
*/
{
	scalar phi, Gamma, theta, twophi0, phi0;

        SASFIT_ASSERT_PTR(param);

	Gamma	= param->p[2];
	phi	= param->p[MAXPAR-2];
	theta 	= x;

        twophi0 = atan(8.0/Gamma);
        phi0 = twophi0/2.0;
        return   (1-cos(twophi0))*pow(1.0+pow(sin(theta),2.0)*cos(twophi0),3.0/2.0)
                  /  (4.0*M_PI*pow(1.0-pow(sin(theta),2.0)*cos(twophi0)*cos(2.0*(phi-phi0)),2.0));
}

scalar sasfit_ff_shearedCylinderGauss_p_core(scalar x, sasfit_param * param)
/*
                                 float theta,
                 float phi,
                                 float theta_bar)
*/
{
	scalar tmp, theta, phi, theta_bar;

        SASFIT_ASSERT_PTR(param);

	theta_bar	= param->p[2];
	phi		= param->p[MAXPAR-2];
	theta 		= x;

        tmp = 0.5*sqrt(M_PI)*theta_bar*gsl_sf_erf(M_PI/theta_bar);
        return exp(-theta*theta/theta_bar/theta_bar)/(tmp);
}

scalar sasfit_ff_shearedCylinder_core(scalar x, sasfit_param * param)
	/*
		Tcl_Interp *interp,
                                 float theta,
                 float phi,
                 float Q,
                 float R,
                 float L,
                                 float Gama,
                                 float psi,
                 bool  *error)
		 */
{
	scalar gamma_p, gamma_m, theta, phi, Gamma, psi, res;

        SASFIT_ASSERT_PTR(param);
	
	Gamma	= param->p[2];
	psi	= sasfit_param_override_get_psi( param->p[3]/180.0*M_PI );
	phi	= param->p[MAXPAR-2];
	theta 	= x;

	gamma_p = acos(sin(theta)*cos(phi)*cos(psi)+cos(theta)*sin(psi));
	gamma_m = acos(sin(theta)*cos(phi)*cos(psi)-cos(theta)*sin(psi));
/*
	return    p_shearedCylinder(theta,phi,Gamma)
		* (  I_shearedCyl(interp,Q,R,L,gamma_p,error)
		+ I_shearedCyl(interp,Q,R,L,gamma_m,error))
		* sin(theta);
*/
	param->p[2] = gamma_p;
	res  = sasfit_ff_shearedCylinder_i_core(param->p[MAXPAR-1], param); // calc with previous Q

	param->p[2] = gamma_m;
	res += sasfit_ff_shearedCylinder_i_core(param->p[MAXPAR-1], param); // calc with previous Q

	param->p[2] = Gamma;
	res *= sasfit_ff_shearedCylinder_p_core(x, param);

	return res * sin(theta);
}

scalar sasfit_ff_shearedCylinderGauss_core(scalar x, sasfit_param * param)
	/*
float I_shearedCylGauss_core(Tcl_Interp *interp,
                                 float theta,
                 float phi,
                 float Q,
                 float R,
                 float L,
                                 float theta_bar,
                                 float psi,
                 bool  *error)
		 */
{
	scalar gamma_p, gamma_m, theta, theta_bar, phi, psi, res;

        SASFIT_ASSERT_PTR(param);

	theta_bar	= param->p[2];
	psi		= sasfit_param_override_get_psi( param->p[3]/180.0*M_PI );
	phi		= param->p[MAXPAR-2];
	theta 		= x;

	gamma_p = acos(sin(theta)*cos(phi)*sin(psi)+cos(theta)*cos(psi));
	gamma_m = acos(sin(theta)*cos(phi)*sin(psi)-cos(theta)*cos(psi));
/*
        return    p_shearedCylinderGauss(theta,phi,theta_bar)
                  * (  I_shearedCyl(interp,Q,R,L,gamma_p,error)
                     + I_shearedCyl(interp,Q,R,L,gamma_m,error)) * sin(theta) ;
	*/
	param->p[2] = gamma_p;
	res  = sasfit_ff_shearedCylinder_i_core(param->p[MAXPAR-1], param); // calc with previous Q

	param->p[2] = gamma_m;
	res += sasfit_ff_shearedCylinder_i_core(param->p[MAXPAR-1], param); // calc with previous Q

	param->p[2] = theta_bar;
	res *= sasfit_ff_shearedCylinderGauss_p_core(x, param);

	return res * sin(theta);
}

/*
float I_shearedCylinder(Tcl_Interp *interp,
				 float Q,
					  float R,
					  float L,
				 float Gama,
				 float psi,
				 float eta,
					  bool  *error)
*/
/*
 * form factor of a spherical Cylinder with radius R, height L and scattering
 * length density eta
 */
scalar sasfit_ff_shearedCylinder(scalar q, sasfit_param * param)
{
	scalar res, R, L, Gamma, eta, theta_bar;

        SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 5, &R, &L, EMPTY, EMPTY, &eta);

	SASFIT_CHECK_COND1((q < 0.0),      param, "q(%lg) < 0", q);
	SASFIT_CHECK_COND1((R < 0.0),      param, "R(%lg) < 0", R);
	SASFIT_CHECK_COND1((L < 0.0),      param, "L(%lg) < 0", L);

	if (R == 0.0) return 0.0;
	if (L == 0.0) return 0.0;

	res = 0.0;

	switch( param->kernelSelector )
	{
		// double integration happens here, hope this still works, need to find an easier-to-read-way to do this
		case CYL_SHRD:
			Gamma	= param->p[2];
			SASFIT_CHECK_COND1((Gamma < 0.0),  param, "Gamma(%lg) < 0", Gamma);

			// Sum = SASFITqrombShearedCylinderPhi(interp,q,R,L,Gama,psi,error);
			param->kernelSelector = CYL_SHRD_TH;
			param->p[MAXPAR-1] = q;	
			res = sasfit_qromb(0.0, 2.0*M_PI, sasfit_ff_shearedCylinder, param);
			param->kernelSelector = CYL_SHRD;

			res = eta*eta*res;
			break;
		case CYL_SHRD_TH:
			Gamma	= param->p[2];
			SASFIT_CHECK_COND1((Gamma < 0.0),  param, "Gamma(%lg) < 0", Gamma);
			param->p[MAXPAR-2] = q; // phi incoming
			res = sasfit_qromb(0.0, M_PI, sasfit_ff_shearedCylinder_core, param);
			break;
		case CYL_SHRD_GAUSS:
			theta_bar = param->p[2];
			SASFIT_CHECK_COND1((theta_bar < 0.0),  param, "theta_bar(%lg) < 0", theta_bar);

			param->kernelSelector = CYL_SHRD_GAUSS_TH;
			param->p[MAXPAR-1] = q;	
			res = sasfit_qromb(0.0, 2.0*M_PI, sasfit_ff_shearedCylinder, param);
			param->kernelSelector = CYL_SHRD_GAUSS;

			res = eta*eta*res;
			break;
		case CYL_SHRD_GAUSS_TH:
			theta_bar= param->p[2];
			SASFIT_CHECK_COND1((theta_bar < 0.0),  param, "theta_bar(%lg) < 0", theta_bar);
			param->p[MAXPAR-2] = q; // phi incoming
			res = sasfit_qromb(0.0, M_PI, sasfit_ff_shearedCylinderGauss_core, param);
			break;
		default:
			SASFIT_ERR_UNKNOWN_KERNEL(param, "kernel");
			break;
	}
	return res;
}

