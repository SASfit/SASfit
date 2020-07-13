/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define N	param->p[0]
#define D	param->p[1]
#define ETA	param->p[2]
#define NU	param->p[3]

scalar sasfit_sq_modifiedcaille(scalar q, sasfit_param * param)
{
    scalar tmp, SQ1, SQ2,qd,w;
    long NN,k,n;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((N < 0.0), param, "N(%lg) < 0",N); // modify condition to your needs
	SASFIT_CHECK_COND1((D < 0.0), param, "d(%lg) < 0",D); // modify condition to your needs
	SASFIT_CHECK_COND1((ETA < 0.0), param, "eta(%lg) < 0",ETA); // modify condition to your needs
	SASFIT_CHECK_COND1((NU < 0.0), param, "Nu(%lg) < 0",NU); // modify condition to your needs

	// insert your code here
    qd=q*D;
    NN = lround(floor(abs(N)));
	w = N-floor(N);

    SQ1=NN;SQ2=NN+1;
    for (k=1;k<NN;k++) {
        tmp=cos(k*qd)*exp(-pow(qd*0.5/M_PI,2.0)*ETA*(M_EULER+log(M_PI*k)));
        SQ1 = SQ1+2*(NN  -k)*tmp;
        SQ2 = SQ2+2*(NN+1-k)*tmp;
    }
    SQ2 = SQ2+2*cos(NN*qd)*exp(-pow(qd*0.5/M_PI,2.0)*ETA*(M_EULER+log(M_PI*NN)));
    return NU+(1-w)*SQ1+w*SQ2;
}

scalar sasfit_sq_modifiedcaille_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_modifiedcaille_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

