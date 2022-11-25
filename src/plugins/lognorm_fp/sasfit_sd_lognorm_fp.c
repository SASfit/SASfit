/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

// define shortcuts for local parameters/variables
#define FP	param->p[0]
#define SIGMA	fabs(param->p[1])
#define P	param->p[2]
#define MU	param->p[3]
#define LENGTH2	fabs(param->p[4])
#define LENGTH3	fabs(param->p[5])
#define SHAPE	param->p[6]
#define N param->p[MAXPAR-1]

scalar Xn(int n, sasfit_param *param) {
    return gsl_pow_int(MU,n)*exp(0.5*SIGMA*SIGMA*n*(2-2*P+n));
}

scalar sasfit_sd_lognorm_fp(scalar X, sasfit_param * param)
{
    scalar c, LNf;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((X < 0.0), param, "X(%lg) < 0",X);
	SASFIT_CHECK_COND1((SIGMA <= 0.0), param, "sigma(%lg) <= 0",SIGMA); // modify condition to your needs
	SASFIT_CHECK_COND1((P < 0.0), param, "p(%lg) < 0",P); // modify condition to your needs
	SASFIT_CHECK_COND1((MU <= 0.0), param, "mu(%lg) <= 0",MU); // modify condition to your needs
	SASFIT_CHECK_COND1((LENGTH2 < 0.0), param, "length2(%lg) < 0",LENGTH2); // modify condition to your needs
	SASFIT_CHECK_COND1((LENGTH3 < 0.0), param, "length3(%lg) < 0",LENGTH3); // modify condition to your needs

	// insert your code here

	switch (lround(SHAPE)) {
	    case 1:  // Sphere dR
	        N = 3./4.*FP/1E21/M_PI /Xn(3,param);
	        //exp(-3./2.*SIGMA*SIGMA*(-5.+2.*P));
	        break;
        case 2:  // Cylinder dR
	        N = FP/M_PI/1E21 / (Xn(2,param)*LENGTH2);
	        break;
        case 3:  // Cylinder dL
            N = FP/M_PI/1E21 / (Xn(1,param)*gsl_pow_2(LENGTH2));
	        break;
        case 4:  // Sph. Sh. iii dR;  volume fraction of core plus shell
            N = FP/4./M_PI/1E21 / (1./3.*Xn(3,param)+LENGTH2*Xn(2,param)+gsl_pow_2(LENGTH2)*Xn(1,param)+gsl_pow_3(LENGTH2)*Xn(0,param)/3.);
	        break;
        case 5:  // Sph. Sh. iii d DeltaR;  volume fraction of core plus shell
            N = FP/4./M_PI/1E21 / (1./3.*gsl_pow_3(LENGTH2)*Xn(0,param)+gsl_pow_2(LENGTH2)*Xn(1,param)+LENGTH2*Xn(2,param)+Xn(3,param)/3.);
	        break;
        case 6:  // Sph. Sh. iii dR;  volume fraction of core
            N = FP*3./4./M_PI/1E21/Xn(3,param);
	        break;
        case 7:  // Sph. Sh. iii d DeltaR;  volume fraction of core
            N = FP*3./4./M_PI/1E21/(gsl_pow_3(LENGTH2)*Xn(0,param));
	        break;
        case 8:  // Sph. Sh. iii d R;  volume fraction of shell
            N = FP/4./M_PI/1E21 / (LENGTH2*Xn(2,param)+gsl_pow_2(LENGTH2)*Xn(1,param)+gsl_pow_3(LENGTH2)*Xn(0,param)/3.);
	        break;
        case 9:  // Sph. Sh. iii d DeltaR;  volume fraction of shell
            N = FP/4./M_PI/1E21 / (gsl_pow_2(LENGTH2)*Xn(1,param)+LENGTH2*Xn(2,param)+Xn(3,param)/3.);
	        break;
        case 10: // CylShell1 dR;  volume fraction of core plus shell
            N = FP/M_PI/1E21 / (LENGTH3*(gsl_pow_2(LENGTH2)*Xn(0,param)+2*LENGTH2*Xn(1,param)+Xn(2,param)));
	        break;
        case 11: // CylShell1 d DeltaR;  volume fraction of core plus shell
            N = FP/M_PI/1E21 / (LENGTH3*(gsl_pow_2(LENGTH2)*Xn(0,param)+2*LENGTH2*Xn(1,param)+Xn(2,param)));
	        break;
        case 12: // CylShell1 d L;  volume fraction of core plus shell
            N = FP/M_PI/1E21 / (Xn(1,param)*(gsl_pow_2(LENGTH3)+2*LENGTH2*LENGTH3+gsl_pow_2(LENGTH2)));
	        break;
        case 13: // CylShell1 dR;  volume fraction of core
            N = FP/M_PI/1E21/(LENGTH3*Xn(2,param));
	        break;
        case 14: // CylShell1 d DeltaR;  volume fraction of core
            N = FP/M_PI/1E21/(LENGTH3*gsl_pow_2(LENGTH2)*Xn(0,param));
	        break;
        case 15: // CylShell1 dL;  volume fraction of core
            N = FP/M_PI/1E21/(gsl_pow_2(LENGTH2)*Xn(1,param));
	        break;
        case 16: // CylShell1 dR;  volume fraction of shell
            N = FP/M_PI/1E21/(LENGTH3*(gsl_pow_2(LENGTH2)*Xn(0,param)+2*Xn(1,param)*LENGTH2));
	        break;
        case 17: // CylShell1 d DeltaR;  volume fraction of shell
            N = FP/M_PI/1E21/(LENGTH3*(Xn(2,param)+2*LENGTH2*Xn(1,param)));
	        break;
        case 18: // CylShell1 dL;  volume fraction of shell
            N = FP/M_PI/1E21/(Xn(1,param)*(gsl_pow_2(LENGTH3)+2*LENGTH2*LENGTH3));
	        break;
        default:
            sasfit_err("UNKNOWN shape parameter, SHAPE in [1,18]\n");
	        return 0;
	}


	if (X == 0.0) return 0.0;

	c = 1.0/sqrt(M_PI*2.0)/SIGMA/pow(MU,1.0-P) / exp(0.5*pow(SIGMA*(1.0-P),2.0));
	LNf = N*c* pow(X,-P) * exp(-0.5*pow(log(X/MU)/SIGMA,2.0));

	return LNf;
	return 0.0;
}

