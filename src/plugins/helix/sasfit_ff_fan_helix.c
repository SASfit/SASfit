/*
 * Author(s) of this file:
 *  Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *  10. Oct. 2017
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define A	param->p[1]
#define OMEGA	(param->p[2]*M_PI/180.)
#define PHI	(param->p[3]*M_PI/180.)

#define DUMMY param->p[4]

#define P	param->p[5]
#define H	param->p[6]
#define ETA_H	param->p[7]

#define ETA_SOLV	param->p[9]

#define Q param->p[MAXPAR-1]
#define QN param->p[MAXPAR-2]
#define	N param->p[MAXPAR-3]

scalar bessfct(scalar r, sasfit_param * param)
{
	return r*gsl_sf_bessel_Jn(lround(N),Q*r*sqrt(fabs(1.0-QN*QN)));
}

scalar gn2(scalar q, int n, sasfit_param * param)
{
	scalar qn,b,u,BessInt;
	b=2*M_PI*R/P;
	u=q*R;
	if (u==0.0) return 0.0;
	qn=n*b/u;
	GSL_MIN(qn,1.0);
	Q=q;
	QN = qn;
	N=n;
	BessInt = sasfit_integrate(A*R,R,bessfct,param);
	return gsl_pow_2(BessInt);
}

scalar prefct(int n, sasfit_param * param)
{
	return gsl_pow_2(cos(n*PHI*0.5)*sinc(n*OMEGA*0.5));
}

scalar sasfit_ff_fan_helix(scalar q, sasfit_param * param)
{
	scalar prefac, sum, sumold;
	int n,twice;
	n=0;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((A < 0.0), param, "a(%lg) < 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((A >= 1.0), param, "a(%lg) >= 1",A); // modify condition to your needs
	SASFIT_CHECK_COND1((P <= 0.0), param, "P(%lg) < 0",P); // modify condition to your needs
	SASFIT_CHECK_COND1((H < 0.0), param, "H(%lg) < 0",H); // modify condition to your needs

	// insert your code here
	prefac =  gsl_pow_2(2.0/(R*R*(1.0-A*A)));
	sum = prefac*gn2(q,0,param);
	sumold = sum;

	twice = 0;
	for (n=1;n<q*P/(2*M_PI);n++) {
		sum = sum + 2.0*prefac*prefct(n,param)*gn2(q,n,param);
		if (n>=1 && (fabs(sum-sumold)<sasfit_eps_get_nriq()*sum || n>NMAX )) {
			twice++;
			if (twice >= 2) break;
		} else {
			twice = 0;
		}
		sumold=sum;
	}
	return thinrod_helix(q,H)*sum*gsl_pow_2(sasfit_ff_fan_helix_v(R,param,0)*(ETA_H-ETA_SOLV));
    // thinrod_helix(q,H) is replacing pi/(q*H) in original paper to account for finite length of helix
    // lim_{q->infty} thinrod_helix(q,H) = H^2  * pi/(q*H) = H*pi/q
}

scalar sasfit_ff_fan_helix_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_fan_helix_v(scalar x, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	param->p[dist] = x;
	return R*R*(1.0-A*A)*OMEGA*H;
}

