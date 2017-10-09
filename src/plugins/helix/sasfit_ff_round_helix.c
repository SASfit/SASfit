/*
 * Author(s) of this file:
 *  Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *  10. Oct. 2017
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R1	param->p[0]
#define DELTA1	param->p[1]
#define R2	param->p[2]
#define DELTA2	param->p[3]
#define ALPHA	param->p[4]*M_PI/180.
#define P	param->p[5]
#define H	param->p[6]

#define NMAX 100

scalar twoJ1x_x(scalar x) {
	if (fabs(x)<1e-6) {
		return 1-x*x/8.0+gsl_pow_4(x)/192.-gsl_pow_6(x)/9216.;
	} else  {
		return 2*gsl_sf_bessel_J1(x)/x;
	}
}

scalar sasfit_ff_round_helix(scalar q, sasfit_param * param)
{
	scalar Qperp, sum,sumold, a1n, a2n;
	int n;
	
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R1 < 0.0), param, "r1(%lg) < 0",R1); // modify condition to your needs
	SASFIT_CHECK_COND1((R2 < 0.0), param, "r2(%lg) < 0",R2); // modify condition to your needs
	SASFIT_CHECK_COND1((DELTA1 < 0.0), param, "delta_1(%lg) < 0",DELTA1); // modify condition to your needs
	SASFIT_CHECK_COND1((DELTA2 < 0.0), param, "delta_2(%lg) < 0",DELTA2); // modify condition to your needs
	SASFIT_CHECK_COND1((H < 0.0), param, "H(%lg) < 0",H); // modify condition to your needs
	SASFIT_CHECK_COND1((P < 0.0), param, "P(%lg) < 0",P); // modify condition to your needs

	// insert your code here
	sum = 0.0;
	sumold = 0.0;
	for (n=0;n<q*P/(2*M_PI) && n<NMAX;n++) {
//		sasfit_out("q: %lf  n: %d\n",q, n);
		Qperp = sqrt(fabs(q*q-gsl_pow_2(2*M_PI*n/P)));
		a1n = gsl_sf_bessel_Jn(n,DELTA1*Qperp)*twoJ1x_x(R1*Qperp)*R1*R1*M_PI;
		a2n = gsl_sf_bessel_Jn(n,DELTA2*Qperp)*twoJ1x_x(R2*Qperp)*R2*R2*M_PI;
		sum = sum+a1n*a1n+a2n*a2n+2.*a1n*a2n*cos(n*ALPHA);
		if (n>1 && fabs(sum-sumold)<1e-6*sum) {
			sasfit_out("fabs(sum-sumold)<1e-6*sum, sum:%lf, n:%d\n",sum,n);
			break;
		}
		sumold=sum;
	}
	return sum;
}

scalar sasfit_ff_round_helix_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_round_helix_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

