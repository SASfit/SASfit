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


#define PHI	param->p[5]
#define MOLARITY	param->p[6]

scalar sasfit_sq_one_yukawa(scalar q, sasfit_param * param)
{
    static scalar Z1_old=-1.23456789,Z2_old=-1.23456789,K1_old=-1.23456789,K2_old=-1.23456789, phi_old=-1.23456789;
    scalar numdens,Z1, Z2;
    static scalar a, b, c, d, tmp;
	static int debug=0, check,ok;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R); // modify condition to your needs
	SASFIT_CHECK_COND1((LAMBDA1 <= 0.0), param, "lambda1(%lg) < 0",LAMBDA1); // modify condition to your needs
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
    if(fabs(Z1) < 0.001) Z1 = 0.001;			// values near zero are very bad for the calculation
	if(fabs(K1) < 0.001) K1 = 0.001;			// values near zero are very bad for the calculation

	if (K1_old!=K1 || Z1_old != Z1 || phi_old!= PHI) {
        K1_old = K1;
        Z1_old = Z1;
        phi_old = PHI;
        ok = Y_SolveEquations( Z1, K1, PHI, &a, &b, &c, &d, debug );
        if( ok ) {
            check = Y_CheckSolution( Z1, K1, PHI, a, b, c, d );
//		if(debug) {
//			sprintf(buf, "solution = (%g, %g, %g, %g) check = %d\r", a, b, c, d, check )
//			XOPNotice(buf);
//		}
        }
	}
    if(ok) {		//less restrictive, if a solution found, return it, even if the equations aren't quite satisfied
		return SqOneYukawa(q*R*2.0, Z1, K1, PHI, a, b, c, d);
	} else {
	    sasfit_err("Could not solve the equations for this set of parameters\n");
		return TCL_ERROR;
	}
}

scalar sasfit_sq_one_yukawa_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_one_yukawa_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}
