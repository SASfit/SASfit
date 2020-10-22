/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sq.h>

// define shortcuts for local parameters/variables
#define R	param->p[0]
#define K1	param->p[1]
#define LAMBDA1	param->p[2]
#define K2	param->p[3]
#define LAMBDA2	param->p[4]
#define PHI	param->p[5]
#define MOLARITY	param->p[6]

scalar sasfit_sq_two_yukawa(scalar q, sasfit_param * param)
{
    static scalar Z1_old=-1.23456789,Z2_old=-1.23456789,K1_old=-1.23456789,K2_old=-1.23456789,phi_old=-1.23456789;
    scalar numdens,Z1, Z2;
    static scalar a, b, c1, c2, d1, d2, tmp;
	static int debug=0, check,ok;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((LAMBDA1 <= 0.0), param, "lambda1(%lg) < 0",LAMBDA1); // modify condition to your needs
	SASFIT_CHECK_COND1((LAMBDA2 <= 0.0), param, "lambda2(%lg) < 0",LAMBDA2); // modify condition to your needs
	// insert your code here
	if (MOLARITY > 0) {
        numdens = GSL_CONST_NUM_AVOGADRO*MOLARITY*1e-24;
        PHI = 4./3.*M_PI*gsl_pow_3(R)*numdens;
	} else {
	    PHI = fabs(PHI);
	    numdens = PHI / (4 / 3. * M_PI * gsl_pow_3(R));
	    MOLARITY = numdens/(GSL_CONST_NUM_AVOGADRO*1e-24);
	}
	Z1 = 1./LAMBDA1;
	Z2 = 1./LAMBDA2;
    if(fabs(Z1) < 0.001) Z1 = 0.001;			// values near zero are very bad for the calculation
	if(fabs(Z2) < 0.001) Z2 = 0.001;			// values near zero are very bad for the calculation
	if(fabs(K1) < 0.001) K1 = 0.001;			// values near zero are very bad for the calculation
	if(fabs(K2) < 0.001) K2 = 0.001;
	if (Z2 > Z1) {
        tmp=Z1;
        Z1=Z2;
        Z2=tmp;
        tmp=K1;
        K1=K2;
        K2=tmp;
	}
	if (fabs(Z1-Z2) < 0.001) {
        K2=K1+K2;
        K1=0.001*K2;
        Z1=2*Z2;
	}
	if (K1_old!=K1 || K2_old!=K2 || Z1_old != Z1 || Z2_old != Z2 || phi_old!= PHI) {
        K1_old = K1;
        K2_old = K2;
        Z1_old = Z1;
        Z2_old = Z2;
        phi_old = PHI;
        ok = TY_SolveEquations( Z1, Z2, K1, K2, PHI, &a, &b, &c1, &c2, &d1, &d2, debug );
        if( ok ) {
            check = TY_CheckSolution( Z1, Z2, K1, K2, PHI, a, b, c1, c2, d1, d2 );
//		if(debug) {
//			sprintf(buf, "solution = (%g, %g, %g, %g, %g, %g) check = %d\r", a, b, c1, c2, d1, d2, check );
//			XOPNotice(buf);
//		}
        }
	}
    if(ok) {		//less restrictive, if a solution found, return it, even if the equations aren't quite satisfied
		return SqTwoYukawa(q*R*2.0, Z1, Z2, K1, K2, PHI, a, b, c1, c2, d1, d2);
	}
	SASFIT_CHECK_COND1((ok==0), param, "Could not solve the equations for this set of parameters, ok(%d)\n",ok);
}

scalar sasfit_sq_two_yukawa_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_two_yukawa_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

