/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define DR	param->p[1]
#define LAM_SOL	param->p[2]
#define MC_RE	param->p[3]
#define MC_IM	param->p[4]
#define MS_RE	param->p[5]
#define MS_IM	param->p[6]
#define POL	param->p[7]

scalar sasfit_ff_mieshell(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((DR < 0.0), param, "dR(%lg) < 0",DR); // modify condition to your needs
	SASFIT_CHECK_COND1((LAM_SOL < 0.0), param, "lam_sol(%lg) < 0",LAM_SOL); // modify condition to your needs

	// insert your code here


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
	scalar lambda_solvent, mc_re, mc_im, ms_re, ms_im, pol;

	SASFIT_ASSERT_PTR(param);

    lambda_solvent = LAM_SOL;
    mc_re = MC_RE;
    mc_im = MC_IM;
    ms_re = MS_RE;
    ms_im = MS_IM;
    pol = POL;
	res = 0.0;
	Rc = (real)R;
	Rs = (real)(R+DR);


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

scalar sasfit_ff_mieshell_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_mieshell_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

