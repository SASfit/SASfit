/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define D	param->p[0]
#define SIGMA	param->p[1]
#define NN	param->p[2]
#define THETA_DEG param->p[3]
#define PHI_DEG param->p[4]
#define PSI_DEG param->p[5]
#define COSGAMMA param->p[MAXPAR-2]
#define Q param->p[MAXPAR-1]

scalar sasfit_sq_paracrystal_orig_1d_aligned(scalar q, sasfit_param * param)
{
	scalar SQN,SQN1, rw1, iw1, rw2, iw2, a2b, w, psi, cosgamma;
	int k,tn;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((D < 0.0), param, "D(%lg) < 0",D); // modify condition to your needs
	SASFIT_CHECK_COND1((NN < 1.0), param, "N(%lg) < 1",NN); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA < 0.0), param, "sigma(%lg) < 0",SIGMA); // modify condition to your needs

	// insert your code here


	psi   = sasfit_param_override_get_psi(PSI_DEG * M_PI/180.);
	COSGAMMA = ( cos(psi)*cos(THETA_DEG*M_PI/180.)
                +sin(psi)*sin(THETA_DEG*M_PI/180.)*cos(PHI_DEG*M_PI/180.));
    Q = q;
	return sasfit_sq_paracrystal_orig_1d_aligned_f(q,param);
}

scalar sasfit_sq_paracrystal_orig_1d_aligned_f(scalar q, sasfit_param * param)
{
	scalar SQN,SQN1, w, psi, gamma, tmp;
	int k,tn;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	tmp = Q*COSGAMMA;
    tn=lround(floor(NN));
    w = NN-floor(NN);
	SQN = 1.0;
    for (k=1; k < tn ;k++) {
		SQN = SQN+2.0/tn * (tn-k)*cos(k*D*tmp) * exp(-0.5*k*gsl_pow_2(tmp*SIGMA));
	}
    tn=lround(floor(NN))+1;
	SQN1 = (SQN-1)*(tn-1)/tn+1;
    k=tn-1;
    SQN1 = SQN1+2.0/tn * (tn-k)*cos(k*D*tmp) * exp(-0.5*k*gsl_pow_2(tmp*SIGMA));
	return (1-w)*SQN+w*SQN1;
}

scalar sasfit_sq_paracrystal_orig_1d_aligned_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

