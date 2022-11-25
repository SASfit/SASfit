/*
 * src/sasfit_sq/sasfit_sq_StickyHardSphere.c
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
#include "include/sasfit_sq_utils.h"

/*
double S_StickyHardSphere(Tcl_Interp *interp,
			double Q,
			double RHS,
			double tau,
			double fp,
			bool  *error)
*/

/**
 * structure factor of particles interacting with a sticky hard-sphere
 * potential
 * RHS: hards sphere radius
 * fp: hard sphere volume fraction
 * tau: stickyness
 * [Percus-Yevick approximation]
 */
scalar sasfit_sq_StickyHardSphere(scalar q, sasfit_param * param)
{
	scalar SQ, CQ, kappa, gama, lamb, mu, epsi, beta, alpha;
	scalar RHS, tau, fp;

	SASFIT_ASSERT_PTR( param );

	sasfit_get_param(param, 3, &RHS, &tau, &fp);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((RHS < 0.0), param, "RHS(%lg) < 0",RHS);
	SASFIT_CHECK_COND1((fp < 0.0), param, "fp(%lg) < 0",fp);
	SASFIT_CHECK_COND1((fp >= 1.0), param, "fp(%lg) >= 1",fp);

	if (fp == 0.0)
	{
		return 1.0;
	}

	kappa = 2.0*q*RHS;
	epsi = tau+(fp/(1.0-fp));
	gama = fp*(1+fp/2.0)/(3.0*pow(1.0-fp,2.0));

	SASFIT_CHECK_COND1((epsi*epsi-gama<0),param,"tau(%lg) is too small",tau);
    if (epsi*epsi-gama>=0) {
        lamb = 6.0/fp*(epsi-sqrt(epsi*epsi-gama));
    } else {
        lamb = 6.0/fp*(epsi);
    }
	mu = lamb*fp*(1-fp);
	beta = -(3.*fp*pow(2.+fp,2.0)-2.*mu*(1.+7.*fp+fp*fp)+mu*mu*(2.+fp)) / (2.*pow(1.-fp,4.));
	alpha = pow(1.+2.*fp-mu,2.)/pow(1-fp,4.);
	CQ = -24.*fp*pow(kappa,-6.)*
			  ( alpha*pow(kappa,3.)*(sin(kappa)-kappa*cos(kappa))
			 +beta*pow(kappa,2.)*(2.*kappa*sin(kappa)-(pow(kappa,2.)-2.)*cos(kappa)-2.)
			 +0.5*fp*alpha*((4.*pow(kappa,3.)-24.*kappa)*sin(kappa)-(pow(kappa,4.)-12.*pow(kappa,2.)+24)*cos(kappa)+24)
			)
		-2*pow(fp*lamb,2.)*(1.-cos(kappa))*pow(kappa,-2.)
		+2.*fp*lamb/kappa*sin(kappa);
	SQ = 1. / (1.-CQ);

	return SQ;
}
