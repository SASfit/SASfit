/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define D	param->p[0]
#define N	param->p[1]
#define QD  param->p[MAXPAR-1]

scalar SQ_N_random_flight(long NN, sasfit_param * param)
{
	scalar sincQD;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid


	if (fabs(QD) < 1e-3) {
		return NN+(1./18. - NN*NN/18.)*QD*QD + ((4. - 5.*NN - 4*NN*NN + 5*NN*NN*NN)*gsl_pow_4(QD))/2160.;
	}

	sincQD=gsl_sf_sinc(QD/M_PI);
	return 2./(1.-sincQD) -1 -(2*(1-gsl_pow_int(sincQD,NN)))/(NN*gsl_pow_2(1-sincQD))*sincQD;
}

scalar sasfit_sq_random_flight(scalar q, sasfit_param * param)
{
	scalar w;
	long NN;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((D < 0.0), param, "D(%lg) < 0",D); // modify condition to your needs
	SASFIT_CHECK_COND1((N < 1.0), param, "N(%lg) < 1",N); // modify condition to your needs

	// insert your code here
	QD=q*D;
	NN = lround(floor(N));
	w = N-floor(N);

	return (1-w)*SQ_N_random_flight(NN,param)+w*SQ_N_random_flight(NN+1,param);
}

scalar sasfit_sq_random_flight_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_random_flight_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

