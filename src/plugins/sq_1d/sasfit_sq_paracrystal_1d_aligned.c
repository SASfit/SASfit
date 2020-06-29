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
#define Q param->p[MAXPAR-1]

scalar sasfit_sq_paracrystal_1d_aligned(scalar q, sasfit_param * param)
{
	scalar a, b, B, B2,B4, Bn, tmp, ferf,SQN, SQN1, rw1, iw1, rw2, iw2, a2b, w, psi, cosgamma,gamma;
	int k,tn;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((D < 0.0), param, "D(%lg) < 0",D); // modify condition to your needs
	SASFIT_CHECK_COND1((NN < 1.0), param, "N(%lg) < 1",NN); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA < 0.0), param, "sigma(%lg) < 0",SIGMA); // modify condition to your needs

	// insert your code here

	psi   = sasfit_param_override_get_psi(PSI_DEG * M_PI/180.);
	cosgamma = ( cos(psi)*cos(THETA_DEG*M_PI/180.)
                +sin(psi)*sin(THETA_DEG*M_PI/180.)*cos(PHI_DEG*M_PI/180.));
    gamma = acos(gamma);
	Q = q;
    tn=lround(floor(NN));
    w = NN-floor(NN);
    tmp = q*cosgamma;

    b = q*q*SIGMA*SIGMA*0.5;
    a = D*q;
    if (sqrt(gsl_pow_2(q*cosgamma)*(D*D*tn+2*SIGMA*SIGMA))<0.1) {
        SQN  = tn - ((-1 + gsl_pow_2(tn))*gsl_pow_2(q)*(gsl_pow_2(D)*tn + 2*gsl_pow_2(SIGMA))*
                    gsl_pow_2(cosgamma))/12. + ((-1 + tn*tn)*gsl_pow_4(q)*
                    (gsl_pow_4(D)*tn*(-3 + 2*gsl_pow_2(tn)) +
                    6*gsl_pow_2(D)*(-2 + 3*gsl_pow_2(tn))*gsl_pow_2(SIGMA) + 15*tn*gsl_pow_4(SIGMA))*
                    gsl_pow_4(cosgamma))/720.;
        tn++;
        SQN1 = tn - ((-1 + gsl_pow_2(tn))*gsl_pow_2(q)*(gsl_pow_2(D)*tn + 2*gsl_pow_2(SIGMA))*
                    gsl_pow_2(cosgamma))/12. + ((-1 + tn*tn)*gsl_pow_4(q)*
                    (gsl_pow_4(D)*tn*(-3 + 2*gsl_pow_2(tn)) +
                    6*gsl_pow_2(D)*(-2 + 3*gsl_pow_2(tn))*gsl_pow_2(SIGMA) + 15*tn*gsl_pow_4(SIGMA))*
                    gsl_pow_4(cosgamma))/720.;
        return (1-w)*SQN+w*SQN1;
    }
    B = exp(-b*gsl_pow_2(cosgamma));
    B2 = gsl_pow_2(B);
    B4 = gsl_pow_2(B2);
    Bn = pow(B,tn);
    SQN = (4*B2 + tn - B4*tn +
            2*B*((-1 + B2*(-1 + tn) - tn)*cos(a*cosgamma) +
                Bn*(B2*cos(a*(-1 + tn)*cosgamma) -
                2*B*cos(a*tn*cosgamma) + cos(a*(1 + tn)*cosgamma))))/
                    (tn*gsl_pow_2(1 + B2 - 2*B*cos(a*cosgamma)));;
    tn=tn+1;
	SQN1 = (SQN-1)*(tn-1)/tn+1;
    k=tn-1;
    SQN1 = SQN1+2.0/tn * (tn-k)*cos(k*D*tmp) * exp(-k*gsl_pow_2(tmp*SIGMA)/2.0);
	if (gsl_finite((1-w)*SQN+w*SQN1)) return (1-w)*SQN+w*SQN1;
	tn--;
    if (fabs(gamma-M_PI_2)<1e-2) {
        SQN  = tn - ((4*b + a*a*tn)*(-1 + tn*tn)*gsl_pow_2(M_PI - 2*gamma))/48.;
        tn++;
        SQN1 = tn - ((4*b + a*a*tn)*(-1 + tn*tn)*gsl_pow_2(M_PI - 2*gamma))/48.;
        return (1-w)*SQN+w*SQN1;
    }
//	If nothing else works
	return sasfit_sq_paracrystal_orig_1d_aligned_f(q, param);
}

scalar sasfit_sq_paracrystal_1d_aligned_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_paracrystal_1d_aligned_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

