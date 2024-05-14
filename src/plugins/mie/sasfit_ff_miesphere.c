/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define LAM_SOL	param->p[1]
#define M_RE	param->p[2]
#define M_IM	param->p[3]
#define POL	param->p[4]

scalar sasfit_ff_miesphere(scalar q, sasfit_param * param)
{   real res, Theta,xx[1];
	complex crefin[1];
	logical perfct;
	real mimcut;
	logical anyang;
	integer numang;
	integer nmom;
	real xmu[1];
	integer ipolzn[1];
	real pmom[44];
	integer c__10;
	logical prnt[2] = { FALSE_,FALSE_ };
	real qext, qsca, gqsc,spike;
	complex sforw,sback,tforw[2],tback[2];
	complex s1[1],s2[1];
	real i1,i2;
	scalar Qmax,pol_ang;
	scalar lambda_solvent, m_re, m_im, pol;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs

	// insert your code here



	SASFIT_ASSERT_PTR(param);

	lambda_solvent=LAM_SOL;
	m_re = M_RE;
	m_im = M_IM;
	pol = POL;

	c__10 = 10;
	res = 0.0;

	SASFIT_CHECK_COND1((lambda_solvent < 0.0), param, "lambda_solvent(%lg) < 0",lambda_solvent);

	Qmax = (4.0*M_PI/lambda_solvent);

	SASFIT_CHECK_COND2((q == Qmax), param, "q(%lg) == Qmax(%lg)",q,Qmax);

	Theta = (real)(2.0*asin(q*lambda_solvent/(4.0*M_PI)));
	xx[0] = (real)(2.0*M_PI*R/lambda_solvent);
	xmu[0] = (real) cos(Theta);

	SASFIT_CHECK_COND1((xmu[0] > 1.0f) || (xmu[0] < -1.0f), param, "xmu[0] is out of range (%lg)",xmu[0]);
	SASFIT_CHECK_COND1((xx[0] > 2e4f), param, "xx[0](%lg) > 2e4f",xx[0]);
	SASFIT_CHECK_COND1((m_re <= 0.0), param, "m_re(%lg) <= 0",m_re);
	SASFIT_CHECK_COND1((fabs(m_re) >= 10), param, "fabs(m_re)(%lg) >= 10",fabs(m_re));
	SASFIT_CHECK_COND1((fabs(m_im) >= 10), param, "fabs(m_im)(%lg) >= 10",fabs(m_im));

	crefin[0].r = (real) m_re;
	crefin[0].i = (real) m_im;
	perfct = FALSE_;
	mimcut = 1e-6f;
	anyang = TRUE_;
	numang = 1;
	nmom = 0;

	sasfit_miev0(&xx[0], &crefin[0], &perfct, &mimcut, &anyang,
			&numang, &xmu[0], &nmom, &ipolzn[0], &c__10,
			prnt, &qext, &qsca, &gqsc,
			pmom, &sforw, &sback,
			s1, s2, tforw, tback, &spike);

	i1 = s1[0].r*s1[0].r + s1[0].i*s1[0].i;
	i2 = s2[0].r*s2[0].r + s2[0].i*s2[0].i;

	pol_ang = (1.0+pol)*M_PI/4.0;
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

scalar sasfit_ff_miesphere_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_miesphere_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

