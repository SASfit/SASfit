/*
 * src/plugins/triax_ellip_shell/sasfit_ff_triax_ellip_shell_sq_baba_ahmed_2.c
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

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define A	param->p[0]
#define DELTA_B	param->p[1]
#define DELTA_C	param->p[2]
#define T	param->p[3]
#define ETA_C	param->p[4]
#define ETA_SH_DRY	param->p[5]
#define ETA_SOL	param->p[6]
#define PHI	param->p[7]
#define V_POL_AP	param->p[8]
#define ALPHA	param->p[9]
#define Z_P	param->p[10]
#define EPSILON_R	param->p[11]
#define MB	param->p[12]
#define ZB	param->p[13]
#define TEMP	param->p[14]

scalar sasfit_ff_triax_ellip_shell_sq_baba_ahmed_2_core(scalar y, sasfit_param * param)
{
	scalar q, p, x, b, c, u_c, u_sh, eta_sh, f_c, f_sh;

	SASFIT_ASSERT_PTR(param);

	q	= param->p[MAXPAR-1];
	p	= param->p[MAXPAR-2]; // pow
	x	= param->p[MAXPAR-3]; // param from outer integration
	b	= param->p[MAXPAR-4];
	c	= param->p[MAXPAR-5];
	eta_sh	= param->p[MAXPAR-6];

	if (fabs(A*b*c) + fabs(T) == 0.0) return 0.0;

	u_c = q*sqrt(( pow(A*cos(x*M_PI/2.),2.) + pow(b*sin(x*M_PI/2.),2.) )*(1.-y*y) + pow(c*y,2.));
	u_sh = q*sqrt(( pow((A+T)*cos(x*M_PI/2.),2.) + pow((b+T)*sin(x*M_PI/2.),2.) ) * (1.-y*y) + pow((c+T)*y,2.));

	f_c  = 4./3.*M_PI*A*b*c*(ETA_C-eta_sh);
	if (u_c != 0.0)
	{
		f_c  = f_c * 3*(sin(u_c) -u_c *cos(u_c) )/pow(u_c ,3);
	}

	f_sh  = 4./3.*M_PI*(A+T)*(b+T)*(c+T)*(eta_sh-ETA_SOL);
	if (u_sh != 0.0) 
	{
		f_sh  = f_sh * 3*(sin(u_sh)-u_sh*cos(u_sh))/pow(u_sh,3);
	}

	return pow(f_sh+f_c, p);
}
scalar sasfit_ff_triax_ellip_shell_sq_baba_ahmed_2_core_x(scalar x, sasfit_param * param)
{
	scalar res;

	SASFIT_ASSERT_PTR(param);

	param->p[MAXPAR-3] = x;

	res = sasfit_integrate(0.0, 1.0, sasfit_ff_triax_ellip_shell_sq_baba_ahmed_2_core, param);

	return res;
}

scalar sasfit_ff_triax_ellip_shell_sq_baba_ahmed_2(scalar q, sasfit_param * param)
{
	scalar res, b, c, w, eta_sh, n;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	DELTA_B = fabs(DELTA_B);
	DELTA_C = fabs(DELTA_C);
	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((A < 0.0), param, "a(%lg) < 0",A);
	SASFIT_CHECK_COND1((DELTA_B < 0.0), param, "delta_b(%lg) < 0",DELTA_B);
	SASFIT_CHECK_COND1((DELTA_C < 0.0), param, "delta_c(%lg) < 0",DELTA_C);
	SASFIT_CHECK_COND1((T < 0.0), param, "t(%lg) < 0",T);
	SASFIT_CHECK_COND1((PHI < 0.0), param, "phi(%lg) < 0",PHI);
	SASFIT_CHECK_COND1((V_POL_AP < 0.0), param, "v_pol/v_ap (%lg) < 0",V_POL_AP);
//	SASFIT_CHECK_COND1((ETA_C < 0.0), param, "eta_c(%lg) < 0",ETA_C); // modify condition to your needs
//	SASFIT_CHECK_COND1((ETA_SH < 0.0), param, "eta_sh(%lg) < 0",ETA_SH); // modify condition to your needs
//	SASFIT_CHECK_COND1((ETA_SOL < 0.0), param, "eta_sol(%lg) < 0",ETA_SOL); // modify condition to your needs

	param->p[MAXPAR-1] = q;	  // Q
	param->p[MAXPAR-2] = 2.0; // pow
	
	b = A + DELTA_B;
	c = b + DELTA_C;
	param->p[MAXPAR-4] = b;
	param->p[MAXPAR-5] = c;

	w = 1.0 - ((V_POL_AP * A * b * c)/((A+T)*(b+T)*(c+T) - A*b*c));
	SASFIT_CHECK_COND1((w < 0.0), param, "w(%lg) < 0",w);
	eta_sh = (1.0-w)*ETA_SH_DRY + w*ETA_SOL;
	param->p[MAXPAR-6] = eta_sh;

	n = PHI/((1.0+V_POL_AP)*4./3.*M_PI*A*b*c);

	res = sasfit_integrate(0.0, 1.0, sasfit_ff_triax_ellip_shell_sq_baba_ahmed_2_core_x, param);
	
	sasfit_init_param(&subParam);
	subParam.p[0] = ALPHA * pow(A*b*c, 1.0/3.0); // R_ref
	subParam.p[1] = ALPHA;
	subParam.p[2] = Z_P;
	subParam.p[3] = EPSILON_R;
	subParam.p[4] = MB;
	subParam.p[5] = ZB;
	subParam.p[6] = PHI * gsl_pow_3(ALPHA); // vol frac
	subParam.p[7] = TEMP;

	return n*res*sasfit_sq_baba_ahmed_syl(q, &subParam);
}

scalar sasfit_ff_triax_ellip_shell_sq_baba_ahmed_2_f(scalar q, sasfit_param * param)
{
	// insert your code here
	return 0.0;
}

scalar sasfit_ff_triax_ellip_shell_sq_baba_ahmed_2_v(scalar q, sasfit_param * param, int dist)
{
	scalar b, c;

	SASFIT_ASSERT_PTR(param);

	b = A + DELTA_B;
	c = b + DELTA_C;

	switch ( dist )
	{
		case 0:
			q = (q + T) * (b + T) * (c + T);
			break;
		case 1:
			q = (A + T) * (q + T) * (c + T);
			break;
		case 2:
			q = (A + T) * (b + T) * (q + T);
			break;
		case 3:
			q = (A + q) * (b + q) * (c + q);
			break;
		default:
			q = (A + T) * (b + T) * (c + T);
			break;
	}
	return 4.0/3.0 * M_PI * q;
}

