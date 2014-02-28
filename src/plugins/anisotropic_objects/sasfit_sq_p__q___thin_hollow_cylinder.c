/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define R0	param->p[0]
#define H0	param->p[1]
#define SIGMA_R	fabs(param->p[2])
#define SIGMA_H	fabs(param->p[3])

#define Q		param->p[MAXPAR-1]
#define	R		param->p[MAXPAR-2]
#define	H		param->p[MAXPAR-3]
#define ALPHA	param->p[MAXPAR-4]


scalar XI_CylShell(scalar x, sasfit_param * param)
{
	scalar xR, xH, XIres;

	SASFIT_ASSERT_PTR(param);

	xR = x*R*sin(ALPHA);
	xH = x*H*cos(ALPHA)/2.;

	if ((R+H) == 0) return 0;

	if (xR == 0)
	{
		XIres = R/(R+H) * cos(xH);
	} else
	{
		XIres = R/(R+H) * 2.*gsl_sf_bessel_J1(xR)/xR * cos(xH);
	}

	if (xH == 0)
	{
		XIres = XIres + H/(R+H) * gsl_sf_bessel_J0(xR);
	} else
	{
		XIres = XIres + H/(R+H) * gsl_sf_bessel_J0(xR)*sin(xH)/xH;
	}
	return (2.* M_PI*R*R + 2.0*M_PI*R*H)*XIres;
}


scalar thinCyl_P_core(scalar x, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param);

	ALPHA = x;

	return  gsl_pow_2(XI_CylShell(Q, param)) * sin(ALPHA);
}

scalar thinCyl_P(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param);

	return sasfit_integrate(0.0, M_PI/2.0, thinCyl_P_core, param);
}



scalar ThinCylShell_H_core(scalar xH, sasfit_param * param)
{
	scalar P, LNdistr;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	H = xH;

	P = thinCyl_P(Q,param);

	sasfit_init_param( &subParam );

	subParam.p[0] = 1.0;
	subParam.p[1] = SIGMA_H;
	subParam.p[2] = 1.0;
	subParam.p[3] = H0;

	LNdistr = sasfit_sd_LogNorm(xH, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	return LNdistr*P;
}

scalar ThinCylShell_H(scalar q,sasfit_param * param)
{
	scalar Lstart = 0.0, Lend = 0.0;

	static scalar Q_old = -1.;
	static scalar R_old = -1.;
	static scalar sigma_H_old = -1.;
	static scalar H_old = -1.;
	static scalar Pprime = -1.;

	SASFIT_ASSERT_PTR(param);

	if ((q != Q_old) || (R != R_old) || (SIGMA_H != sigma_H_old) || (H !=H_old))
	{
//		Pprime = thinCyl_P(Q, param);
//		goto out;
		if (SIGMA_H == 0.0)
		{
			H = H0;
			Pprime = thinCyl_P(Q, param);
		} else
		{
			find_LogNorm_int_range(2,H0,SIGMA_H,&Lstart,&Lend,param);
			Pprime 	= sasfit_integrate(Lstart, Lend, &ThinCylShell_H_core, param);
		}

out:
		Q_old	= q;
		R_old	= R0;
		sigma_H_old = SIGMA_H;
		H_old	= H0;
	}
	return Pprime;
}

scalar ThinCylShell_core(scalar xR, sasfit_param * param)
{
	scalar P, LNdistr;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	R = xR;
	P = ThinCylShell_H(Q,param);

	sasfit_init_param( &subParam );

	subParam.p[0] = 1.0;
	subParam.p[1] = SIGMA_R;
	subParam.p[2] = 1.0;
	subParam.p[3] = R0;

	LNdistr = sasfit_sd_LogNorm(xR, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	return LNdistr*P;
}

scalar ThinCylShell(scalar q, sasfit_param * param)
{
	scalar xstart = 0.0, xend = 0.0;

	static scalar Q_old = -1.;
	static scalar R0_old = -1.;
	static scalar sigma_R_old = -1.;
	static scalar H_old = -1.;
	static scalar Pprime = 1.;

	SASFIT_ASSERT_PTR(param);

	Q = q;

	if ((q != Q_old) || (R0 != R0_old) || (SIGMA_R != sigma_R_old) || (H !=H_old) )
	{
		if (SIGMA_R == 0.0)
		{
			R = R0;
			H = H0;
			Pprime = ThinCylShell_H(Q,param);;
		} else
		{
			find_LogNorm_int_range(2,R0,SIGMA_R,&xstart,&xend,param);
			Pprime 	= sasfit_integrate(xstart, xend, ThinCylShell_core, param);
		}

		Q_old	= q;
		R0_old	= R0;
		H_old	= H0;
		sigma_R_old = SIGMA_R;
	}

	return Pprime;
}
scalar sasfit_sq_p__q___thin_hollow_cylinder(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((H < 0.0), param, "H(%lg) < 0",H); // modify condition to your needs

	// insert your code here
	return ThinCylShell(q,param);
}

scalar sasfit_sq_p__q___thin_hollow_cylinder_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_p__q___thin_hollow_cylinder_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

