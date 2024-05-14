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

scalar sasfit_sq_perfect_ordered(scalar q, sasfit_param * param)
{
    scalar qd,qd2,N2,N4,N6,qd4,SQ,SQ1, SQ2,w;
    long NN,k;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
//	SASFIT_CHECK_COND1((N < 0.0), param, "N(%lg) < 0",N); // modify condition to your needs
	SASFIT_CHECK_COND1((D < 0.0), param, "d(%lg) < 0",D); // modify condition to your needs
	SASFIT_CHECK_COND1((DUMMY < 0.0), param, "dummy(%lg) < 0",DUMMY); // modify condition to your needs
	SASFIT_CHECK_COND1((NU < 0.0), param, "Nu(%lg) < 0",NU); // modify condition to your needs

	// insert your code here
	if (N < 0) return sasfit_sq_perfect_ordered_f(q,param);
    qd=q*D;
    NN = lround(floor(abs(N)));
	w = fabs(N)-floor(fabs(N));

    SQ1=NN;SQ2=NN+1;
    for (k=1;k<NN;k++) {
        SQ1 = SQ1+2*(NN-k)*cos(k*qd);
        SQ2 = SQ2+2*(NN+1-k)*cos(k*qd);
    }
    SQ2 = SQ2+2*cos(NN*qd);

    return NU+(1-w)*SQ1+w*SQ2;
}

scalar sasfit_sq_perfect_ordered_f(scalar q, sasfit_param * param)
{
    scalar qd,qd2,N2,N4,N6,qd4,SQ,SQ1, SQ2,w;
    long NN,k;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
    qd=q*D;
    qd2=qd*qd;
    qd4=qd2*qd2;

    NN = lround(floor(abs(N)));
	w = fabs(N)-floor(fabs(N));

	SQ1 = gsl_pow_2(sin(NN*qd/2.)/sin(qd/2.));
	if (!gsl_finite(SQ1)) {
        N2=NN*NN;
        N4=N2*N2;
        SQ1 = N2+(N2-N4)/12.*qd2+(3*N2-5*N4+2*N6)/720.*qd4;
    }

	SQ2 = gsl_pow_2(sin((NN+1)*qd/2.)/sin(qd/2.));
	if (!gsl_finite(SQ2)) {
        N2=(NN+1)*(NN+1);
        N4=N2*N2;
        SQ2 = N2+(N2-N4)/12.*qd2+(3*N2-5*N4+2*N6)/720.*qd4;
    }
    return NU+(1-w)*SQ1+w*SQ2;
}


scalar sasfit_sq_perfect_ordered_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

