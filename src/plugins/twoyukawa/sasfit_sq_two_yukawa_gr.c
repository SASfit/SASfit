/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
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
#define Z1  param->p[MAXPAR-1]
#define Z2  param->p[MAXPAR-2]


scalar sasfit_sq_two_yukawa_gr(scalar r, sasfit_param * param)
{
    static scalar R_old=-1.23456789,
                  Z1_old=-1.23456789,
                  Z2_old=-1.23456789,
                  K1_old=-1.23456789,
                  K2_old=-1.23456789,
                  phi_old=-1.23456789;
    scalar numdens ;
    static scalar a, b, c1, c2, d1, d2, tmp;
    int i;
    double dr, dq, SqV[NR], grV[NR], rV[NR];
	static int debug=0, check,ok,grok;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

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
//	sasfit_out("%lf\n",r);
	if (K1_old!=K1 || K2_old!=K2 || Z1_old != Z1 || Z2_old != Z2 || phi_old!= PHI || R!=R_old) {
        K1_old = K1;
        K2_old = K2;
        Z1_old = Z1;
        Z2_old = Z2;
        phi_old = PHI;
        grok =0;
        ok = TY_SolveEquations( Z1, Z2, K1, K2, PHI, &a, &b, &c1, &c2, &d1, &d2, debug );
        if( ok ) {
            check = TY_CheckSolution( Z1, Z2, K1, K2, PHI, a, b, c1, c2, d1, d2 );
            dq=2e-3/R;
            for (i=0;i<NR;i++) SqV[i]=SqTwoYukawa((i+1)*dq*R*2.0, Z1, Z2, K1, K2, PHI, a, b, c1, c2, d1, d2);
            grok=PairCorrelation( PHI, dq, SqV, &dr, grV, NR );
            for (i=0;i<NR;i++) {
                    rV[i]=(i+1)*dr;
 //                   sasfit_out("%d %lf\t%lf\n",i, rV[i], SqV[i]);
            }

            if (grok) {
//                sasfit_out("try spline init\n");
                if (acc_spline==NULL) {
                        sasfit_out("acc_spline is not allocated, will do it now:\n");
                    acc_spline = gsl_interp_accel_alloc ();
                }
                if (grsteffen_T==NULL) {
                    sasfit_out("spline memory is not allocated, will do it now:\n");
                    grsteffen_T = gsl_spline_alloc (gsl_interp_steffen , NR);
                    //grlinear_T  = gsl_spline_alloc (gsl_interp_linear,   NR);
                }
                gsl_spline_init(grsteffen_T, rV, grV, NR);
 //               sasfit_out("Hope it worked.\n");
            }
        }
	}
    SASFIT_CHECK_COND1((ok==0), param, "Could not solve the equations for this set of parameters, ok(%d)\n",ok);
    if(grok) {		//less restrictive, if a solution found, return it, even if the equations aren't quite satisfied
        if (fabs(r)<dr) return 0;
        if (fabs(r)>NR*dr) return 1;
        return gsl_spline_eval (grsteffen_T, fabs(r), acc_spline);
	}
	SASFIT_CHECK_COND1((!grok), param, "Could not calculatre g(r), grok(%d)\n",grok);
}

scalar sasfit_sq_two_yukawa_gr_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sq_two_yukawa_gr_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

