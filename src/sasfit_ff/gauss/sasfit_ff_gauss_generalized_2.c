/*
 * src/sasfit_ff/gauss/sasfit_ff_gauss_generalized_2.c
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

#include "include/private.h"

#define RG			param->p[0]
#define NU			param->p[1]
#define B_P			param->p[2]
#define VOLUME		param->p[3]
#define ETA_S		param->p[4]


scalar sasfit_ff_gauss_generalized_2(scalar q, sasfit_param * param)
{
	scalar u,i0,beta;

	SASFIT_ASSERT_PTR(param);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
    SASFIT_CHECK_COND1((RG <= 0.0), param, "RG(%lg) < 0",RG);
	SASFIT_CHECK_COND1((NU <= 0.0), param, "nu(%lg) <= 0",NU);
	SASFIT_CHECK_COND1((NU > 1.0), param, "nu(%lg) > 1",NU);

	u = (2.0*NU+1.0)*(2.0*NU+2.0)*q*q*RG*RG/6.0;
	beta = B_P - VOLUME*ETA_S;
	i0 = beta*beta;
	if (q*RG<0.1) return i0*exp(-q*q*RG*RG/3);
//    if (u>1000.0){
//	    return i0*(
//	        gsl_sf_gamma(1.0/(2.*NU))/(NU*pow(u,1.0/(2.0*NU))) 
//		  - gsl_sf_gamma(1.0/NU) /(NU*pow(u,1.0/NU))
//		  );
//	} else {
	    return i0*
	       (pow(u,1/(2.*NU)) * gsl_sf_gamma(1.0/(2.*NU)) 
		                     - gsl_sf_gamma(1.0/NU)
		  - pow(u,1/(2.*NU)) * gsl_sf_gamma_inc(1.0/(2.*NU),u) + 
                               gsl_sf_gamma_inc(1.0/NU,u)
			)/(NU*pow(u,1/NU));
//	}
}
