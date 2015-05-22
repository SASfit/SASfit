/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables
#define FP  param->p[0]
#define SIGMA   param->p[1]
#define P   param->p[2]
#define MU  param->p[3]
#define LENGTH2 param->p[4]
#define LENGTH3 param->p[5]
#define SHAPE   param->p[6]

scalar X(int n,sasfit_param * param) {
    return pow(MU,n)*exp(0.5*SIGMA*SIGMA*n*(2.-2.*P+n));
}

scalar sasfit_sd_lognorm_fp(scalar q, sasfit_param * param)
{
    scalar R, DeltaR, L;
	scalar c, N, LNf;
    
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

    SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
    SASFIT_CHECK_COND1((FP < 0.0), param, "fp(%lg) < 0",FP); // modify condition to your needs
    SASFIT_CHECK_COND1((SIGMA < 0.0), param, "sigma(%lg) < 0",SIGMA); // modify condition to your needs
    SASFIT_CHECK_COND1((MU < 0.0), param, "mu(%lg) < 0",MU); // modify condition to your needs
    SASFIT_CHECK_COND1((LENGTH2 < 0.0), param, "length2(%lg) < 0",LENGTH2); // modify condition to your needs
    SASFIT_CHECK_COND1((LENGTH3 < 0.0), param, "length3(%lg) < 0",LENGTH3); // modify condition to your needs
    SASFIT_CHECK_COND1((SHAPE < 0.0), param, "shape(%lg) < 0",SHAPE); // modify condition to your needs

    // insert your code here


        if (q == 0.0) return 0.0;

        c = 1.0/sqrt(M_PI*2.0)/SIGMA/pow(MU,1.0-P) / exp(0.5*pow(SIGMA*(1.0-P),2.0));


        switch ((int) SHAPE) {
            case 1: N = FP*1e-21*3./(4.*M_PI   * X(3,param));
                    break;
            case 2: L = LENGTH2;
                    N = FP*1e-21  / (M_PI*X(2,param)*L);
                    break;
            case 3: R = LENGTH2;
                    N = FP*1e-21  / (M_PI*X(1,param)*gsl_pow_2(R));
                    break;
            case 4: DeltaR = LENGTH2;
                    N = FP*1e-21/(4.*M_PI* ( X(3,param)/3.
                                            +X(2,param)*DeltaR
                                            +X(1,param)*gsl_pow_2(DeltaR)
                                            +X(0,param)*gsl_pow_3(DeltaR)/3.
                                           )
                                 );
                    break;
            case 5: R = LENGTH2;
                    N = FP*1e-21/(4.*M_PI* (  gsl_pow_3(R)*X(0,param)/3.
                                            + gsl_pow_2(R)*X(1,param)
                                            +           R *X(2,param)
                                            +              X(3,param)/3.
                                           )
                                 );
                    break;
            case 6: DeltaR = LENGTH2;
                    N = FP*1e-21/(4.*M_PI*X(3,param)/3.);
                    break;
            case 7: R = LENGTH2;
                    N = FP*1e-21/(4.*M_PI*gsl_pow_3(R)*X(0,param)/3.);
                    break;
            case 8: DeltaR = LENGTH2;
                    N = FP*1e-21  / (4.*M_PI* ( X(2,param)*DeltaR
                                               +X(1,param)*gsl_pow_2(DeltaR)
                                               +X(0,param)*gsl_pow_3(DeltaR)/3.
                                              )
                                    );
                    break;
            case 9: R = LENGTH2;
                    N = FP*1e-21  / (4.*M_PI* ( gsl_pow_2(R)*X(1,param)
                                                      +   R *X(2,param)
                                                      +      X(3,param)/3.
                                              )
                                    );
                    break;
           case 10: DeltaR = LENGTH2;
                    L = LENGTH3;
                    N = FP*1e-21/(M_PI* L*(  X(2,param)
                                           + X(1,param)*2.       *DeltaR
                                           + X(0,param)*gsl_pow_2(DeltaR)
                                          )
                                 );
                    break;
           case 11: R = LENGTH2;
                    L = LENGTH3;
                    N = FP*1e-21/(M_PI* L*( gsl_pow_2(R)    *X(0,param)
                                           +          R * 2.*X(1,param)
                                           +                 X(2,param)
                                          )
                                 );
                    break;
           case 12: R = LENGTH2;
                    DeltaR = LENGTH3;
                    N = FP*1e-21/(M_PI* (  gsl_pow_2(R)
                                         + 2*R*DeltaR
                                         + gsl_pow_2(DeltaR)
                                        ) * X(1,param)
                                 );
                    break;
           case 13: DeltaR = LENGTH2;
                    L = LENGTH3;
                    N = FP*1e-21/(M_PI* X(2,param) * L);
                    break;
           case 14: R = LENGTH2;
                    L = LENGTH3;
                    N = FP*1e-21/(M_PI* gsl_pow_2(R) * X(2,param) * L);
                    break;
           case 15: R = LENGTH2;
                    DeltaR = LENGTH3;
                    N = FP*1e-21/(M_PI* gsl_pow_2(R) * X(1,param) );
                    break;
           case 16: DeltaR = LENGTH2;
                    L = LENGTH3;
                    N = FP*1e-21/(M_PI* L* ( gsl_pow_2(DeltaR)*X(0,param)
                                            +2*DeltaR         *X(1,param)
                                           )
                                 );
                    break;
           case 17: R = LENGTH2;
                    L = LENGTH3;
                    N = FP*1e-21/(M_PI* L* (     X(2,param)
                                            +2*R*X(1,param)
                                           )
                                 );
                    break;
           case 18: R = LENGTH2;
                    DeltaR = LENGTH3;
                    N = FP*1e-21/(M_PI* X(1,param)* ( gsl_pow_2(DeltaR)
                                                     +2*DeltaR*R
                                           )
                                 );
                    break;
            default: N=FP;
        }

        LNf = N*c* pow(q,-P) * exp(-0.5*gsl_pow_2(log(q/MU)/SIGMA));

    return LNf;



    return 0.0;
}

scalar sasfit_sd_lognorm_fp_f(scalar q, sasfit_param * param)
{
    SASFIT_ASSERT_PTR(param); // assert pointer param is valid

    // insert your code here
    return 0.0;
}

scalar sasfit_sd_lognorm_fp_v(scalar q, sasfit_param * param, int dist)
{
    SASFIT_ASSERT_PTR(param); // assert pointer param is valid

    // insert your code here
    return 0.0;
}

