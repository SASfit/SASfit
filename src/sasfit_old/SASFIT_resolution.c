/*
 * src/sasfit_old/SASFIT_resolution.c
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
 */

/*#########################################################################*/
/*#                                                                       #*/
/*#                                                                       #*/
/*# Written by Joachim Kohlbrecher                                        #*/
/*#########################################################################*/
/*#     Version Vb1.0 12.12.2004  Joachim Kohlbrecher                     #*/
/*#########################################################################*/

#include <stdio.h>
#include <stdlib.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_sf_bessel.h>
#include <tcl.h>

#include "include/SASFIT_nr.h"
#define float double

/*#########################################################################*/
/*#                                                                       #*/
/*#########################################################################*/
float Rav(q,q_av,sigma_t)
float q,q_av,sigma_t;
{
	float ax = 0.0, result = 0.0;
	if (sigma_t == 0.0) {
		if (q == q_av) {
			result = 1.0;
		} else {
			result = 0.0;
		}
	} else {
		ax=fabs(q*q_av/sigma_t/sigma_t);
		if (ax < GSL_LOG_DBL_MAX-1) // GSL tests against this (~718)
		{                           // specfunc/bessel_I0.c (l. 206)
			result = q/sigma_t/sigma_t 
			         * exp(-0.5*(q*q+q_av*q_av)/sigma_t/sigma_t)
			         * gsl_sf_bessel_I0(ax);
		} else {
			// for large arguments I_alpha(x) approximately exp(x)/sqrt(2 pi x)(1+(1-2alpha)(1+2alpha)/(8x))
			result = 1.0/(sqrt(2*M_PI)*sigma_t)*exp(-0.5*gsl_pow_2((q-q_av)/sigma_t)) * sqrt(q/q_av)
				*(1. + 3675./(32768.*gsl_pow_4(ax)) 
				    + 75./(1024.*gsl_pow_3(ax)) 
					+ 9./(128.*gsl_pow_2(ax)) 
					+ 1./(8.*ax)
				    + 59535./(262144.*gsl_pow_5(ax)) 
					+ 2401245./(4194304.*gsl_pow_6(ax)) 
					+ 57972915./(33554432.*gsl_pow_7(ax))
					+ 13043905875./(2147483648.*gsl_pow_8(ax))
					+ 418854310875./(17179869184.*gsl_pow_9(ax))
					);
		}
	}
	return result;
}

/*#########################################################################*/
/*#                                                                       #*/
/*#########################################################################*/
float sigma_t(lam,Dlam,r1,L,r2,l,Dd,D,q_av)
float lam;   /* wavelength in reciprocal units of q_av, i.e. nm			*/
float Dlam;  /* wavelength spread in the same units than lam, i.e. nm	*/
float r1;    /* source aperture size (radius)  in mm					*/
float L;     /* collimation length in mm								*/
float r2;    /* sample aperture size (radius)  in mm					*/
float l;     /* sample detector distance in mm							*/
float Dd;    /* step width for radial average in mm						*/
float D;     /* detector resolution in mm								*/
float q_av;  /* average Q value in 1/nm									*/
{
	float k_av, theta_av, theta2_av, a1, a2, Dbeta1, Dbeta2,
		  sigma_W, sigma_C1, sigma_C2, sigma, sigma_D1, sigma_D2, sigma_av;

	k_av      = 2.0*M_PI/lam;
    theta_av  = asin(q_av/(2.0*k_av));
	theta2_av = 2.0*theta_av;
	a1        = r1/(L+l/pow(cos(theta2_av),2.0));
	a2        = r2*pow(cos(theta2_av),2.0)/l;
	if (a1 >= a2) {
		Dbeta1 = 2.0*r1/L - 0.5*r2*r2/r1 * pow(cos(theta2_av),4.0)/(l*l*L)
			              * pow(L+l/pow(cos(theta2_av),2.0),2.0);
	} else {
        Dbeta1 =   2.0*r2*(1.0/L + pow(cos(theta2_av),2.0)/l) 
                 - 0.5*r1*r1/r2 * l/L 
				 / (pow(cos(theta2_av),2.0)*(L+l/pow(cos(theta2_av),2.0)));
	}
	if (a1 >= a2) {
		Dbeta2 = 2.0*r1/L - 0.5*r2*r2/r1 * pow(cos(theta2_av),2.0)/(l*l*L)
			              * pow(L+l/cos(theta2_av),2.0);
	} else {
        Dbeta2 =   2.0*r2*(1.0/L + cos(theta2_av)/l) 
                 - 0.5*r1*r1/r2 * l/L 
				 / (cos(theta2_av)*(L+l/cos(theta2_av)));
	}
	sigma_W  = q_av*Dlam/lam / (2.0*sqrt(2.0*log(2.0)));
	sigma_C1 = k_av*cos(theta_av)*Dbeta1/(2.0*sqrt(2.0*log(2.0)));
	sigma_C2 = k_av*              Dbeta2/(2.0*sqrt(2.0*log(2.0)));
	sigma    = sqrt(sigma_W*sigma_W + sigma_C1*sigma_C1);
	sigma_D1 = k_av*cos(theta_av)*pow(cos(theta2_av),2.0)*D/l  / (2.0*sqrt(2.0*log(2.0)));
	sigma_D2 = k_av*                  cos(theta2_av)     *D/l  / (2.0*sqrt(2.0*log(2.0)));
	sigma_av = k_av*cos(theta_av)*pow(cos(theta2_av),2.0)*Dd/l / (2.0*sqrt(2.0*log(2.0)));
	return sqrt(sigma*sigma+sigma_D1*sigma_D1+sigma_av*sigma_av);
}
