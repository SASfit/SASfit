/*
 * Author(s) of this file:
 *  Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *  10. Oct. 2017
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define D	param->p[1]
#define N   param->p[2]

#define P	param->p[5]
#define H	param->p[6]
#define ETA_B	param->p[7]

#define ETA_SOLV	param->p[9]

scalar hphi(scalar q, sasfit_param * param) {
	scalar V, u, F;
	u = q*R;
	if (fabs(u)<1e-6) {
		F = 1-0.1*u*u+1./280.*gsl_pow_4(u);
	} else {
		F = 3*(sin(u)-u*cos(u))/gsl_pow_3(u);
	}
	return F;
}

scalar hPsi2(scalar q, int j, sasfit_param * param) {
	scalar a;
	a = 2*M_PI*j/(P*q);
	if (1.0<=a*a) return 0.0;
	return gsl_pow_2(gsl_sf_bessel_Jn(j,q*D*0.5*sqrt(1-a*a)));
}

scalar sasfit_ff_beads_helix(scalar q, sasfit_param * param)
{
	scalar sum,sumold, prefactor,V;
	int i;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((H < 0.0), param, "H(%lg) < 0",H); // modify condition to your needs
	SASFIT_CHECK_COND1((D < 0.0), param, "D(%lg) < 0",D); // modify condition to your needs
	SASFIT_CHECK_COND1((P < 0.0), param, "P(%lg) < 0",P); // modify condition to your needs
	SASFIT_CHECK_COND1((N < 0.0), param, "N(%lg) < 0",N); // modify condition to your needs

	// insert your code here
	V = 3./4. * M_PI *gsl_pow_3(R);
	prefactor = gsl_pow_2(N*V/P *(ETA_B-ETA_SOLV)*hphi(q,param));
	sum = hPsi2(q,0,param);
	sumold=sum;
	for (i=1;i<q*P/(2*M_PI);i++) {
		sum = sum + 2*hPsi2(q,i,param);
		if (i>=1 && (fabs(sum-sumold)<sasfit_eps_get_nriq()*sum || i>NMAX || i>=sasfit_eps_get_jmax_nriq())) {
//			sasfit_out("fabs(sum-sumold)<eps*sum\t sum:%lg\t sum-sumold:%lg\t i:%d\n",sum,sum-sumold,i);
			break;
		}
		sumold=sum;
	}
//	return sum;
	return thinrod_helix(q,H)*prefactor*sum;
}

scalar sasfit_ff_beads_helix_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_beads_helix_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

