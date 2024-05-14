/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define A	param->p[0]
#define B	param->p[2]
#define C	param->p[4]
#define ETA	param->p[6]

scalar sasfit_ff_ribbon_mono(scalar q, sasfit_param * param)
{
    scalar rod, xs;
    sasfit_param subParam;
    scalar Res1F2, pFq_r, pFq_i;
	scalar pr[1],pi[1],qr[2], qi[2], zi, zr;
	int ln_pFq, ix, nsigfig, ip, iq;
	long int  errorstr_len=256;
	bool error;
	char errorstr[256];

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	subParam.p[0] = C;
	subParam.p[3] = ETA;
	rod = sasfit_ff_rod(q,&subParam);

	ln_pFq = 0;
	ix = 0;
	nsigfig = 10;
	ip = 1;
	iq = 2;

	pi[0] = 0;
	pr[0] = 0.5;
	qi[0] = 0;
	qr[0] = 1.5;
	qi[1] = 0;
	qr[1] = 2;

	zi=0;
	zr=-gsl_pow_2(q*B/2.);

	if (q*B/2. > 3) {
        Res1F2 = 2.0/(B*q);
	} else if (q*B/2. < 1e-4) {
	    Res1F2 = 1-gsl_pow_2(q*B)/24+gsl_pow_4(q*B)/960.;
	} else {
	    Res1F2 = sasfit_pfq(pr,pi,qr,qi,ip,iq,zr,zi,ln_pFq,ix,&pFq_r,&pFq_i,nsigfig,param);
	}
	xs = gsl_pow_2(A*B*gsl_sf_bessel_j0(q*A*0.5))*Res1F2;
	return rod*xs;
}

scalar sasfit_ff_ribbon_mono_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_ribbon_mono_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

