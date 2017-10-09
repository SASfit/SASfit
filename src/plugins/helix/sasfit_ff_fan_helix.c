/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define A	param->p[1]
#define P	param->p[2]
#define H	param->p[3]
#define OMEGA	param->p[4]*M_PI/180
#define PHI	param->p[5]*M_PI/180

#define Q param->p[MAXPAR-1]
#define QN param->p[MAXPAR-2]
#define	N param->p[MAXPAR-3]

scalar bessfct(scalar r, sasfit_param * param) 
{
	return r* gsl_sf_bessel_Jn(lround(N),Q*r/sqrt(1.0-QN*QN));
}

scalar gn2(scalar q, int n, sasfit_param * param)
{
	scalar qn,b,u,BessInt;
	b=2*M_PI*R/P;
	u=q*R;
	qn=n*b/u;
	if (qn > 1.0) return 0.0;
	Q=q;
	QN = qn;
	N=n;
//	sasfit_out("lround(N): %d, Q: %lf\n",lround(N), Q);
	BessInt = sasfit_integrate(A*R,R,bessfct,param);
	return gsl_pow_2(2.0/(R*R*(1.0-A*A))*BessInt);
}

scalar prefct(int n, sasfit_param * param)
{
	return gsl_pow_2(cos(n*PHI*0.5)*sin(n*OMEGA*0.5)/(n*OMEGA*0.5));
}

scalar sasfit_ff_pringle_schmidt_helix(scalar q, sasfit_param * param)
{
	scalar prefac, sum;
	int n;
	n=0;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((A <= 0.0), param, "a(%lg) <= 0",A); // modify condition to your needs
	SASFIT_CHECK_COND1((A >= 1.0), param, "a(%lg) >= 1",A); // modify condition to your needs
	SASFIT_CHECK_COND1((P <= 0.0), param, "P(%lg) < 0",P); // modify condition to your needs
	SASFIT_CHECK_COND1((H < 0.0), param, "H(%lg) < 0",H); // modify condition to your needs
	SASFIT_CHECK_COND1((OMEGA <= 0.0), param, "omega(%lg) < 0",OMEGA); // modify condition to your needs
	SASFIT_CHECK_COND1((PHI < 0.0), param, "phi(%lg) < 0",PHI); // modify condition to your needs

	// insert your code here
	sum = gn2(q,0,param);
	for (n=1;n<q*P/(2*M_PI);n++) {
//		sasfit_out("n: %d\n",n);
		sum = sum+2*prefct(n,param)*gn2(q,n,param);
	}
	return M_PI/(q*H)*sum;
}

scalar sasfit_ff_pringle_schmidt_helix_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_pringle_schmidt_helix_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