scalar sasfit_sd_lognorm_fp_f(scalar X, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_sd_lognorm_fp_v(scalar X, sasfit_param * param, int dist)
{
    scalar Vav,V;
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
		switch (lround(SHAPE)) {
	    case 1:  // Sphere dR
	        Vav = 4./3.*M_PI *gsl_pow_3(X);
	        break;
        case 2:  // Cylinder dR
	        Vav = M_PI *gsl_pow_2(X)*LENGTH2;
	        break;
        case 3:  // Cylinder dL
            Vav = M_PI *gsl_pow_2(LENGTH2)*X;
	        break;
        case 4:  // Sph. Sh. iii dR;  volume fraction of core plus shell
            Vav = 4.*M_PI *(gsl_pow_2(X)*LENGTH2 + X*gsl_pow_2(LENGTH2)+1./3.*gsl_pow_3(LENGTH2)+1./3.*gsl_pow_3(X));
	        break;
        case 5:  // Sph. Sh. iii d DeltaR;  volume fraction of core plus shell
            Vav = 4.*M_PI *(gsl_pow_2(LENGTH2)*X + LENGTH2*gsl_pow_2(X)+1./3.*gsl_pow_3(X)+1./3.*gsl_pow_3(LENGTH2));
	        break;
        case 6:  // Sph. Sh. iii dR;  volume fraction of core
            Vav = 4./3.*M_PI *gsl_pow_3(X);
	        break;
        case 7:  // Sph. Sh. iii d DeltaR;  volume fraction of core
            Vav = 4./3.*M_PI *gsl_pow_3(LENGTH2);
	        break;
        case 8:  // Sph. Sh. iii d R;  volume fraction of shell
            Vav = 4*M_PI*(gsl_pow_2(X)*LENGTH2+X*gsl_pow_2(LENGTH2)+gsl_pow_3(LENGTH2)/3.);
	        break;
        case 9:  // Sph. Sh. iii d DeltaR;  volume fraction of shell
            Vav = 4*M_PI*(gsl_pow_2(LENGTH2)*X+LENGTH2*gsl_pow_2(X)+gsl_pow_3(X)/3.);
	        break;
        case 10: // CylShell1 dR;  volume fraction of core plus shell
            Vav = M_PI*LENGTH3*(gsl_pow_2(LENGTH2)+2*X*LENGTH2+gsl_pow_2(X));
	        break;
        case 11: // CylShell1 d DeltaR;  volume fraction of core plus shell
            Vav = M_PI*LENGTH3*(gsl_pow_2(LENGTH2)+2*X*LENGTH2+gsl_pow_2(X));
	        break;
        case 12: // CylShell1 d L;  volume fraction of core plus shell
            Vav = M_PI*X*(gsl_pow_2(LENGTH2)+2*LENGTH3*LENGTH2+gsl_pow_2(LENGTH3));
	        break;
        case 13: // CylShell1 dR;  volume fraction of core
            Vav = M_PI*LENGTH3*gsl_pow_2(X);
	        break;
        case 14: // CylShell1 d DeltaR;  volume fraction of core
            V = M_PI*LENGTH3*gsl_pow_2(LENGTH2) ;
	        break;
        case 15: // CylShell1 dL;  volume fraction of core
            Vav =  M_PI*X*gsl_pow_2(LENGTH2);
	        break;
        case 16: // CylShell1 dR;  volume fraction of shell
            Vav = M_PI*LENGTH3*(gsl_pow_2(LENGTH2)+2*X*LENGTH2);
	        break;
        case 17: // CylShell1 d DeltaR;  volume fraction of shell
            Vav = M_PI*LENGTH3*(gsl_pow_2(X)+2*LENGTH2*X);
	        break;
        case 18: // CylShell1 dL;  volume fraction of shell
            Vav = M_PI*X*(gsl_pow_2(LENGTH3)+2*LENGTH2*LENGTH3);
	        break;
        default:
            sasfit_err("UNKNOWN shape parameter, SHAPE in [1,18]\n");
	        return 0;
	}
}

