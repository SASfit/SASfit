/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define D	param->p[0]
#define N	param->p[1]
#define DELTA	param->p[2]
#define QD  param->p[MAXPAR-1]
#define QDelta  param->p[MAXPAR-2]
#define Q  param->p[MAXPAR-3]

scalar SQ_N_pc_random_flight(long NN, sasfit_param * param)
{
	scalar sincQD, DD2, DELTA2,NN2;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid
	if (fabs(QD) < 1e-3) {
		DD2 = D*D;
		DELTA2 = DELTA*DELTA;
		NN2 = N*N;
		return NN - ((DD2 + 3*DELTA2)*(-1 + NN2)*Q*Q)/18. + 
		  ((-1 + NN)*(1 + NN)*(-4*DD2*DD2 + 5*gsl_pow_2(DD2 + 3*DELTA2)*NN)*gsl_pow_4(Q))/2160.;
	}

	sincQD=gsl_sf_sinc(QD/M_PI);

	return (exp(gsl_pow_2(QDelta))*NN*gsl_pow_2(QD) +
     sin(QD)*(-(NN*sin(QD)) + 2*QD*
         (-exp(gsl_pow_2(QDelta)/2.) +
           gsl_pow_int(sincQD,NN)/
            exp((gsl_pow_2(QDelta)*(-1 + NN))/2.))))/
   (NN*gsl_pow_2(-(exp(gsl_pow_2(QDelta)/2.)*QD) + sin(QD)));

}

scalar sasfit_sq_pc_random_flight(scalar q, sasfit_param * param)
{
	scalar w;
	long NN;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((D < 0.0), param, "D(%lg) < 0",D); // modify condition to your needs
	SASFIT_CHECK_COND1((DELTA <= 0.0), param, "DELTA(%lg) <= 0",DELTA); // modify condition to your needs
	SASFIT_CHECK_COND1((N < 1.0), param, "N(%lg) < 1",N); // modify condition to your needs

	// insert your code here
	Q=q;
	QD=q*D;
	QDelta = q*DELTA;
	NN = lround(floor(N));
	w = N-floor(N);

	return (1-w)*SQ_N_pc_random_flight(NN,param)+w*SQ_N_pc_random_flight(NN+1,param);
}

scalar sasfit_sq_pc_random_flight_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_pc_random_flight_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

