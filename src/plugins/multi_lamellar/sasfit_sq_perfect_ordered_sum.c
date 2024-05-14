/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define N	param->p[0]
#define D	param->p[1]
#define DUMMY	param->p[2]
#define NU	param->p[3]
#define Q   param->p[MAXPAR-1]
#define SIGMA_SD    param->p[MAXPAR-2]
#define NMEAN    param->p[MAXPAR-3]


scalar sasfit_sq_perfect_ordered_sum(scalar q, sasfit_param * param)
{
    scalar q2, SQ1, SQ2,SQ1sum,SQ2sum,qd,w;
    long NN,j,k,n;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
//	SASFIT_CHECK_COND1((N < 0.0), param, "N(%lg) < 0",N); // modify condition to your needs
	SASFIT_CHECK_COND1((D < 0.0), param, "d(%lg) < 0",D); // modify condition to your needs
	SASFIT_CHECK_COND1((DUMMY < 0.0), param, "dummy(%lg) < 0",DUMMY); // modify condition to your needs
	SASFIT_CHECK_COND1((NU < 0.0), param, "Nu(%lg) < 0",NU); // modify condition to your needs

	// insert your code here

    qd=q*D;
    NN = lround(floor(abs(N)));
	w = abs(N)-floor(abs(N));

    if (fabs(N)>=5) {
        SIGMA_SD = sqrt(fabs(N));
    } else {
        SIGMA_SD = 0.5*(fabs(N)-1);
    }
    SQ1sum=0;SQ2sum=0;
    for (n=lround(NN-2*SIGMA_SD);n<=lround(NN+2*SIGMA_SD);n++) {
        SQ1=n;SQ2=n+1;
        for (k=1;k<n;k++) {
            SQ1 = SQ1+2*(n  -k)*cos(k*qd);
            SQ2 = SQ2+2*(n+1-k)*cos(k*qd);
        }
        SQ2 = SQ2+2*cos(n*qd);
        SQ1sum=SQ1sum + 1./(SIGMA_SD*sqrt(2*M_PI))*exp(-gsl_pow_2((n-NN)/SIGMA_SD)/2.)*SQ1;
        SQ2sum=SQ2sum + 1./(SIGMA_SD*sqrt(2*M_PI))*exp(-gsl_pow_2((n-NN)/SIGMA_SD)/2.)*SQ2;
    }

    return NU+(1-w)*SQ1sum+w*SQ2sum;
}

scalar sasfit_sq_perfect_ordered_sum_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_perfect_ordered_sum_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

;
